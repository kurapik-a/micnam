
#include "lks32mc45x_lib.h"
#include "mymode.h"
#include "foc.h"
// ----------------------------------- CAN总线中断 -----------------------------------
void CAN_IRQHandler(void)
{
    if (CAN_RTIF & BIT3) /*PTB 发送中断标志*/
    {
        CAN_RTIF = BIT3;
    }
    if (CAN_RTIF & BIT2) /*STB 发送中断标志*/
    {
        CAN_RTIF = BIT2;
    }
    /*****************接收设备一直无应答，则取消重发发送*****************/
    if (CAN_ERRINT & BIT0) /*取消发送中断标志*/
    {
        CAN_ERRINT = BIT0;
        if (CAN_TECNT > 127) /*被动错误*/
        {
            if (CAN_EALCAP & 0x80) /* 应答错误*/
            {
                //		   CAN_TCMD |= BIT3; /*取消PTB发送*/
            }
        }
    }
    if (CAN_RTIF & BIT7) /*接收到有效帧标志*/
    {
        CAN_RTIF = BIT7;
        CAN_Receive_Msg(&(can_par.id), &(can_par.len), &(can_par.ide), &(can_par.rtr), can_par.RX);
		/*
		id:		滤波器设置的ID就是串口通信设备ID
		len:	上位机发送的数据量
		ide:	帧格式,该程序使用标准帧
		rtr:	数据/遥控
		若出现遥控帧:返回 速度,位置(REG_Actual_redS)\(REG_Actual_POS)
		若出现数据帧:
			规定数据格式:
		字节0:寄存器地址
		字节1:写入的值(小端float格式)
		字节2:写入的值(小端float格式)
		字节3:写入的值(小端float格式)
		字节4:写入的值(小端float格式)
		字节5:校验和(1+2+3+4+5的和)
		若数据帧长度只有1:
			寄存器数据读取
		字节0:需要读取的寄存器地址 
		返回:6字节
		字节0:寄存器地址
		字节1:u32寄存器的值
		字节2:u32寄存器的值
		字节3:u32寄存器的值
		字节4:u32寄存器的值
		字节5:以上字节的校验和
		*/
		if(can_par.rtr)		//遥控帧
		{
//			memcpy(&can_par.TX[0], &reg_moto[REG_Actual_redS], 4);
//			memcpy(&can_par.TX[4], &reg_moto[REG_Actual_POS], 4);
			
			*(uint32_t *)(can_par.TX + 0) = *(uint32_t *)(reg_moto + REG_Actual_redS);
			*(uint32_t *)(can_par.TX + 4) = *(uint32_t *)(reg_moto + REG_Actual_POS);
			
			My_CAN_Send_Msg(reg_moto[REG_addr], 0, 0,can_par.TX , 8);
		}
		else if(can_par.len==6 && can_par.RX[0]<=255)	//数据帧
		{//该设备can总线不允许广播帧
			static u16 can_reg_addr;
			static float Value;
			static u8 sum;
			sum = can_par.RX[0]+can_par.RX[1]+can_par.RX[2]+can_par.RX[3]+can_par.RX[4];
			if(sum == can_par.RX[5])
			{
				can_reg_addr=can_par.RX[0];			//寄存器地址不会超过8位
				//memcpy(&Value, &can_par.RX[1], 4);	
				Value = *(float *)(can_par.RX + 1);//获取内容(float)
				if(can_reg_addr!=REG_addr)
				{
					
				}
				reg_Parse(can_reg_addr,Value);
			}
			// -------------- 默认回复 -------------- 
			*(uint32_t *)(can_par.TX + 0) = *(uint32_t *)(reg_moto + REG_Actual_redS);
			*(uint32_t *)(can_par.TX + 4) = *(uint32_t *)(reg_moto + REG_Actual_POS);
			My_CAN_Send_Msg(reg_moto[REG_addr], 0, 0,can_par.TX , 8);
		}
		else if(can_par.len==1 && can_par.RX[0]<=255)	//数据帧(寄存器读取)
		{//返回地址可以用于查看地址更改是否成功
			can_par.TX[0]=can_par.RX[0];	//返回寄存器地址	can_par.RX是u8类型,不会出现>255的数据
//			memcpy(&can_par.TX[1], &reg_moto[can_par.RX[0]], 4);	//获取内容(float)
			
			*(float *)(can_par.TX + 1) = *(float *)(reg_moto + can_par.RX[0]);
			
			can_par.TX[5]=can_par.TX[0]+can_par.TX[1]+can_par.TX[2]+can_par.TX[3]+can_par.TX[4];
			My_CAN_Send_Msg(reg_moto[REG_addr], 0, 0,can_par.TX , 6);		//已经确保reg_moto[REG_addr]是跟随ID改变指令改变的
		}
    }
    if (CAN_RTIF & BIT4) {
        CAN_ERRINT = BIT4; /*被动错误中断标志*/
    }
    if (CAN_RTIF & BIT5) {
        CAN_RTIF = BIT5;
    }
    if (CAN_RTIF & BIT6) {
        CAN_RTIF = BIT6;
    }
    if (CAN_RTIF & BIT1) {
        CAN_RTIF = BIT1;
    }
    CAN_RTIF = 0xff;
}
// ----------------------------------- ADC采集中断 -----------------------------------
void ADC0_IRQHandler(void)
{
	if (ADC0_IF & BIT0)                     // 判断是否发生第一采样完成中断
	{
		ADC0_IF = BIT0;
	}
}
void ADC1_IRQHandler(void)
{
	if (ADC1_IF & BIT0)                     // 判断是否发生第一采样完成中断
	{
		ADC1_IF = BIT0;
		

	}
}
void ADC2_IRQHandler(void)
{
	if (ADC2_IF & BIT0)                     // 判断是否发生第一采样完成中断
	{
		ADC2_IF = BIT0;
	}
}

float absx(float x)
{
	if(x>0) return x;
	else return -x;
}

