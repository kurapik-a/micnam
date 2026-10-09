#ifndef __mymode_h__
#define __mymode_h__

#include "main.h"
#include "stdio.h"
#include "stdarg.h"

// ----------------- DSP 加速数学函数 (硬件CORDIC, 使用前先调 CORDIC_Enable() 开时钟) -----------------
float sinf(float w);                 // sin, 输入弧度
float cosf(float w);                 // cos, 输入弧度
float FastSqrt(float x, float y, float xmax);   // √(x?+y?), xmax 为允许的最大量纲值
float atanf(float x);                // atan(x) [-π/2,π/2], DSP加速(替代数学库atanf)
float FastAtan2(float y, float x);   // atan2(y,x) [-π,π], DSP加速(替代数学库atan2f)

void ALL_GPIO(GPIO_TypeDef *GPIOx,u32 GPIO_Pin_x,GPIO_Mode_TypeDef GPIO_Mode,GPIO_PuPd_TypeDef PD);
void TURN_PIN(GPIO_TypeDef *GPIOx,u32 GPIO_Pin_x,u8 of);
#define RX_SIZE	128
#define TX_SIZE	128
typedef struct
{
	u8 T_c;	//发送完成标志
	u8 R_c;	//接收完成标志
	float floatnumber[31];	//128/4 4个浮点数
	u32 timer_IDLE;			//软件空闲中断的计时判断状态(通过定时器(16位可以满足))
	u32 i_rxbuf;
	char RX_BUFF[RX_SIZE];	//接收数组
	char TX_BUFF[TX_SIZE];	//发送数组
	u8 tim_uart;			//时间计数值 定时器单位ms
	u32 longtime;			//串口超时未接收数据,将重新设置数据接收
} uart_stru;
extern uart_stru UART0_;
void UART0_INIT(u32 baud);
void UART0_SEND(char *t,uint16_t len);
void UART0_PRINT(char *stringg, ...);
//限幅函数
#define limit(value, min_value, max_value) ((value) < (min_value) ? (min_value) : (value) > (max_value) ? (max_value) : (value))
void TIMer_base(UTIMER_TypeDef *UTIMERx,u32 TH,u32 COMP);
//spi0
#define  SPI0_CS_GPIO          GPIO1
#define  SPI0_CS_Pin           GPIO_Pin_14
#define  SPI0_CS_Source        GPIO_PinSource_14
#define  SPI0_MISO_GPIO        GPIO1
#define  SPI0_MISO_Source      GPIO_PinSource_12
#define  SPI0_MOSI_GPIO        GPIO1
#define  SPI0_MOSI_Source      GPIO_PinSource_11
#define  SPI0_SCLK_GPIO        GPIO1
#define  SPI0_SCLK_Source      GPIO_PinSource_13
#define  SPI0_CS_H             GPIO_SetBits(SPI0_CS_GPIO,SPI0_CS_Pin)
#define  SPI0_CS_L             GPIO_ResetBits(SPI0_CS_GPIO,SPI0_CS_Pin)
//spi1
#define  SPI1_MISO_GPIO        GPIO1
#define  SPI1_MOSI_GPIO        GPIO1
#define  SPI1_SCLK_GPIO        GPIO1
#define  SPI1_CS_GPIO          GPIO1   
#define  SPI1_CS_Pin           GPIO_Pin_11
#define  SPI1_MISO_Source      GPIO_PinSource_10
#define  SPI1_MOSI_Source      GPIO_PinSource_9
#define  SPI1_SCLK_Source      GPIO_PinSource_8
#define  SPI1_CS_H             GPIO_SetBits(SPI1_CS_GPIO,SPI1_CS_Pin)
#define  SPI1_CS_L             GPIO_ResetBits(SPI1_CS_GPIO,SPI1_CS_Pin)
void spi0_init(u32 speed_bps);
void spi1_init(u32 speed_bps);
uint8_t MC_MCU_SPIxSendByte(SPI_TypeDef *SPIx,uint8_t byte);
void FOC_PWM(u32 R);
void ABC_OUT_PWM(int A,int B,int C);
void ADC_OPMA_INIT(void);
typedef struct
{
	volatile	float   Out;          /* LPF输出 */	
	float   T_s;                  /* LPF执行周期 */	
	float   F;                    /* LPF滤波频率Freq */	
	float   Last;                 /* 上次LPF结果 */
	float   Actual;               /* 实际值 */
}LPF_Handle_t;
void LowPassFilter (LPF_Handle_t *LPF , float in);
typedef struct
{
	float AF;					//前馈
	float Kp;					//比例
	float Ki;					//积分
	float Kd;					//微分
	float Ref;					//目标值
	float fdbk;					//反馈
	float err;					//误差
	float errlast;				//上一次误差
	float Integrator;			//积分累计
	float Integrator_Limit;		//积分限幅
	float D_limit;				//微分限幅
	float PID_Ts;				//运行周期
	float OUT_Limit;				//输出限幅
	float Result;				//输出
}PID_Handle_t;
float PID_Control (PID_Handle_t *PID_,float fdbk);
// NVR和MAIN的Flash并不是同一个地址空间,他们都有各自的0地址
// NVR只有两个扇区且一个扇区为1024字节
#define  ADDR0	0x0             // 0-0x3FF     为NVR Sector0
#define  ADDR1	0x400           // 0x400-0x7FF 为NVR Sector1
extern u32	reg_moto[256];	//寄存器数组 全部保存在NVR区域中
void NVR_save(void);
void NVR_get(void);
void Vofa_Justfloat_send(float *p,u16 sum);
float TEMP_GET(float in);

extern PID_Handle_t ID_PID;
extern LPF_Handle_t ID_LPF;
extern PID_Handle_t IQ_PID;
extern LPF_Handle_t IQ_LPF;
extern PID_Handle_t SPEED_PID;
extern PID_Handle_t POS_PID;
void CAN_INIT(void);
void IWDG_init(void);

// -------------------------- 卡尔曼滤波 --------------------------
typedef struct {
    float Q;      // 过程噪声协方差 (相信模型的程度)
    float R_;      // 测量噪声协方差 (相信传感器的程度)
    float X;      // 估计值 (输出结果)
    float P;      // 估计误差协方差
    float K;      // 卡尔曼增益
} KalmanFilter;
void Kalman_Init(KalmanFilter *kf, float Q_, float R_, float P_, float X_);
float Kalman_Update(KalmanFilter *kf, float measurement);

// -------------------------- 卡尔曼滤波 --------------------------


#endif


