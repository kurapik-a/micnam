#ifndef QMI8658_H   
#define QMI8658_H


#include "at32f435_437.h"               // Device header
#include "at32f435_437_board.h"

#define QMI8658_ADDR        (0XD6)   // 8-bit HAL 地址		地址控制线接高电平的时候的地址

#define QMI8658_WHO_AM_I    0x00        // 固定返回 0x05
#define QMI8658_CTRL1       0x0B        // 加速度计使能
#define QMI8658_CTRL2       0x0C        // 陀螺仪使能
#define QMI8658_ACC_CFG     0x0D        // 加速度量程/ODR
#define QMI8658_GYR_CFG     0x0E        // 陀螺仪量程/ODR

#define QMI8658C_WHO_AM_I 0x00
#define QMI8658C_RESET 0x60
#define QMI8658C_STATUS0 0x2e	//状态

#define QMI8658C_CTRL1 0x02
#define QMI8658C_CTRL2 0x03
#define QMI8658C_CTRL3 0x04
#define QMI8658C_CTRL4 5
#define QMI8658C_CTRL5 6
#define QMI8658C_CTRL6 7
#define QMI8658C_CTRL7 0x08


#define QMI8658C_AX_L 0x35	//AXL


/* 传感器数据结构体 */
typedef struct {
    float acc_x;	// 加速度 X
    float acc_y;	// 加速度 Y
    float acc_z;	// 加速度 Z
    float gyr_x;	// 陀螺仪 X
    float gyr_y;	// 陀螺仪 Y
    float gyr_z;	// 陀螺仪 Z
	float q0;		//四元数
	float q1;
	float q2;
	float q3;
	float AngleX;	//欧拉角
	float AngleY;
	float AngleZ;
} Imu6DOF_t;

extern Imu6DOF_t q;
void qmi8658c_init(void);
void qmi8658c_Read_AccAndGry( Imu6DOF_t *p );
void qmi8658c_fetch_angleFromAcc(Imu6DOF_t *p);


// 公共函数
//void QMI8658_Init(void);

//void QMI8658_ReadRaw(int16_t *ax, int16_t *ay, int16_t *az,
//                     int16_t *gx, int16_t *gy, int16_t *gz);

#endif // EXAMPLE_H  // 头文件保护结束