/*
1ms定时器控制:
	操作串口通信
	测量速度
	测量温度
	测量母线电压
*/
// ----------------------------------- 定时器中断 -----------------------------------
char of_sin;
float F_sin;
float A_sin;


static float AW_;
static DMA_InitTypeDef uart0_dma_config__;
void TIMER2_IRQHandler(void)
{
	if(UTIMER2_IF & UTIMER_IE_CH0)	//比较值中断
		UTIMER2_IF = UTIMER_IE_CH0;  // 清除UTimer中断标志位
	if(UTIMER2_IF & UTIMER_IE_ZERO)	//过零中断
	{
		TURN_PIN(led1,1);
		TURN_PIN(led1,0);
		UTIMER2_IF = UTIMER_IE_ZERO;  // 清除UTimer中断标志位
		static int TEMP_ADC;
		static int V_ADC;
		TEMP_ADC=ADC1_DAT3>>3;	//8192为最大 3.3V p3.4
		V_ADC=ADC1_DAT4>>2;		//p3.3
//		PCB.temp=TEMP_GET(TEMP_ADC)*0.01f+0.99f*PCB.temp;	//该函数计算一次对数,可能比较耗时
//		PCB.Voltage	=(float)(V_ADC)/8192*3.33f/10*101 *1.0f + 0.0f*PCB.Voltage ;//*constantADC_Voltage_SCALE_POWER;	//母线电压ADC	24.05-5314
		
		PCB.temp=TEMP_GET(TEMP_ADC)*0.01f+0.99f*PCB.temp;	//该函数计算一次对数,可能比较耗时
		PCB.Voltage	=(float)(V_ADC)*constantADC_Voltage_SCALE_POWER *0.02f + 0.98f*PCB.Voltage ;//*constantADC_Voltage_SCALE_POWER;	//母线电压ADC	24.05-5314
		
		
		if(PCB.Over_ctrl_timer)
			PCB.Over_ctrl_timer--;
		//1.8v	3.45/0.00452578
		if( MOTO.state&state_FOCloopsPOS )		//单位置闭环使能
		{
			SPEED_PID.Ref=PID_Control(&POS_PID,MOTO.Theta_Absoluteangle);
		}
		if(MOTO.state&state_FOCloopspeed)	//单速度闭环使能
		{
			SPEED_PID.errlast=SPEED_PID.err;	//保存上一次误差
			SPEED_PID.err = SPEED_PID.Ref - MOTO.Aw;	//现在误差
			SPEED_PID.Integrator += SPEED_PID.err * SPEED_PID.Ki * SPEED_PID.PID_Ts;	//矩形积分 (还有梯形积分)
			if(absx(MOTO.Aw)<absx(SPEED_PID.Ref*0.9f))  //误差很大的时候没有积分 卡停的时候保持力矩 但是要防止突然松手时的过冲 所以清空积分
				SPEED_PID.Integrator_Limit=absx(MOTO.OUT_I);	//设置积分限幅
			else
				SPEED_PID.Integrator_Limit=Vmax;				//设置积分限幅
			SPEED_PID.Integrator=limit(SPEED_PID.Integrator,-SPEED_PID.Integrator_Limit,SPEED_PID.Integrator_Limit);
			SPEED_PID.Result =SPEED_PID.Ref*SPEED_PID.AF + SPEED_PID.err * SPEED_PID.Kp + SPEED_PID.Integrator+limit((SPEED_PID.err-SPEED_PID.errlast)/SPEED_PID.PID_Ts,-SPEED_PID.D_limit,SPEED_PID.D_limit);
			MOTO.OUT_U=limit(SPEED_PID.Result,-SPEED_PID.OUT_Limit,SPEED_PID.OUT_Limit);
		}
		// ---------------------- 安全保护 ----------------------
		static  u16  cnt_time[6];
		if(!(reg_moto[REG_ERR_hide] & err_Overcurrent))
		{
			if((fabs(MOTO.IQ)>PCB.Overcurrent) || (fabs(MOTO.ID)>PCB.Overcurrent))	//过大电流限制
			{
				cnt_time[0]++;
				if(cnt_time[0]>(0.01f/TS_timer2))	//10ms错误滤波
				{
					stop();
					reg_moto[REG_ERR]|=err_Overcurrent;
					cnt_time[0]=0;
				}
			}else
				cnt_time[0]=0;
		}
		if(!(reg_moto[REG_ERR_hide] & err_Overtemperature))
		{
			if((PCB.temp<0) || (PCB.temp>PCB.Overtemperature))				//温度限制
			{
				cnt_time[1]++;
				if(cnt_time[1]>0.05f/TS_timer2)
				{
					stop();
					reg_moto[REG_ERR]|=err_Overtemperature;
					cnt_time[1]=0;
				}
			}else
				cnt_time[1]=0;
		}
		if(!(reg_moto[REG_ERR_hide] & err_voltage))
		{
			if((PCB.Voltage< (*(float*)&(reg_moto[REG_BATTERY_MIN_VOLTAGE])) ) || (PCB.Voltage> (*(float*)&(reg_moto[REG_BATTERY_MAX_VOLTAGE])) )  )	//电压限制 目前电路无法读取超过33V电压
			{
				cnt_time[2]++;
				if(cnt_time[2]>(0.010f/TS_timer2))	//判断 时间为10ms
				{
					stop();	//触发保护
					reg_moto[REG_ERR]|=err_voltage;	//报错电压异常
					cnt_time[2]=0;
				}
			} else
				cnt_time[2]=0;
		}
		if(!(reg_moto[REG_ERR_hide] & err_Over_ctrl_time))	//通信时间限制
		{
			if(!PCB.Over_ctrl_timer)
			{
				stop();
				reg_moto[REG_ERR]|=err_Over_ctrl_time;
			}
		}
		if((MOTO.state&state_FOCloopIQ)&&(!(reg_moto[REG_ERR_hide] & err_currentLOOP_Failure)))	//电流环闭环失效 (电机坏)
		{//电流环Q使能
			if(  fabs(MOTO.IQ)<fabs(IQ_PID.Ref)*0.8f  )		//电流环90%,持续10ms认为失效
			{
				cnt_time[3]++;
				if(cnt_time[3]>(0.1f/TS_timer2))	//100ms判断时间
				{
					stop();	//触发保护
					reg_moto[REG_ERR]|=err_currentLOOP_Failure;	//报错电压异常
					cnt_time[3]=0;
				}
			}
			else
				cnt_time[3]=0;
		}
		if((MOTO.state&state_FOCloopspeed)&&(!(reg_moto[REG_ERR_hide] & err_SpeedLOOP_Failure)))	//速度环 闭环失效 (堵转)
		{
			if( fabs(MOTO.Aw)<fabs(SPEED_PID.Ref)*0.8f  && (fabs(MOTO.IQ)>fabs(IQ_PID.Ref)*0.6f)  )			//电流较大,开始做功
			{
				cnt_time[4]++;
				if(cnt_time[4]>(1.0f/TS_timer2))	//1s判断时间
				{
					stop();	//触发保护
					reg_moto[REG_ERR]|=err_SpeedLOOP_Failure;	//报错电压异常
					cnt_time[4]=0;
				}
			}
			else
				cnt_time[4]=0;
		}
		// ---------------------- 安全保护 ----------------------
		
		// ---------------------- 扫频调试 ----------------------
		if(of_sin)
		{
			static float T_sin;
			T_sin+=F_sin*_2PI*TS_timer2;
			if(T_sin>6.28*100)
				T_sin=0;
			if(T_sin<-6.28*100)
				T_sin=0;
			SPEED_PID.Ref=A_sin*sinf(T_sin);
		}
		// ---------------------- 扫频调试 ----------------------
		
		
		static int time_send_vofa;
		time_send_vofa++;
		if(time_send_vofa>=  (u16)(0.001f/TS_timer2))
		{
			time_send_vofa=0;
			static u32 last_REG_debug;	//保存上一次的debug数据值，若数据变化将 TX_BUFF 数组清空
			if(last_REG_debug!=reg_moto[REG_debug])	//若发送模式发生变化
				memset(UART0_.TX_BUFF, 0, 128);  	// 全部字节设为 0
			last_REG_debug=reg_moto[REG_debug];
			if(reg_moto[REG_debug]==0)	//调试数据发送
			{
				UART0_.floatnumber[0]	=	MOTO.ID		;	//ID
				UART0_.floatnumber[1]	=	MOTO.IQ		;	//IQ
				UART0_.floatnumber[2]	=	MOTO.IA		;	//IA
				UART0_.floatnumber[3]	=	MOTO.IB		;	//IB
				UART0_.floatnumber[4]	=	MOTO.IC		;	//IC
				UART0_.floatnumber[5]	=	PCB.Voltage	;	//母线电压
				UART0_.floatnumber[6]	=	PCB.temp	;	//板子温度
				UART0_.floatnumber[7]	=	MOTO.GET_UA	;	//A电压采样
				UART0_.floatnumber[8]	=	MOTO.GET_UB	;	//B电压采样
				UART0_.floatnumber[9]	=	MOTO.GET_UC	;	//C电压采样
				UART0_.floatnumber[10]	=	MOTO.Mechanical_Angle;		//机械角度
				UART0_.floatnumber[11]	=	reg_moto[REG_ERR]	;		//错误码
				UART0_.floatnumber[12]	=	MOTO.Theta_Absoluteangle;	//多圈角度
				UART0_.floatnumber[13]	=	MOTO.Aw		;	//角速度
				
				UART0_.floatnumber[14]	=	MOTO.Theta_adc	;	//输出D轴电压
				
				
				//UART0_.floatnumber[14]	=	MOTO.UD		;	//输出D轴电压
				UART0_.floatnumber[15]	=	MOTO.UQ		;	//输出Q轴电压
				UART0_.floatnumber[16]	=	IQ_PID.Kp		;	//电机的相电感
				UART0_.floatnumber[17]	=	IQ_PID.Ki		;	//电机的相电阻
				
				Vofa_Justfloat_send(UART0_.floatnumber,18);
			}
			if(reg_moto[REG_debug]==1)	//寄存器数据显示
			{
				UART0_.floatnumber[0]	=				reg_moto[REG_addr				];	// 地址 设备ID
				UART0_.floatnumber[1]	=				reg_moto[REG_Version			];	// 版本
				UART0_.floatnumber[2]	=	*(float*)&	reg_moto[REG_Output_VoltageD	];	// 输出电压(D轴)
				UART0_.floatnumber[3]	=	*(float*)&	reg_moto[REG_Output_VoltageQ	];	// 输出电压(Q轴)
				UART0_.floatnumber[4]	=	*(float*)&	reg_moto[REG_Output_currentD	];	// 目标电流	ID
				UART0_.floatnumber[5]	=	*(float*)&	reg_moto[REG_Output_currentQ	];	// 目标电流	IQ
				UART0_.floatnumber[6]	=	*(float*)&	reg_moto[REG_Output_speed		];	// 目标速度
				UART0_.floatnumber[7]	=	*(float*)&	reg_moto[REG_Output_POS			];	// 目标位置
				UART0_.floatnumber[8]	=				reg_moto[REG_DIV				];	// 线序
				UART0_.floatnumber[9]	=				reg_moto[REG_PP					];	// 磁极对数
				UART0_.floatnumber[10]	=	*(float*)&	reg_moto[REG_offsetTheta		];	// 电角度校准值
				UART0_.floatnumber[11]	=	*(float*)&	reg_moto[REG_L					];	// 电机电感参数
				UART0_.floatnumber[12]	=	*(float*)&	reg_moto[REG_R					];	// 电机电阻参数
				UART0_.floatnumber[13]	=	*(float*)&	reg_moto[REG_Overcurrent		];	// 过流保护
				UART0_.floatnumber[14]	=				reg_moto[REG_ERR				];	// 故障码
				UART0_.floatnumber[15]	=				reg_moto[REG_Overtime			];	// 通信超时时间
				UART0_.floatnumber[16]	=				reg_moto[REG_ERR_hide			];	// 故障屏蔽码
				UART0_.floatnumber[17]	=				reg_moto[REG_MOTO_MODE			];	// 电机运行模式
				UART0_.floatnumber[18]	=	*(float*)&	reg_moto[REG_LOOP_DQ_P      	];	// 电流环P参数
				UART0_.floatnumber[19]	=	*(float*)&	reg_moto[REG_LOOP_DQ_I      	];	// 电流环I参数
				UART0_.floatnumber[20]	=	*(float*)&	reg_moto[REG_LOOP_SPEED_P		];	// 速度环P参数
				UART0_.floatnumber[21]	=	*(float*)&	reg_moto[REG_LOOP_SPEED_I		];	// 速度环I参数
				UART0_.floatnumber[22]	=	*(float*)&	reg_moto[REG_LOOP_POS_P			];	// 位置环P参数
				UART0_.floatnumber[23]	=	*(float*)&	reg_moto[REG_LOOP_POS_I			];	// 位置环I参数
				UART0_.floatnumber[24]	=	*(float*)&	reg_moto[REG_Overtemperature	];	// 过温保护值
				UART0_.floatnumber[25]	=	*(float*)&	reg_moto[REG_DEADZONE_CUR_THR   ];	// 电流死区补偿判断值
				UART0_.floatnumber[26]	=	*(float*)&	reg_moto[REG_DEADZONE_VOL_THR   ];	// 电压死区补偿判断值
				UART0_.floatnumber[27]	=	*(float*)&	reg_moto[REG_BATTERY_MAX_VOLTAGE];	// 最大电池电压
				UART0_.floatnumber[28]	=	*(float*)&	reg_moto[REG_BATTERY_MIN_VOLTAGE];	// 最小电池电压
//				UART0_.floatnumber[29]	=				reg_moto[			];	// 电机运行模式
				Vofa_Justfloat_send(UART0_.floatnumber,30);
			}
		}
		if(UART0_.RX_BUFF[1]&& !UART0_.tim_uart)	//DMA接收数据,若下标1有数据,说明接收到了一帧数据
		{
			UART0_.tim_uart=3;	//等待5ms数据发送完毕
			UART0_.longtime=0;
		}
		else
			UART0_.longtime++;	//长时间没有收到数据
		if(UART0_.tim_uart>0)//计时到,开始处理数据
		{
			UART0_.tim_uart--;
			if(UART0_.tim_uart==0)	//等待完成
			{//	StrinW22;7;20
				UART0_.RX_BUFF[29] = 0; // 强制末尾置零//检查数据量过大,因为发送的数据不会超过30个字符,若超过则忽略 解决了atof()函数越界问题
				if(
					UART0_.RX_BUFF[1]=='S'
				&&	UART0_.RX_BUFF[2]=='t'
				&&	UART0_.RX_BUFF[3]=='r'
				&&	UART0_.RX_BUFF[4]=='i'
				&&	UART0_.RX_BUFF[5]=='n'
				)//接收字符串命令包头
				{
					static u16 reg_addr,addr;
					static u16 i,i1;
					reg_addr=0;i=0;addr=0;i1=0;
					if('0'<=UART0_.RX_BUFF[7] && UART0_.RX_BUFF[7]<='9')	//判断数据正确性
						addr=atoi(&UART0_.RX_BUFF[7]);		//获取设备地址
					else
						reg_addr=256;	//不允许做修改和控制
					while(UART0_.RX_BUFF[7+i]!=';')	//遇到 ';' 为结束符 10进制
					{
						i++;
						if(i>=5)break;
					}
					if('0'<=UART0_.RX_BUFF[8+i] && UART0_.RX_BUFF[8+i]<='9')	//判断数据正确性
						reg_addr=atoi(&UART0_.RX_BUFF[8+i]);	//获取寄存器地址
					else
						reg_addr=256;	//不允许做修改和控制
					if((addr==(u8)reg_moto[REG_addr]||addr==0 )&& (reg_addr<256))	//地址正确 0为广播地址 拦截过大寄存器地址数据
					{
						while(UART0_.RX_BUFF[8+i]!=';')	//遇到 ';' 为结束符 10进制
						{
							i++;
							i1++;
							if(i1>=5)break;
						}
						if(	UART0_.RX_BUFF[6]=='W')			//写寄存器
						{//若写寄存器可能出现 1.9999f=1的情况,请把输入的数据+0.5f,这样可以自动四舍五入
							static float Value;
							Value=atof(&UART0_.RX_BUFF[9+i]);
							reg_Parse(reg_addr,Value);		//寄存器操作
							if(addr!=0)
								UART0_PRINT("%ld copy that\n",(u8)reg_moto[REG_addr]);
						}
						else if(	UART0_.RX_BUFF[6]=='R')	//读寄存器
						{
							if(addr!=0)
								UART0_PRINT("%dREG%ld:%d\n",(u8)reg_moto[REG_addr],reg_addr,reg_moto[reg_addr]);
						}
					}
				}
				for(int i=0;i<30;i++)
					UART0_.RX_BUFF[i]=0;//去除包头,等待下一帧数据
				//准备下一次DMA接收	对于该单片机,必须这么做 否则无法重置DMA
				
//				DMA_StructInit(&uart0_dma_config__);
				uart0_dma_config__.DMA_Channel_EN = ENABLE;  			/* DMA 通道使能*/
				uart0_dma_config__.DMA_IRQ_EN     = DISABLE; 			/* DMA 中断使能 */
				uart0_dma_config__.DMA_RMODE      = ENABLE;  			/* 多轮传输使能 */
				uart0_dma_config__.DMA_CIRC       = DISABLE; 			/* 循环模式使能 */
				uart0_dma_config__.DMA_SINC       = DISABLE;  		/* 源地址递增使能 */
				uart0_dma_config__.DMA_DINC       = ENABLE; 			/* 目的地址递增使能 */
				uart0_dma_config__.DMA_SBTW       = 0;				/* 源地址访问位宽， 0:byte, 1:half-word, 2:word */
				uart0_dma_config__.DMA_DBTW       = 0;				/* 目的地址访问位宽， 0:byte, 1:half-word, 2:word */
				uart0_dma_config__.DMA_REQ_EN     = DMA_REQ_UART0_RX;	/* 通道 x 硬件 DMA 请求使能，高有效 */
				uart0_dma_config__.DMA_TIMES      = RX_SIZE;				/* DMA 通道 x 数据搬运次数 */	//发送时会重新更改发送次数
				uart0_dma_config__.DMA_SADR       = (u32)&UART0_BUFF;	/* DMA 通道 x 源地址 */
				uart0_dma_config__.DMA_DADR       = (u32)&UART0_.RX_BUFF;	/* DMA 通道 x 目的地址 */
				DMA_Init(DMA_CH1, &uart0_dma_config__);
				DMA_Trigger(DMA_CH1);	//触发一次
			}
		}
		if(UART0_.longtime>4000)	//长时间 1s 没有收到数据 重置串口接收
		{
			//准备下一次DMA接收	对于该单片机,必须这么做 否则无法重置DMA
//			static DMA_InitTypeDef uart0_dma_config__;
//			DMA_StructInit(&uart0_dma_config__);
			uart0_dma_config__.DMA_Channel_EN = ENABLE;  			/* DMA 通道使能*/
			uart0_dma_config__.DMA_IRQ_EN     = DISABLE; 			/* DMA 中断使能 */
			uart0_dma_config__.DMA_RMODE      = ENABLE;  			/* 多轮传输使能 */
			uart0_dma_config__.DMA_CIRC       = DISABLE; 			/* 循环模式使能 */
			uart0_dma_config__.DMA_SINC       = DISABLE;  		/* 源地址递增使能 */
			uart0_dma_config__.DMA_DINC       = ENABLE; 			/* 目的地址递增使能 */
			uart0_dma_config__.DMA_SBTW       = 0;				/* 源地址访问位宽， 0:byte, 1:half-word, 2:word */
			uart0_dma_config__.DMA_DBTW       = 0;				/* 目的地址访问位宽， 0:byte, 1:half-word, 2:word */
			uart0_dma_config__.DMA_REQ_EN     = DMA_REQ_UART0_RX;	/* 通道 x 硬件 DMA 请求使能，高有效 */
			uart0_dma_config__.DMA_TIMES      = RX_SIZE;				/* DMA 通道 x 数据搬运次数 */	//发送时会重新更改发送次数
			uart0_dma_config__.DMA_SADR       = (u32)&UART0_BUFF;	/* DMA 通道 x 源地址 */
			uart0_dma_config__.DMA_DADR       = (u32)&UART0_.RX_BUFF;	/* DMA 通道 x 目的地址 */
			DMA_Init(DMA_CH1, &uart0_dma_config__);
			DMA_Trigger(DMA_CH1);	//触发一次
		}
	}
}

