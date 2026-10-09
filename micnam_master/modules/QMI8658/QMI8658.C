#include "QMI8658.h"
#include "bsp.h"
#include "math.h"
#include "main.h"

void I2C_WriteReg(uint8_t reg, uint8_t val)
{
//    mysoftIIC_Write(QMI8658_ADDR, reg, val);
	u8 p[2]={reg,val};
	I2C2_send_data(QMI8658_ADDR,p,2);
	while (i2c_wait_end(&hi2cy, 0xFFFFFFF) != I2C_OK)
		;
}
uint8_t I2C_ReadReg(uint8_t reg)
{
//    return mysoftIIC_read(QMI8658_ADDR, reg);
	u8 p=reg,get;
	I2C2_send_data(QMI8658_ADDR,&p,1);
	while (i2c_wait_end(&hi2cy, 0xFFFFFFF) != I2C_OK)
		;
	I2C2_read_data(QMI8658_ADDR,&get,1);
	while (i2c_wait_end(&hi2cy, 0xFFFFFFF) != I2C_OK)
		;
	return get;
}
//寄存器,读取到的数据保存到p[] 读取寄存器数量
void I2C_ReadRegs(uint8_t reg,u8 *p,u8 number)
{
//	I2C_ReadNbyte(QMI8658_ADDR,reg,p,number);
	I2C2_send_data(QMI8658_ADDR,&reg,1);
	while (i2c_wait_end(&hi2cy, 0xFFFFFFF) != I2C_OK)
		;
	I2C2_read_data(QMI8658_ADDR,p,number);
	while (i2c_wait_end(&hi2cy, 0xFFFFFFF) != I2C_OK)
		;
}



void qmi8658c_init(void)
{
    uint8_t id = 0;
//	delay_ms(10);
	vTaskDelay(pdMS_TO_TICKS(10));  
    id=I2C_ReadReg(QMI8658C_WHO_AM_I);
    while (id != 0x05)
    {
        id=I2C_ReadReg(QMI8658C_WHO_AM_I);
//        delay_ms(50);
		vTaskDelay(pdMS_TO_TICKS(50));  
    }
    I2C_WriteReg(QMI8658C_RESET, 0xb0); // 复位
	vTaskDelay(pdMS_TO_TICKS(10));
//	delay_ms(10);
    I2C_WriteReg(QMI8658C_CTRL1, 0x40); // CTRL1 设置地址自动增加
    I2C_WriteReg(QMI8658C_CTRL7, 0x03); // CTRL7 允许加速度和陀螺仪
    I2C_WriteReg(QMI8658C_CTRL2, 0x95); // CTRL2 设置ACC 4g 250Hz
    I2C_WriteReg(QMI8658C_CTRL3, 0xd5); // CTRL3 设置GRY 512dps 250Hz
	I2C_WriteReg(QMI8658C_CTRL5, 0x00); // 关低功耗，保证 250 Hz 全速
}

Imu6DOF_t q;
void qmi8658c_Read_AccAndGry( Imu6DOF_t *p )
{
    uint8_t status, data_ready = 0;
    int16_t buf[6];
	u8 bu[12];
    status=I2C_ReadReg(QMI8658C_STATUS0); // 读状态寄存器
    if (status & 0x03)                                    // 判断加速度和陀螺仪数据是否可读
    {
        data_ready = 1;
    }
    if (data_ready == 1)
    {
        data_ready = 0;
		I2C_ReadRegs(QMI8658C_AX_L,bu,12);
		buf[0]=bu[0]+(bu[1]<<8);
		buf[1]=bu[2]+(bu[3]<<8);
		buf[2]=bu[4]+(bu[5]<<8);
		buf[3]=bu[6]+(bu[7]<<8);
		buf[4]=bu[8]+(bu[9]<<8);
		buf[5]=bu[10]+(bu[11]<<8);
        p->acc_x = buf[0];
        p->acc_y = buf[1];
        p->acc_z = buf[2];
        p->gyr_x = buf[3];
        p->gyr_y = buf[4];
        p->gyr_z = buf[5];
    }
}

#include "can_wheel.h"

/* 6 轴最优融合 - QMI8658 纯手写版 */
#define ACC_SCALE  (0.000122f)
#define GYR_SCALE  (0.015625f)
#define DT        0.003f          // IMU 积分周期 3ms, 必须严格遵循任务周期
#define KP         (0.8f)
#define KI         (0.0001f)
#define ACC_TRUST_TH   (0.3f)		// 信任门限：> ACC_TRUST_TH g 完全不信
#define STILL_CNT      (50)		//0.004*50 = 0.2s

