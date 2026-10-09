#include "lks32mc45x_lib.h"
#include "mymode.h"
#include "main.h"
#include "math.h"
#include "MY_irq.h"
#include "foc.h"

int main(void)
{
	#if(tiaoshi==0)
//		IWDG_init();
	#endif
//	UTIMER_DelayUs(UTIMER0,1000);	//1ms等待启动
	CORDIC_Enable();	//DSP三角函数运算时钟使能
//	ALL_GPIO(io485,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	//RS485收发使能
	UART0_INIT(921600);		//uart波特率
	spi0_init(15000000);	//spi波特率
	FOC_PWM(PWMCCR);		// 10K
	ADC_OPMA_INIT();		// ADC和运放
	timer2irq_init(timer2_CCR);	//1ms中断若没有配置滤波 会导致数据初始化大跳 比如速度
	ALL_GPIO(io24,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	//引出脚
	ALL_GPIO(io27,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	//引出脚
	ALL_GPIO(io28,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	//引出脚
	ALL_GPIO(key30,GPIO_Mode_IN,GPIO_PuPd_UP);		//按键 中
	ALL_GPIO(key215,GPIO_Mode_IN,GPIO_PuPd_UP);		//按键 靠近mos
	ALL_GPIO(led1,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	//LED
	ALL_GPIO(led2,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	//LED
	ALL_GPIO(led3,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	//LED
	TURN_PIN(led1,1);
	TURN_PIN(led2,1);
	TURN_PIN(led3,1);
	NVR_get();	//从NVR中读取数据到寄存器
//	NVR_EraseSector(ADDR0);//擦除数据
	if(reg_moto[REG_NEW]!=ERG_magic)	//未赋值状态 新片
	{
		float xx=1.0f;	//中间变量
		memset(reg_moto, 0, sizeof(reg_moto));//数组清零
		reg_moto[REG_NEW]=ERG_magic;	//新片校验
		reg_moto[REG_ERR_hide]=0;
		PCB.Overcurrent=80.0f;
		reg_moto[REG_Overcurrent] = *(u32*)& PCB.Overcurrent;	//默认80A电流
		reg_moto[REG_DIV]=0;
		reg_moto[REG_PP]=1;
		xx=0.000001f;
		*(float*)&reg_moto[REG_L]=0.001f;	//默认电感0.001H
		*(float*)&reg_moto[REG_R]=20.0f;	//默认电阻20ohm
		xx=1.0f;
		reg_moto[REG_offsetTheta]=*(u32*)&xx;
		xx=0.0f;reg_moto[REG_A_offset0]=*(u32*)&xx;
		xx+=2.093f;reg_moto[REG_A_offset1]=*(u32*)&xx;
		xx+=2.093f;reg_moto[REG_A_offset2]=*(u32*)&xx;
		xx+=2.093f;reg_moto[REG_A_offset3]=*(u32*)&xx;
		reg_moto[REG_debug]=0;	//默认为调试模式
		PCB.Overtemperature=60;
		reg_moto[REG_Overtemperature]=	 *(u32*)& PCB.Overtemperature;//默认过温判断为
		
		xx=0.0f;
		reg_moto[REG_LOOP_DQ_P]=*(u32*)&xx;	//电流环参数默认值
		reg_moto[REG_LOOP_DQ_I]=*(u32*)&xx;	//电流环参数默认值
		
		SPEED_PID.Integrator_Limit=SPEED_PID.OUT_Limit=10;				//默认最大转矩时输出10A线电流
		reg_moto[REG_Output_currentQ]= *(u32*)& SPEED_PID.OUT_Limit;	//默认转矩电流
		SPEED_PID.Kp=0;					//默认速度环参数为0
		reg_moto[REG_LOOP_SPEED_P]=*(u32*)& SPEED_PID.Kp;
		SPEED_PID.Ki=0;
		reg_moto[REG_LOOP_SPEED_I]=*(u32*)& SPEED_PID.Ki;
		
		POS_PID.Integrator_Limit=POS_PID.OUT_Limit=20;			//默认位置环输出限幅（到达位置时的速度）
		reg_moto[REG_Output_speed]= *(u32*)& POS_PID.OUT_Limit;	//默认位置环到达目标时的速度为20red/s
		POS_PID.Kp=0;					//默认位置环参数为0
		reg_moto[REG_LOOP_POS_P]=*(u32*)& POS_PID.Kp;
		POS_PID.Ki=0;
		reg_moto[REG_LOOP_POS_I]=*(u32*)& POS_PID.Ki;
		xx=10000.0f;				//默认是一个超大的值	//这样大部分情况都是触发电压判断的死区补偿
		reg_moto[REG_DEADZONE_CUR_THR]=	*(u32*)&xx;//死区补偿电流阈值
		xx=dead_time_threshold;		//出厂默认电压阈值
		reg_moto[REG_DEADZONE_VOL_THR]= *(u32*)&xx;
		
		xx=4.20f*6.0f;	//默认三元里6s电压
		reg_moto[REG_BATTERY_MAX_VOLTAGE]=*(u32*)&(xx);	//电池过压报警
		xx=3.0f*6.0f;
		reg_moto[REG_BATTERY_MIN_VOLTAGE]=*(u32*)&(xx);	//电池低压报警
		reg_moto[REG_ERR_hide]=err_currentLOOP_Failure+err_SpeedLOOP_Failure;//电流环速度环失效屏蔽

		NVR_save();	//保存寄存器数据到NVR
		UTIMER_DelayUs(UTIMER0,1000);//1ms延时
		NVIC_SystemReset();
	}
	set_const_init();		//在中断前赋值初始化数据  根据NVR内保存的数据给电流环速度环位置环赋值
	Data_preprocessing();	//角度校准数据
	reg_moto[REG_Version]=20260927;	//版本号
	MOTO.state=state_offset+state_getspeed+state_encoder_measurement;		//初始化
	Kalman_Init(&MOTO.AW_LPF,0.1,10,0,0);	//速度卡尔曼滤波
	//CAN 在state_offset相关的内容里开启了
	while(1)
	{
		#if(tiaoshi==0)
			IWDG_FeedDog();
		#endif
		TURN_PIN(led2,!GPIO_ReadOutputDataBit(led2));
		UTIMER_DelayUs(UTIMER3,50000);	//50ms等待启动
//		Seven_SVPWM();
//		该版本没有其他按键,只有复位按键
//		if( !GPIO_ReadInputDataBit(key30) )	//中间按键按下
//		{
//			MOTO.state=state_Calibration_A;
//			TURN_PIN(led2,!GPIO_ReadOutputDataBit(led2));	//led翻转
//			while( !GPIO_ReadInputDataBit(key30) );
//		}
//		if( !GPIO_ReadInputDataBit(key215) )	//靠近mos按键按下
//			stop();
		//需要把接收到的数据在主函数中轮询
	}
}