void UART0_IRQHandler(void)
{
	if (UART_GetIRQFlag(UART0, UART_IF_SendOver))// /发送完成中断
	{
		UART0_.T_c=1;	//标志位置位,下一次发送做准备
//		TURN_PIN(io37,1);
//		
//		TURN_PIN(io485,0);
//		TURN_PIN(io37,0);
		UART_ClearIRQFlag(UART0, UART_IF_SendOver);
	}
	if (UART_GetIRQFlag(UART0, UART_IF_RcvOver)) //接收完成中断/
	{
		UART_ClearIRQFlag(UART0, UART_IF_RcvOver);	
		//处理逻辑:------------
		//通过定时器做一个空闲中断
//		UART0_.RX_BUFF[UART0_.sum]=UART_ReadData(UART0);
//		UART0_.i_rxbuf++;
//		UART0_.tim_uart=5;//5ms定时时间 若5ms没有收到信息,将会在定时器中断中触发空闲
		//处理逻辑:------------
	}
	if(UART_GetIRQFlag(UART0,BIT8))
	{
		UART0->IF=0XFFFFFFFF;	//清除所有uart中断标志
	}
	if (UART_GetIRQFlag(UART0, UART_IF_SendBufEmpty)) //发送缓冲区空中断/
	{	UART_ClearIRQFlag(UART0, UART_IF_SendOver);	}
	if (UART_GetIRQFlag(UART0, UART_IF_StopError)) //停止位错误
	{	UART_ClearIRQFlag(UART0, UART_IF_StopError);	}
	if (UART_GetIRQFlag(UART0, UART_IF_CheckError)) //校验错误
	{	UART_ClearIRQFlag(UART0, UART_IF_CheckError);	}
}