static float gyr_bias[3] = {0};
static float q0 = 1.0f, q1 = 0, q2 = 0, q3 = 0;
static float intg_err_x = 0, intg_err_y = 0, intg_err_z = 0;

static inline float vec3_norm(float x, float y, float z)
{
    return sqrtf(x * x + y * y + z * z);
}

void qmi8658c_fetch_angleFromAcc(Imu6DOF_t *p)
{
    /* 1. 物理单位 */
    float ax = p->acc_x * ACC_SCALE;
    float ay = p->acc_y * ACC_SCALE;
    float az = p->acc_z * ACC_SCALE;
    float gx = (p->gyr_x - gyr_bias[0]) * GYR_SCALE * 0.0174533f;
    float gy = (p->gyr_y - gyr_bias[1]) * GYR_SCALE * 0.0174533f;
    float gz = (p->gyr_z - gyr_bias[2]) * GYR_SCALE * 0.0174533f;

    /* 2. 静止自校零 + 残差一次性清零 */
	static float sum[3] = {0}, cnt = 0;
	static uint8_t zeroed = 0;                   // 只清一次残差

	float acc_mag = fabsf(vec3_norm(ax, ay, az) - 1.0f);
	float gyr_mag = vec3_norm(gx, gy, gz);

	if (acc_mag < 0.02f && gyr_mag < 2.0f) {
		sum[0] += p->gyr_x; sum[1] += p->gyr_y; sum[2] += p->gyr_z; cnt++;
	}
	if (cnt >= STILL_CNT) 
	{
		if (!zeroed) {                           // 只清一次残差
			intg_err_z = 0;	//清z轴
			zeroed   = 1;
		}
	} else {
		zeroed = 0;                              // 下次静止可再清
	}
    /* 3. 归一化加速度 */
    float norm = vec3_norm(ax, ay, az);
    if (norm > 0.8f) { ax /= norm; ay /= norm; az /= norm; }

    /* 4. 重力误差 */
    float vx = 2.0f*(q1*q3 - q0*q2);
    float vy = 2.0f*(q0*q1 + q2*q3);
    float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
    float ex = ay*vz - az*vy;
    float ey = az*vx - ax*vz;
    float ez = ax*vy - ay*vx;

    /* 5. 信任门限：击打/跳跃时衰减 KP */
    float accErr = fabsf(norm - 1.0f);
    float trust  = (accErr > ACC_TRUST_TH) ? 0.0f : (1.0f - accErr / ACC_TRUST_TH);
    ex *= trust; ey *= trust; ez *= trust;

    /* 6. PI 补偿 */
    intg_err_x += ex * KI * DT;
    intg_err_y += ey * KI * DT;
    intg_err_z += ez * KI * DT;
    gx += KP*ex + intg_err_x;
    gy += KP*ey + intg_err_y;
    gz += KP*ez + intg_err_z;

    /* 7. 四元数积分 */
    float qDot0 = -0.5f*(q1*gx + q2*gy + q3*gz);
    float qDot1 =  0.5f*(q0*gx + q2*gz - q3*gy);
    float qDot2 =  0.5f*(q0*gy - q1*gz + q3*gx);
    float qDot3 =  0.5f*(q0*gz + q1*gy - q2*gx);
    q0 += qDot0 * DT; q1 += qDot1 * DT; q2 += qDot2 * DT; q3 += qDot3 * DT;

    /* 8. 归一化 */
    norm = sqrtf(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 /= norm; q1 /= norm; q2 /= norm; q3 /= norm;

    /* 9. 输出四元数 & 欧拉角 */
    p->q0 = q0; p->q1 = q1; p->q2 = q2; p->q3 = q3;
    p->AngleX = atan2f(2.0f*(q0*q1 + q2*q3), 1.0f - 2.0f*(q1*q1 + q2*q2)) * 57.29578f;
    p->AngleY = asinf(2.0f*(q0*q2 - q3*q1)) * 57.29578f;
    p->AngleZ = atan2f(2.0f*(q0*q3 + q1*q2), 1.0f - 2.0f*(q2*q2 + q3*q3)) * 57.29578f;
}