void MCPWM00_IRQHandler(void)	//MCPWM的下桥开启中断
{
	MCPWM_ClearCnt0IRQFlag(MCPWM0, MCPWM_IF_T1);
	SPI_SendData(SPI0, 0x00);//只用于触发SPI传输的PWM中断
}

void SPI0_IRQHandler(void)//在该中断中做FOC控制  该ADC配置为在触发ADC采集后0.5us采集完成 在触发ADC采集后2us进入该中断
{
	SPI_ClearIRQFlag(SPI0,SPI_IF_TranDone);
	if(MOTO.state&state_encoder_measurement)	//角度测量模式
	{
		static float A_enco;
		A_enco=enc2mech_fpu(Encoder());	//校准完成,在高速下有比较好的效果,处理时间不算长 慢速下处理时间可控,高速下处理时间不可控 在 400red/s的时候可能不可控,但是任然运行稳定
		Angle_get
		(
			A_enco						,
			&MOTO.Theta_adc				,	//电角度输出_对齐ADC采样时刻的电角度
			&MOTO.Theta_pwm				,	//对齐PWM输出时刻的电角度
			&MOTO.Last_Mechanical_Angle	,	//上一次机械角度
			&MOTO.Mechanical_Angle		,	//机械角度
			&MOTO.Theta_Absoluteangle	,	//多圈机械角度
			&MOTO.Laps					,	//圈数
			&MOTO.Aw					,	//机械角速度
			&MOTO.PP						//磁极对数(输入)
		);
		
//		//第二种多圈角度
//		Last_Mechanical_Angle_1 = Mechanical_Angle_1;	// 记录上一次角度
//		Mechanical_Angle_1=off_set_A- MOTO.A_enco_bad;	//原点校准
//		static char first_A=2;
//		if(first_A)
//		{
//			Last_Mechanical_Angle_1 = Mechanical_Angle_1;		//防止圈数初始化被破坏
//			if(Mechanical_Angle_1<0)	//开机检查一下位置设定圈数
//				Laps_1=1;				//多圈开机时所在的位置为1圈
//			else 
//				Laps_1=0;				//多圈开机时所在的位置为0圈
//			first_A--;
//		}
//		else
//		{
//			mind_angle1=Mechanical_Angle_1 - Last_Mechanical_Angle_1;	
//			if(mind_angle1<0)	//取绝对值
//				mind_angle1=-mind_angle1;
//			if (mind_angle1 > PI) // 测出圈数
//			{
//				if (Mechanical_Angle_1 > Last_Mechanical_Angle_1)
//					Laps_1 = Laps_1 - 1;
//				else
//					Laps_1 = Laps_1 + 1;
//			}
//			Theta_Absoluteangle_1 = (float)Laps_1 * _2PI + Mechanical_Angle_1; // 测出多圈绝对角度
//		}
	}
	if(MOTO.state&state_F)			//电压开环输出
	{
		static float thhe;
		thhe+=MOTO.A_openloop*TS_PWM*MOTO.PP;	//设置速度
		if(thhe>_2PI)
			thhe-=_2PI;
		if(thhe<-_2PI)
			thhe+=_2PI;
		MOTO.Theta_pwm=thhe;
		MOTO.Theta_adc=thhe;
	}
	if(MOTO.state&state_Current_measurement)	//电流测量计算 需要6us
	{
		static float mid_Iabc;
		//实际线序要根据电路来确定
		if(MOTO.DIV)
		{	MOTO.IB=(float)((ADC2_DAT0 >> 2)-MOTO.Current_offset[2])*constantADC_CURRENT_SCALE_C;
			MOTO.IC=(float)((ADC1_DAT0 >> 2)-MOTO.Current_offset[1])*constantADC_CURRENT_SCALE_B;
			MOTO.IA=(float)((ADC0_DAT0 >> 2)-MOTO.Current_offset[0])*constantADC_CURRENT_SCALE_A;}
		else
		{	MOTO.IC=(float)((ADC2_DAT0 >> 2)-MOTO.Current_offset[2])*constantADC_CURRENT_SCALE_C;		//U UVW对印电路板 ABC是程序内的 因为会自动判断线序 无对印关系
			MOTO.IB=(float)((ADC1_DAT0 >> 2)-MOTO.Current_offset[1])*constantADC_CURRENT_SCALE_B;		//V
			MOTO.IA=(float)((ADC0_DAT0 >> 2)-MOTO.Current_offset[0])*constantADC_CURRENT_SCALE_A;}		//W
		mid_Iabc=(MOTO.IC+MOTO.IB+MOTO.IA)/3;//零序电压
		MOTO.IA=MOTO.IA-mid_Iabc;
		MOTO.IB=MOTO.IB-mid_Iabc;
		MOTO.IC=MOTO.IC-mid_Iabc;
		Clark_Park
		(
			&MOTO.Theta_adc	,//对准ADC采集时刻的电角度
			&MOTO.IA	,//A相电流
			&MOTO.IB	,//B相电流
			&MOTO.IC	,//C相电流
			&MOTO.Ia	,//a轴电流
			&MOTO.Ib	,//b轴电流
			&MOTO.ID	,//D轴电流
			&MOTO.IQ	 //Q轴电流
		);
		
//		LowPassFilter(&IQ_LPF,MOTO.IQ);//这个常数越大,电流环在高速的时候越不容易发散. 这个常数可以看做为电机的反电动势补偿 通过电阻来做一个自适应
//		MOTO.IQ=IQ_LPF.Out;
//		LowPassFilter(&ID_LPF,MOTO.ID);
//		MOTO.ID=ID_LPF.Out;
		
		*(float *)(reg_moto + REG_Actual_currentQ) = MOTO.IQ;
		*(float *)(reg_moto + REG_Actual_currentD) = MOTO.ID;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//	// MOTO.state 从  0X4384   -->   0X24104
//	if(MOTO.state&state_sensorless)
//	{
//		//滑膜观测器+锁相环预测电角度和机械角速度
//		//Sensorless_SMO(&SMO_Param, MOTO.R, MOTO.L, MOTO.PP, &MOTO.Ia, &MOTO.Ib, &MOTO.Ua, &MOTO.Ub, &MOTO.Theta_adc, &MOTO.Aw);
//		SMO_Sensorless(&sm_GET,MOTO.Ia, MOTO.Ib, MOTO.Ua,MOTO.Ub,&MOTO.Theta_adc, &MOTO.Aw);
//		MOTO.Theta_pwm=MOTO.Theta_adc;	//这里使用相同的角度
//	}
//	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	if(!MOTO.Safety)
	{
		if(MOTO.state&state_FOCloopIQ)	//电流闭环
		{
			MOTO.UQ=PID_Control(&IQ_PID,MOTO.IQ);	//输出给 UQ的中间变量
		}
		if(MOTO.state&state_FOCloopID)	//电流闭环
		{
			MOTO.UD=PID_Control(&ID_PID,MOTO.ID);	//输出给 UQ的中间变量
		}
		//计算到电流环需要13us时间
		if(MOTO.state&state_FOCSVPWM)	//svpwm输出使能
		{
			// ------------- 数据融合 -------------
			if(MOTO.state&state_FOC_Current_protec)	//速度环和位置环没有激活的时候电流环激活了,说明是一个电压控制+电流限幅
			{
				IQ_PID.Integrator_Limit=IQ_PID.OUT_Limit=absx(MOTO.OUT_U);//限幅值就是输入电压
				if(MOTO.OUT_U>0.0001f)	//限制死区
				{
					IQ_PID.Ref=MOTO.REF_I;		//设置目标电流为正,正转//取小输出
					MOTO.OUT_I=PID_Control(&IQ_PID,MOTO.IQ);	//输出给 UQ的中间变量
					if((MOTO.OUT_I)<MOTO.OUT_U)		MOTO.UQ=MOTO.OUT_I;
					else MOTO.UQ=MOTO.OUT_U;
				}
				else if(MOTO.OUT_U<-0.0001f)	//限制死区
				{
					IQ_PID.Ref=-MOTO.REF_I;		//设置目标电流为负,反转//取小输出
					MOTO.OUT_I=PID_Control(&IQ_PID,MOTO.IQ);	//输出给 UQ的中间变量
					if((MOTO.OUT_I)>MOTO.OUT_U)	MOTO.UQ=MOTO.OUT_I;
					else MOTO.UQ=MOTO.OUT_U;
				}
				else
				{
					IQ_PID.Ref=0;
					MOTO.OUT_I=PID_Control(&IQ_PID,MOTO.IQ);	//输出给 UQ的中间变量
					MOTO.UQ=MOTO.OUT_I;
				}
			}
			// ------------- 数据融合 -------------
			// ------------- 等比例限幅 -------------
			static float sqrt_mid;//中间变量
			sqrt_mid=FastSqrt(MOTO.UQ,MOTO.UD,100.0f);	// 100V 作为最大值转为定点  sqrt(UQ_MID^2+UD_MID^2)
			if(sqrt_mid>Vmax)	//当电压矢量模长大于 Vmax 的时候  sqrt_mid是电压矢量模长
			{
				MOTO.UD*=Vmax/sqrt_mid;
				MOTO.UQ*=Vmax/sqrt_mid;
			}
			// ------------- 等比例限幅 -------------
			//计算到这里需要17us时间
			Seven_SVPWM(&MOTO.DIV,&MOTO.Theta_pwm,&MOTO.UD,&MOTO.UQ,&MOTO.Ua,&MOTO.Ub,&MOTO.UA,&MOTO.UB,&MOTO.UC);	//SVPWM计算需要10us
			//计算到这里需要27us
		}
		static char stop_fist_time;		//一次执行变量
		if(MOTO.state&state_Disconnect)	//断开电机
		{
			if(!stop_fist_time)
			{
				stop();
				stop_fist_time=1;
			}
		}
		else	//重新使能电机
		{
			if(stop_fist_time)
			{
				Power_on_moto();
				stop_fist_time=0;
			}
		}
		if(MOTO.state&state_circuit_Brake)	//短路刹车
		{
			ABC_OUT_PWM(0,0,0);
		}
	}
	if(MOTO.state==0)				//空闲关闭PWM
	{
		MOTO.Theta_pwm=MOTO.Theta_adc=0.0f;
		MOTO.UD=0.0f;
		MOTO.UQ=0.0f;
		Seven_SVPWM(&MOTO.DIV,&MOTO.Theta_pwm,&MOTO.UD,&MOTO.UQ,&MOTO.Ua,&MOTO.Ub,&MOTO.UA,&MOTO.UB,&MOTO.UC);
	}
	if(MOTO.state&state_getspeed)		//速度测量使能  放在ADC中断里，可以测得比较准确的速度
	{
		static u16 time_wget;	//速度获取周期
		time_wget++;
		//if(time_wget>= (u16)(timer2_V))
		if(time_wget>= 20)	//1ms
		{
			time_wget=0;
			MC_Speed_GetActualSpeed
			(
				&MOTO.Theta_Absoluteangle		,	//多圈角度(输入)
				&MOTO.Theta_lastAbsoluteangle	,	//上一次多圈角度(函数内得出)
				&MOTO.Mechanical_Angle			,	//机械角度(输出)用于更改临界点的速度值
				&MOTO.Laps						,	//圈数(输出)用于更改临界点的多圈角度值
				&AW_							//速度 测量出来的速度
			);
			MOTO.Aw=AW_;
			Kalman_Update(&MOTO.AW_LPF,AW_);	//专门给上报滤波做的
			*(uint32_t *)(reg_moto + REG_Actual_redS) = *(uint32_t *)&MOTO.AW_LPF.X;
			*(uint32_t *)(reg_moto + REG_Actual_POS) = *(uint32_t *)&MOTO.Theta_Absoluteangle;
		}
	}
	if(MOTO.state&get_uABC)			//获取三相电压使能 不随电源电压变化而变化(做了补偿)
	{
		static float mid_Vabc;
		if(MOTO.DIV)
		{	MOTO.GET_UB=(float)((ADC1_DAT2>>2)-MOTO.Voltage_offset[2])*constantADC_Voltage_SCALE_C +(Vdc-PCB.Voltage)/2.0f ;	
			MOTO.GET_UC=(float)((ADC1_DAT1>>2)-MOTO.Voltage_offset[1])*constantADC_Voltage_SCALE_B +(Vdc-PCB.Voltage)/2.0f ;	
			MOTO.GET_UA=(float)((ADC0_DAT1>>2)-MOTO.Voltage_offset[0])*constantADC_Voltage_SCALE_A +(Vdc-PCB.Voltage)/2.0f ;	
		}else
		{
			MOTO.GET_UC=(float)((ADC1_DAT2>>2)-MOTO.Voltage_offset[2])*constantADC_Voltage_SCALE_C +(Vdc-PCB.Voltage)/2.0f ;	//U UVW对印电路板 ABC是程序内的 因为会自动判断线序 无对印关系
			MOTO.GET_UB=(float)((ADC1_DAT1>>2)-MOTO.Voltage_offset[1])*constantADC_Voltage_SCALE_B +(Vdc-PCB.Voltage)/2.0f ;	//V
			MOTO.GET_UA=(float)((ADC0_DAT1>>2)-MOTO.Voltage_offset[0])*constantADC_Voltage_SCALE_A +(Vdc-PCB.Voltage)/2.0f ;	//W
		}
		mid_Vabc=(MOTO.GET_UC+MOTO.GET_UB+MOTO.GET_UA)/3;//零序电压
		MOTO.GET_UC=MOTO.GET_UC-mid_Vabc;
		MOTO.GET_UB=MOTO.GET_UB-mid_Vabc;
		MOTO.GET_UA=MOTO.GET_UA-mid_Vabc;
		
		MOTO.GET_Ua = MOTO.GET_UA;
		MOTO.GET_Ub = (MOTO.GET_UB - MOTO.GET_UC) * Clark_b;
		static float cos_t_;
		static float sin_t_;
		cos_t_ = cosf(MOTO.Theta_adc);
		sin_t_ = sinf(MOTO.Theta_adc);
		MOTO.GET_Ud =  MOTO.GET_Ua * cos_t_ + MOTO.GET_Ub * sin_t_;
		MOTO.GET_Uq =  MOTO.GET_Ub * cos_t_ - MOTO.GET_Ua * sin_t_;
		
		
//			ABC_OUT_PWM(ABC_1[0]*PWMCCR/24,ABC_1[1]*PWMCCR/24,ABC_1[2]*PWMCCR/24);	//PWM半输出
	}
	if(MOTO.state&state_Calibration_RL)	//电机参数识别模式  会从电角度校准中调过来
	{
		static char tc=1;
		if(tc)	//初始化参数识别结构体
		{
			set_Calibration_LR_get();
			tc=0;
		}
		Moto_Calibration_RL(&moto_Calibration_LR);	//参数识别函数
		if(moto_Calibration_LR.over)	//参数识别完成
		{
			reg_moto[REG_L]=*(u32*)&moto_Calibration_LR.L;
			reg_moto[REG_R]=*(u32*)&moto_Calibration_LR.R;
			NVR_save();	//保存寄存器数据到NVR
			NVIC_SystemReset();
		}
	}
	if(MOTO.state&state_Calibration_A)	//校准模式
	{static char tc=1;
		if(tc)	//初始化
		{
			set_Calibration_Calibration();
			tc=0;
		}
		Moto_Calibration_Program(&moto_Calibration);	//循环进入校准函数
		if(moto_Calibration.over)	//校准完成标志
		{
			reg_moto[REG_DIV]=moto_Calibration.DIV;
			reg_moto[REG_PP]=moto_Calibration.PP;
			reg_moto[REG_offsetTheta]=*(u32*)&moto_Calibration.Zero_position_offset;	// 获取指向A轴时的机械角度
			for(int i=0;i<72;i++)
				reg_moto[REG_A_offset0+i]=*(u32*)&moto_Calibration.A_offset[i];
			NVR_save();	//保存寄存器数据到NVR
			MOTO.state=state_Calibration_RL;
		}
	}
	if(MOTO.state&state_offset)				//电流,电压偏置测量
	{
		static u32 i=0;
		static int ADC_Buffer[6];
		ABC_OUT_PWM(PWMCCR/2,PWMCCR/2,PWMCCR/2);	//PWM半输出
//		ABC_OUT_PWM(0,0,0);	//PWM半输出
		ADC_Buffer[0] += (ADC0_DAT0 >> 2);	//W
		ADC_Buffer[1] += (ADC1_DAT0 >> 2);	//V
		ADC_Buffer[2] += (ADC2_DAT0 >> 2);	//U
		ADC_Buffer[3] += (ADC0_DAT1 >> 2);	//W
		ADC_Buffer[4] += (ADC1_DAT1 >> 2);	//V
		ADC_Buffer[5] += (ADC1_DAT2 >> 2);	//U
		i++;
		if(i>=5000)	//	1000次AD采集取平均值就是ADC偏置
		{
			MOTO.Current_offset[0]=ADC_Buffer[0]/5000;	//计算偏置
			MOTO.Current_offset[1]=ADC_Buffer[1]/5000;
			MOTO.Current_offset[2]=ADC_Buffer[2]/5000;
			MOTO.Voltage_offset[0]=ADC_Buffer[3]/5000;
			MOTO.Voltage_offset[1]=ADC_Buffer[4]/5000;
			MOTO.Voltage_offset[2]=ADC_Buffer[5]/5000;
			//MOTO.state=state_Current_measurement+state_encoder_measurement+state_FOCSVPWM+state_getspeed;
			//开机校准完后的状态
//			MOTO.state=0;
			MOTO.state=	state_encoder_measurement	//电角度测量
						+state_FOCSVPWM				//SVPWM输出使能
						+state_getspeed				//速度测量使能
						+state_Current_measurement	//DQ和三相电流测量使能
						+get_uABC					//三相电压测量使能
						;
			CAN_INIT();	//当电机测量好初始值,才对CAN 初始化
			My_CAN_Send_Msg(reg_moto[REG_addr], 0, 0, (u8*)"FOC<-YGY", 8);	//必须发一帧,否则不能应答总线CAN
		}
	}
	//运行整个需要花费32us
}




