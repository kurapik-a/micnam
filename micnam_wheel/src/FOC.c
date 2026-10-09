#include "foc.h"

PID_Handle_t ID_PID=
{
	.Integrator_Limit=Vmax
	,.OUT_Limit=Vmax
	,.D_limit=0
	,.PID_Ts=TS_PWM
}
;
LPF_Handle_t ID_LPF=
{
	.F=1000			//滤波
	,.T_s=TS_PWM
	,.Actual=0
	,.Last=0
	,.Out=0
};
PID_Handle_t IQ_PID=
{
	.Integrator_Limit=Vmax
	,.OUT_Limit=Vmax
	,.D_limit=0
	,.PID_Ts=TS_PWM
}
;
LPF_Handle_t IQ_LPF=
{
	.F=1000			//滤波
	,.T_s=TS_PWM
	,.Actual=0
	,.Last=0
	,.Out=0
};
PID_Handle_t SPEED_PID=
{
	.Integrator_Limit=Vmax
	,.OUT_Limit=Vmax
	,.D_limit=0
	,.PID_Ts=TS_V_get
};//速度环
PID_Handle_t POS_PID=
{
	.Integrator_Limit=Vmax
	,.OUT_Limit=Vmax
	,.D_limit=0
	,.PID_Ts=TS_timer2
};//位置环

/*
    timer 为192000000表示1s中断
*/
void timer2irq_init(u32 timer)
{
    TIMer_base(UTIMER2, timer, 1);    // 配置空闲定时器 0.1s
    UTIMER2->IE = UTIMER_IRQEna_Zero; // 过零中断
    NVIC_EnableIRQ(TIMER2_IRQn);      // 开启中断
    NVIC_SetPriority(TIMER2_IRQn, 2); // 优先级
}

moto_ MOTO;
PCB_ PCB=
{
	.temp=30,
	.Voltage	=Vdc,
	.Over_ctrl_timer = 0xffffffff,
};
/*
功能:用于将电压转换为PWM占空比比值
输入范围：-PCB.Voltage/2 到  +PCB.Voltage/2  
*/
void ABC_OUT(float *UA, float *UB, float *UC)
{
    *UA = (*UA / PCB.Voltage + 0.5f) * PWMCCR; //	PWM最大值/母线电压 换算成输入 Vdc_Half 时的幅值为 Vdc
    *UB = (*UB / PCB.Voltage + 0.5f) * PWMCCR;
    *UC = (*UC / PCB.Voltage + 0.5f) * PWMCCR;
//	*UA = (*UA / Vdc + 0.5f) * PWMCCR; //	PWM最大值/母线电压 换算成输入 Vdc_Half 时的幅值为 Vdc
//    *UB = (*UB / Vdc + 0.5f) * PWMCCR;
//    *UC = (*UC / Vdc + 0.5f) * PWMCCR;
    *UA = limit(*UA, 0, PWMCCR - 50); // 限幅，因为超过 constantPWM_PERIOD 就会导致自举电容充电时间不足
    *UB = limit(*UB, 0, PWMCCR - 50); // 最大 constantPWM_PERIOD 大概500us
    *UC = limit(*UC, 0, PWMCCR - 50);
	if(*UA>PWMCCR_MAX_SET)	//限幅,否者自举困难
		*UA=PWMCCR_MAX_SET;
	if(*UB>PWMCCR_MAX_SET)
		*UB=PWMCCR_MAX_SET;
	if(*UC>PWMCCR_MAX_SET)
		*UC=PWMCCR_MAX_SET;
    ABC_OUT_PWM( *UA , *UB , *UC );	//给到PWM硬件
}
/*
功能:
    输入提前定义的变量
Direct:线序 1/0
Theta:电角度0-2pi
UD;反帕克变换的D轴电压输入 0+-Vdc_Half
UQ;反帕克变换的Q轴电压输入 0+-Vdc_Half
Ua;过程变量用于在实验中观察变换过程 可通过串口打印成波形
ub;过程变量用于在实验中观察变换过程 可通过串口打印成波形
UA;过程变量用于在实验中观察变换过程 可通过串口打印成波形
UB;过程变量用于在实验中观察变换过程 可通过串口打印成波形
UC;过程变量用于在实验中观察变换过程 可通过串口打印成波形
举例:
Seven_SVPWM
(
    &MOTO.DIV,
    &MOTO.Theta,
    &MOTO.UD,
    &MOTO.UQ,
    &MOTO.Ua,
    &MOTO.Ub,
    &MOTO.UA,
    &MOTO.UB,
    &MOTO.UC
);
*/
/*
SVPWM输入函数
UD,UQ是电压输出DQ轴矢量
注意，DQ轴矢量电压单位是V
*/
void Seven_SVPWM(int *Direct, float *theta, float *UD, float *UQ, float *Ua, float *Ub, float *UA, float *UB, float *UC)
{
    static float Max = 0, Min = 0, Adder = 0;
    // 帕克逆变换
    float temp1, temp2;
    temp1 = cosf(*theta);
    temp2 = sinf(*theta);
    *Ua   = *UD * temp1 - *UQ * temp2;
    *Ub   = *UQ * temp1 + *UD * temp2;

    // 克拉克逆变换
    *UA = *Ua;
    *UB = (Sqrt3 * *Ub - *Ua) / 2;
    *UC = (-*Ua - Sqrt3 * *Ub) / 2;

    Max   = *UA > *UB ? (*UA > *UC ? *UA : *UC) : (*UB > *UC ? *UB : *UC);
    Min   = *UA < *UB ? (*UA < *UC ? *UA : *UC) : (*UB < *UC ? *UB : *UC);
    Adder = (Max + Min) / 2; // 正弦波波峰消减量
	/*
	1.1547倍数处理，将电压最大值归一了，若24v母线
	最大矢量输入是+-13.856
	这里做了处理后，最大输入就是+-12
	*/
//    *UA   = (*UA + Adder) * 1.15470053838f;
//    *UB   = (*UB + Adder) * 1.15470053838f;
//    *UC   = (*UC + Adder) * 1.15470053838f;
	*UA   = (*UA - Adder);
	*UB   = (*UB - Adder);
	*UC   = (*UC - Adder);
	
	//电流法补偿:
	if( MOTO.IA>MOTO.DEADZONE_CUR_THR )			*UA +=Vdc_dead_time;	//+所需占空比	//电流超过阈值 MOTO.DEADZONE_CUR_THR 是电流噪声的3倍
	else if( MOTO.IA<-MOTO.DEADZONE_CUR_THR )	*UA -=Vdc_dead_time;	//-所需占空比 //电流超过阈值
	else
	{	//电压法补偿:
		if( *UA >MOTO.DEADZONE_VOL_THR)					*UA +=Vdc_dead_time;	//+所需占空比//大于占空比一半 +死区时间占空比
		else if( *UA <-MOTO.DEADZONE_VOL_THR)			*UA -=Vdc_dead_time;	//-所需占空比//小于占空比一半
		else *UA=*UA*MOTO.K_dead_time;	//非补偿区的线性过度
	}
	if( MOTO.IB>MOTO.DEADZONE_CUR_THR )			*UB +=Vdc_dead_time;	//+所需占空比
	else if( MOTO.IB<-MOTO.DEADZONE_CUR_THR )	*UB -=Vdc_dead_time;	//-所需占空比
	else 
	{	//电压法补偿:
		if( *UB >MOTO.DEADZONE_VOL_THR)					*UB +=Vdc_dead_time;	//+所需占空比//大于占空比一半 +死区时间占空比
		else if( *UB <-MOTO.DEADZONE_VOL_THR)			*UB -=Vdc_dead_time;	//-所需占空比//小于占空比一半
		else *UB=*UB*MOTO.K_dead_time;	//非补偿区的线性过度
	}
	if( MOTO.IC>MOTO.DEADZONE_CUR_THR )			*UC +=Vdc_dead_time;	//+所需占空比
	else if( MOTO.IC<-MOTO.DEADZONE_CUR_THR )	*UC -=Vdc_dead_time;	//-所需占空比
	else
	{ 	//电压法补偿:
		if( *UC >MOTO.DEADZONE_VOL_THR)					*UC +=Vdc_dead_time;	//+所需占空比//大于占空比一半 +死区时间占空比
		else if( *UC <-MOTO.DEADZONE_VOL_THR)			*UC -=Vdc_dead_time;	//-所需占空比//小于占空比一半
		else *UC=*UC*MOTO.K_dead_time; //非补偿区的线性过度
	}
    // 最大幅值为1
    if (*Direct) // 反向
        ABC_OUT(UA, UC, UB);
    else // 正向
        ABC_OUT(UA, UB, UC);
}
/*
作用:
    这是一个通过底层硬件
    与编码器通信的函数
    读取的数据是上一的角度
    所以需要通过速度超前预估一个角度 若转速不快的话,可以不预估
    TS_PWM * *Speed_ 就是预估
*/
//2026-3-18 经过改良,该函数只用于读取spi寄存器数据,数据发送已经在MCPWM中运行了
float Encoder(void)
{
//    static uint16_t raw_data;
//    uint8_t tx_buf[2] = {0x00, 0x00}; // MA732读取命令（根据实际协议调整）
//    uint8_t rx_buf[2];
//    SPI0_CS_L; // 拉低CS
//    // SPI2读写操作（发送命令并接收数据）
//    rx_buf[0] = MC_MCU_SPIxSendByte(SPI0, tx_buf[0]); // 发送并接收第1字节
//    rx_buf[1] = MC_MCU_SPIxSendByte(SPI0, tx_buf[1]); // 发送并接收第2字节
//    SPI0_CS_H;                                        // 拉高CS
//    raw_data = rx_buf[0] << 8 | rx_buf[1];
//    if (DIV_enco)
//        return -(float)raw_data * Encoder_Constant; // 转换为弧度
//    else
//        return (float)raw_data * Encoder_Constant; // 转换为弧度
	
	if (DIV_enco)
        return -(float)SPI0->RX_DATA * Encoder_Constant; // 转换为弧度
    else
        return (float)SPI0->RX_DATA * Encoder_Constant; // 转换为弧度
}

/*
作用:
    通过周期调用可算出:
    IN						:输入角度
    Theta_					:电角度(需要正确的磁极对数)
    Last_Mechanical_Angle_	:上一次机械角度
    Mechanical_Angle_		:机械角度
    Theta_Absoluteangle_	:绝对角度
    Laps_					:圈数(开机时为0)
    Speed_					:速度(输入变量)
    PP_						:磁极对数(输入变量)
举例:
    Angle_get
    (
        Encoder()					,	//原始角度0+-2pi输入(输入)
        &MOTO.Theta					,	//电角度输出
        &MOTO.Last_Mechanical_Angle	,	//上一次机械角度
        &MOTO.Mechanical_Angle		,	//机械角度
        &MOTO.Theta_Absoluteangle	,	//多圈机械角度
        &MOTO.Laps					,	//圈数
        &MOTO.Aw					,	//机械角速度
        &MOTO.PP					,	//磁极对数(输入)
    );
注意: IN是通过Encoder()函数获取到编码器角度 必须是2pi量程
	输入IN必须是校准过的编码器值
*/
void Angle_get(float IN, float *Theta_ADC,float *Theta_PWM, float *Last_Mechanical_Angle_, float *Mechanical_Angle_, float *Theta_Absoluteangle_, int *Laps_, float *Aw_, int *PP_)
{
    static int loop_theta   = 0;
    *Last_Mechanical_Angle_ = *Mechanical_Angle_;                            // 记录上一次角度
//    *Mechanical_Angle_      = *Theat_Offset_-IN;   // 读取编码器角度  机械角度 
	*Mechanical_Angle_=-IN;
    *Theta_ADC	=  (*Mechanical_Angle_ - *Aw_ * KTH7823_ADC )* *PP_ - loop_theta * _2PI ;	//向后预估延时1us	与ADC采样时刻一致
	*Theta_PWM	=  (*Mechanical_Angle_  + *Aw_ * KTH7823_PWM)* *PP_ - loop_theta * _2PI ; 	//向前预估50us 		与PWM输出时刻一致
	static float mind_angle;//中间变量
	mind_angle=*Mechanical_Angle_ - *Last_Mechanical_Angle_;
	if(mind_angle<0)	//取绝对值
		mind_angle=-mind_angle;
    if (mind_angle > PI) // 测出圈数
    {
        if (*Mechanical_Angle_ > *Last_Mechanical_Angle_)
            *Laps_ = *Laps_ - 1;
        else
            *Laps_ = *Laps_ + 1;
    }
    *Theta_Absoluteangle_ = (float)*Laps_ * _2PI + *Mechanical_Angle_; // 测出多圈绝对角度
}

/*
作用:
电流测量值通过帕克克拉克变换,得到的DQ轴电流
参数:
theta	:电角度	(输入)
I_A		:A相电流	(输入)
I_B		:B相电流	(输入)
I_C		:C相电流	(输入)
I_a		:阿拉法轴	(无感算法可能需要该参数作为输出)
I_b		:贝塔轴		(无感算法可能需要该参数作为输出)
I_d		:D轴(输出)
I_q		:Q轴(输出)
举例:
Clark_Park
(
    &MOTO.Theta	,//电角度
    &MOTO.IA	,//A相电流
    &MOTO.IB	,//B相电流
    &MOTO.IC	,//C相电流
    &MOTO.Ia	,//a轴电流
    &MOTO.Ib	,//b轴电流
    &MOTO.ID	,//D轴电流
    &MOTO.IQ	 //Q轴电流
);
*/
void Clark_Park(float *theta, float *I_A, float *I_B, float *I_C, float *I_a, float *I_b, float *I_d, float *I_q)
{
    *I_a = *I_A;
    *I_b = (*I_B - *I_C) * Clark_b;
    float cos_t = cosf(*theta);
    float sin_t = sinf(*theta);
    *I_d =  *I_a * cos_t + *I_b * sin_t;
    *I_q =  *I_b * cos_t - *I_a * sin_t;
}

// ------------------------------------------ 电机自动校准通用逻辑 ------------------------------------------
Calibration moto_Calibration;       // 电机校准函数结构体
LR_get moto_Calibration_LR;         // 电机电阻电感自动获取结构体
void QDA(float Q, float D, float A) //(函数从定义,这个函数是电压矢量配置函数)
{
    int DIV    = 1; // 反相
    MOTO.Theta_pwm = A;	//对齐pwm输出时刻的电角度
    MOTO.UD    = D;
    MOTO.UQ    = Q;
    Seven_SVPWM(
        &DIV,
        &MOTO.Theta_pwm,	//对齐pwm输出时刻的电角度
        &MOTO.UD,
        &MOTO.UQ,
        &MOTO.Ua,
        &MOTO.Ub,
        &MOTO.UA,
        &MOTO.UB,
        &MOTO.UC);
}
/*
0为母线电压中值
这是一个3相pwm设置函数,+-半母线电压 单位(V)
*/
void ABC_(float A, float B, float C)
{
    ABC_OUT(&A, &B, &C);
}
float enco(void) // 重定义编码器函数
{
    return Encoder();
}
void DQ_I(float *ID, float *IQ, float THETA) // 重定义ADC值获取函数(单位A)
{
    float a, b,mid_Iabc;
    //实际线序要根据电路来确定
	if(MOTO.DIV)
	{	MOTO.IB=(float)((ADC2_DAT0 >> 2)-MOTO.Current_offset[2])*constantADC_CURRENT_SCALE_C;
		MOTO.IC=(float)((ADC1_DAT0 >> 2)-MOTO.Current_offset[1])*constantADC_CURRENT_SCALE_B;
		MOTO.IA=(float)((ADC0_DAT0 >> 2)-MOTO.Current_offset[0])*constantADC_CURRENT_SCALE_A;}
	else
	{	MOTO.IC=(float)((ADC2_DAT0 >> 2)-MOTO.Current_offset[2])*constantADC_CURRENT_SCALE_C;
		MOTO.IB=(float)((ADC1_DAT0 >> 2)-MOTO.Current_offset[1])*constantADC_CURRENT_SCALE_B;
		MOTO.IA=(float)((ADC0_DAT0 >> 2)-MOTO.Current_offset[0])*constantADC_CURRENT_SCALE_A;}
	mid_Iabc=(MOTO.IC+MOTO.IB+MOTO.IA)/3;//零序电压
	MOTO.IA-=mid_Iabc;
	MOTO.IB-=mid_Iabc;
	MOTO.IC-=mid_Iabc;
    Clark_Park(
        &THETA, // 电角度
        &MOTO.IA,     // A相电流
        &MOTO.IB,     // B相电流
        &MOTO.IC,     // C相电流
        &a,     // a轴电流
        &b,     // b轴电流
        ID,     // D轴电流
        IQ      // Q轴电流
    );
//    *ID /= 2.8f;
}
/*
获取相电压函数 单位(V)
*/
void DQ_U(float *VD, float *VQ, float THETA)
{
    float a, b,mid_Uabc;
	if(MOTO.DIV)
	{	MOTO.GET_UB=MOTO.UB;//(float)((ADC1_DAT2>>2)-MOTO.Voltage_offset[2])*constantADC_Voltage_SCALE_C +(Vdc-PCB.Voltage)/2.0f ;	
		MOTO.GET_UC=MOTO.UC;//(float)((ADC1_DAT1>>2)-MOTO.Voltage_offset[1])*constantADC_Voltage_SCALE_B +(Vdc-PCB.Voltage)/2.0f ;	
		MOTO.GET_UA=MOTO.UA;//(float)((ADC0_DAT1>>2)-MOTO.Voltage_offset[0])*constantADC_Voltage_SCALE_A +(Vdc-PCB.Voltage)/2.0f ;	
	}else
	{
		MOTO.GET_UC=MOTO.UC;//(float)((ADC1_DAT2>>2)-MOTO.Voltage_offset[2])*constantADC_Voltage_SCALE_C +(Vdc-PCB.Voltage)/2.0f ;	//U UVW对印电路板 ABC是程序内的 因为会自动判断线序 无对印关系
		MOTO.GET_UB=MOTO.UB;//(float)((ADC1_DAT1>>2)-MOTO.Voltage_offset[1])*constantADC_Voltage_SCALE_B +(Vdc-PCB.Voltage)/2.0f ;	//V
		MOTO.GET_UA=MOTO.UA;//(float)((ADC0_DAT1>>2)-MOTO.Voltage_offset[0])*constantADC_Voltage_SCALE_A +(Vdc-PCB.Voltage)/2.0f ;	//W
	}
	mid_Uabc=(MOTO.GET_UC+MOTO.GET_UB+MOTO.GET_UA)/3;//零序电压
	MOTO.GET_UA-=mid_Uabc;
	MOTO.GET_UB-=mid_Uabc;
	MOTO.GET_UC-=mid_Uabc;
	
	Clark_Park(
        &THETA, // 电角度
        &MOTO.GET_UA,     // A相电流
        &MOTO.GET_UB,     // B相电流
        &MOTO.GET_UC,     // C相电流
        &a,     // a轴电流
        &b,     // b轴电流
        VD,     // D轴电流
        VQ      // Q轴电流
    );
}
// ---------------------- 重定义 ----------------------
void set_Calibration_Calibration(void)
{
    // 设置值
    moto_Calibration.TS  = TS_PWM; // 运行频率 1khz
    moto_Calibration.VIN = _2PI * 5;  // 电角度转速	6.28red/s
    
    moto_Calibration.DIV                  = 0; // 默认值
    moto_Calibration.PP                   = 0; // 默认值
    moto_Calibration.Zero_position_offset = 0; // 默认值
    moto_Calibration.Zero_I[0]            = 0; // 默认值
    moto_Calibration.Zero_I[1]            = 0; // 默认值
    moto_Calibration.Zero_I[2]            = 0; // 默认值
    moto_Calibration.over                 = 0; // 默认值
    moto_Calibration.time_delay[0]        = 0; // 默认值
    moto_Calibration.time_delay[1]        = 0; // 默认值
    moto_Calibration.time_delay[2]        = 0; // 默认值
    moto_Calibration.po                   = 0; // 默认值
    moto_Calibration.po_last              = 0; // 默认值
    moto_Calibration.po_enco              = 0; // 默认值
    moto_Calibration.po_enco_last         = 0; // 默认值
    moto_Calibration.laps                 = 0; // 默认值
    moto_Calibration.Absolute_po          = 0; // 默认值
    moto_Calibration.steps                = 0; // 默认值
}
#define step1  0
#define step2  1
#define step3  2
#define step4  3
#define step5  4
#define step6  5
#define step7  6
#define step8  7
#define step9  8
#define step10 9
#define step11 10
#define step12 11
#define step13 12
#define step14 13
/*
作用:
用于自动测量电机的磁极对数,电角度偏置,线序
输入结构体 Calibration
Moto_Calibration_Program(&moto_Calibration);
*/
void Moto_Calibration_Program(Calibration *stru)
{
    //	stru.over=0;
    switch (stru->steps) {
        case step1: // 第1步
        {
            stru->po += stru->VIN * stru->TS; // 速度时间换算为电角度
            QDA(0, stru->UIN, stru->po);      // 电机旋转,速度为stru.VIN/PP;//PP是磁极对数
            stru->time_delay[0]++;
            if (stru->time_delay[0] > (1.0f / stru->TS)) // 电机开环旋转1秒
            {
                stru->time_delay[0] = 0;
                stru->steps         = step2; // 下一步
            }
        } break;
        case step2: // 第2步
        {
            stru->po -= stru->VIN * stru->TS; // 速度时间换算为电角度
            QDA(0, stru->UIN, stru->po);      // 电机旋转,速度为stru.VIN/PP;//PP是磁极对数
            stru->time_delay[0]++;
            if (stru->time_delay[0] > (0.5f / stru->TS)) // 电机开环旋转0.5秒
            {
                stru->time_delay[0] = 0;
                stru->steps         = step3; // 下一步
                stru->po            = 0;
            }
        } break;
        case step3: {             // 电机复位等待 3步
            QDA(0, stru->UIN, 0); // 电机复位 0.5秒
            stru->time_delay[0]++;
            if (stru->time_delay[0] > (0.5f / stru->TS)) // 延时时间
            {
                stru->steps = step4; // 下一步
                QDA(0, 0, 0);        // 电机复位 0.5秒
                stru->time_delay[0]        = 0;
                stru->Zero_position_offset = enco();                 // 读取初始位置
                stru->Zero_position_offset = stru->po_enco = enco(); // Zero_position_offset 复用做方向识别
            }
        } break;
        case step4: {                         // 开始开环旋转电机 4
            stru->po += stru->VIN * stru->TS; // 速度时间换算为电角度
            QDA(0, stru->UIN, stru->po);      // 电机旋转,速度为stru.VIN/PP;//PP是磁极对数
            // ----------------- 绝对位置测量 -----------------
            stru->po_enco_last = stru->po_enco;
            stru->po_enco      = enco();
            if ((stru->po_enco_last - stru->po_enco) > PI) // 圈数判断
                stru->laps++;
            if ((stru->po_enco_last - stru->po_enco) < -PI)
                stru->laps--;
            stru->Absolute_po = stru->laps * _2PI + stru->po_enco; // 绝对位置测量
            // ----------------- 绝对位置测量 -----------------
            stru->time_delay[0]++;
            if (stru->time_delay[0] > (1.0f / stru->TS)) // 电机开环旋转1秒
            {
                stru->time_delay[0] = 0;
                stru->steps         = step5; // 下一步
                if (stru->Absolute_po - stru->Zero_position_offset > 0)
                    stru->DIV = 0; // 正转线序方向正确(逆时钟)
                else
                    stru->DIV = 1;                              // 反转
                stru->po_last              = stru->po;          // 记录电角度
                stru->Zero_position_offset = stru->Absolute_po; // Zero_position_offset 复用为磁极对数测量参数
            }
        } break;
        case step5: {                             // 检查电机线序后调整旋转方向,开始记录磁极对数 5
            if (stru->DIV == 0)                   // 方向没有改变,顺时针
                stru->po += stru->VIN * stru->TS; // 速度时间换算为电角度
            else                                  // 方向改变,逆时钟
                stru->po -= stru->VIN * stru->TS; // 速度时间换算为电角度
            QDA(0, stru->UIN, stru->po);          // 电机旋转,速度为stru->VIN/PP;//PP是磁极对数
            // ----------------- 绝对位置测量 -----------------
            stru->po_enco_last = stru->po_enco;
            stru->po_enco      = enco();
            if ((stru->po_enco_last - stru->po_enco) > PI) // 圈数判断
                stru->laps++;
            if ((stru->po_enco_last - stru->po_enco) < -PI)
                stru->laps--;
            stru->Absolute_po = stru->laps * _2PI + stru->po_enco; // 绝对位置测量
            // ----------------- 绝对位置测量 -----------------
            if (fabs(stru->Zero_position_offset - stru->Absolute_po) > _2PI) // 旋转了机械角度一周
            {
                QDA(0, stru->UIN, 0); // 定A相 这里一定会顿一下
                stru->time_delay[0] = 0;
				stru->time_delay[1] =0;
                stru->steps         = step6;                          // 下一步
                stru->po            = fabs(stru->po - stru->po_last); // 取差值 取正值
                while (stru->po > (_2PI - 0.28f))                     // 求余 0.28是容错
                {
                    stru->po -= _2PI; // 电角度周期个数
                    stru->PP++;       // 磁极对数测量(旋转过程中不丢步才能做到)
                }
            }
        } break;
        case step6: { // 开始校准机械角度 6
            stru->time_delay[0]++;
            QDA(0, stru->UIN, 0);                    // 定A相
            if (stru->time_delay[0] >= 0.5f / stru->TS+0.5f) // 电机锁A相0.5秒等待稳定
            {
				stru->time_delay[1]++;
				stru->Zero_position_offset += enco();    // 获取机械角度校准值
				if(stru->time_delay[1]> 0.5f / stru->TS+0.5f-1 )		//累计0.5s
				{
					stru->po=0;		//	清零
//					QDA(0, 0, 0);	//	定A相
					stru->time_delay[0] = 0;
					stru->time_delay[1]=0;
					stru->steps         = step7;                // 下一步
					stru->Zero_position_offset /= 0.5f / stru->TS; // 获取指向A轴时的机械角度
//					while (stru->Zero_position_offset < 0)      // 角度求余0-2pi
//						stru->Zero_position_offset += _2PI;
//					while (stru->Zero_position_offset > (_2PI / stru->PP))
//						stru->Zero_position_offset -= _2PI / stru->PP;
				}
            }
        } break;
        case step7: {                    // 对编码器做多段线性校准
            static int raozu        = 0; // 绕组个数 一共有pp*3个绕组
            static int time_jiaozun = 0, time_Stable = 0;
            // stru->VIN = _2PI * 4;  // 1秒转4个磁极对 校准一圈就是 PP*2PI
            if (time_jiaozun == 0)
			{
				stru->po += stru->VIN * stru->TS/ 2; // 速度时间换算为电角度 校准速度减慢 自增量为:2PI*5*0.00005/2  0.000785
				if (stru->DIV == 0)
					QDA(0, stru->UIN / 2, stru->po);      // 正转 使用额定的一半
				else
					QDA(0, stru->UIN / 2, -stru->po);      // 正转 使用额定的一半
            }
            if (fabs(stru->po - _2PI_3 * raozu) < 0.01f) // 判断到达绕组指向方向
            {
				stru->po     = _2PI_3 * raozu;		//强行指向120度绕组方向
				if (stru->DIV == 0)
					QDA(0, stru->UIN / 2, stru->po); // 正转 使用额定的一半
				else
					QDA(0, stru->UIN / 2, -stru->po);      // 反转 使用额定的一半
                if (time_Stable > (0.5f / stru->TS+0.5f)-1)
				{
					time_jiaozun++;
                    stru->A_offset[raozu] += enco(); // 获取机械角度累加值
				}
                else                                 // 等待稳定
                    time_Stable++;
                if (time_jiaozun > (0.5f / stru->TS+0.5f)-1) // 定角0.5秒
                {
                    stru->A_offset[raozu] /= (0.5f / stru->TS); // 获取机械角度平均值
                    time_jiaozun = 0;
                    if (raozu == stru->PP * 3 - 1) // 校准完成
                    {
                        stru->steps = step11;
                    }
                    raozu++;
                    time_Stable = 0;
                }
            }
            if (stru->po > _2PI * stru->PP * 2) // 电机开环转2圈认为错误
            {
                stru->steps = step11;
				TURN_PIN(led1,1);	//错误
				TURN_PIN(led2,0);	//错误
				TURN_PIN(led3,0);	//错误
            }
        } break;

        case step11: {      // 校准完成 保存在flash中,复位单片机,重新校准电流 此处需要根据不同的单片机来更改
            stru->over = 1; // 校准完成 可在此截断校准 截断校准将 stru->over=1;
            QDA(0, 0, 0);   // 提供4V电压在电阻上
        } break;
            // ABC_OUT(UA,UB,UC);
    }
}
void set_Calibration_LR_get(void)
{
    // 设置值
    moto_Calibration_LR.TS  = TS_PWM; // 运行频率 1khz
    moto_Calibration_LR.w   = 0;      // 方波角速度
    moto_Calibration_LR.UIN = 1.0f;    // 电压(v)		对于电阻采样V6版本 最大1V若大于1V,电压采集将会出问题
	
    moto_Calibration_LR.L           = 0; // 默认值
    moto_Calibration_LR.R           = 0; // 默认值
    moto_Calibration_LR.UD          = 0; // 默认值
    moto_Calibration_LR.UQ          = 0; // 默认值
    moto_Calibration_LR.ID          = 0; // 默认值
    moto_Calibration_LR.IQ          = 0; // 默认值
    moto_Calibration_LR.over        = 0; // 默认值
    moto_Calibration_LR.Absolute_po = 0; // 默认值
    moto_Calibration_LR.steps       = 0; // 默认值
}

/*
作用:
用于自动测量电机的磁极对数,电角度偏置,线序
输入结构体 Calibration
需要提前初始化 moto_Calibration
Moto_Calibration_RL(&moto_Calibration_LR);
*/
void Moto_Calibration_RL(LR_get *stru)
{
    switch (stru->steps) {
        case step1: // 第1步 //等待稳定
        {
            stru->time_delay[0]++;
            QDA(0, stru->UIN, 0);                        // 给D轴一个电压
            if (stru->time_delay[0] > (0.5f / stru->TS)) // 100ms时间用于测量电压电流  若20kHZ中断,将会累加5000次
            {
                stru->time_delay[0] = 0;
                stru->steps         = step2;
            }
        } break;
        case step2: // 第2步 给电压到电机,测量电流电压
        {
            float UD1, UQ1, ID1, IQ1;
            stru->time_delay[0]++;
            QDA(0, stru->UIN, 0); // 给D轴一个电压
            DQ_U(&UD1, &UQ1, 0);
            DQ_I(&ID1, &IQ1, 0);
            stru->UD += UD1;                             // 累加电压
            stru->ID += ID1;                             // 累加电流
            if (stru->time_delay[0] > (0.5f / stru->TS)) // 500ms时间用于测量电压电流  若20kHZ中断,将会累加5000次
            {
                QDA(0, 0, 0);
                stru->time_delay[0] = 0;
                stru->R             = stru->UD / stru->ID; // 算出D轴电阻 10.0用于纠偏
                stru->steps         = step3;// 即将进入电感测量
            }
        } break;
        case step3: // 测量电感
        {
            static float v_high_sum, v_low_sum; // 高低电压累加
            static float i_high_sum, i_low_sum; // 高低电流累加
            static u32 SAMPLES_PER_HALF = 0;
            static float delta_v;
            static float delta_i;
            static float dt;
            static float di_dt;
            static float v_corrected;
            static float v_actual;
            static float i_actual;
            static char k;
            stru->time_delay[0]++;
            if (stru->time_delay[0] >= (0.00025f/stru->TS)) //(0.0005f/stru->TS))	//频率:1khz // 半周期时间=500us，每次中断50us，需10次中断：10*50us=500us → 方波频率=1kHz
            {
                if (stru->time_delay[1] == 0) // 首次进入初始化
                {
                    v_high_sum       = 0.0f;
                    v_low_sum        = 0.0f;
                    i_high_sum       = 0.0f;
                    i_low_sum        = 0.0f;
                    SAMPLES_PER_HALF = 0;
                    v_actual         = 0;
                    i_actual         = 0;
                    k                = 0; // 确保初始状态与step2结束时的输出一致（step2结束输出-UIN，对应k=0的状态）
                }
                DQ_U(&v_actual, &stru->UQ, 0);
                DQ_I(&i_actual, &stru->IQ, 0);
                k                   = !k;
                stru->time_delay[0] = 0; // 归零
                if (k)                   // 频率:1khz
                {
                    v_low_sum += v_actual;
//					v_low_sum-=1;		//认为电压采集数据不可信
                    i_low_sum += i_actual;
                    QDA(0, stru->UIN, 0); // 电压为 +stru->UIN
                } else                    // 频率:2khz
                {
                    v_high_sum += v_actual;
//					v_high_sum+=1;		//认为电压采集数据不可信
                    i_high_sum += i_actual;
                    QDA(0, -stru->UIN, 0); // 电压为 -stru->UIN
                }
                SAMPLES_PER_HALF++;
            }
            stru->time_delay[1]++;
            if (stru->time_delay[1] >= (0.5f/stru->TS) ) //(0.5f/stru->TS))	// 总测量时间=0.5s，每次中断50us，需10000次中断：10000*50us=500,000us=0.5s
            {
                QDA(0, 0, 0);
                SAMPLES_PER_HALF /= 2;
                delta_v     = 0;
                delta_i     = 0;
                dt          = 0;
                di_dt       = 0;
                v_corrected = 0;
                // 计算电压差、电流差和变化率
                delta_v = (v_high_sum / (float)SAMPLES_PER_HALF) - (v_low_sum / (float)SAMPLES_PER_HALF);
                delta_i = (i_high_sum / (float)SAMPLES_PER_HALF) - (i_low_sum / (float)SAMPLES_PER_HALF);
                dt      = 0.00025f;            // 半周期时间（秒）
                if (fabs(delta_i) < 0.000001f) // 电流变化过小（接近0，可能电机卡死或接线问题）
                {
                    stru->L = -1.0f; // 用特殊值标记异常
                } else {
                    di_dt       = delta_i / dt;
                    v_corrected = delta_v - stru->R * delta_i;
                    stru->L     = (float)((float)v_corrected / (float)di_dt) ;	//校准系数2.0
                    // 合理性校验（根据电机实际参数调整范围，例如1μH~100mH）
                    if (stru->L < 0.00f) {
                        stru->L = 0; // 错误
                    }
                }
                stru->time_delay[1] = 0;
                stru->steps         = step4;
            }
        } break;
        case step4: {
            stru->over = 1; // 校准完成 stru->over=1;
        } break;
    }
}
/*
作用:
用于测量电机速度(red/s)
举例:
MC_Speed_GetActualSpeed
(
    &MOTO.Theta_Absoluteangle		,	//多圈角度(输入)
    &MOTO.Theta_lastAbsoluteangle	,	//上一次多圈角度(函数内得出)
    &MOTO.Mechanical_Angle			,	//机械角度(输入)用于更改临界点的速度值
    &MOTO.Laps						,	//圈数(输出)用于更改临界点的多圈角度值
    &MOTO.Aw							//速度 测量出来的速度
);
*/
void MC_Speed_GetActualSpeed(float *Theta_Absoluteangle_, float *Theta_lastAbsoluteangle_, float *Mechanical_Angle_, int *Laps_, float *speed_)
{
	if(*Theta_Absoluteangle_>6283.18f||
		*Theta_Absoluteangle_<-6283.18f
	)//绝对角度的最大限幅 1000圈
	{
		*Theta_lastAbsoluteangle_=*Mechanical_Angle_;
		*Theta_Absoluteangle_=*Mechanical_Angle_;
		*Laps_=0;
	}
	else
	{
		*speed_=(*Theta_Absoluteangle_-*Theta_lastAbsoluteangle_)/TS_V_get;//计算速度  根据运行周期计算速度
		*Theta_lastAbsoluteangle_=*Theta_Absoluteangle_;	//记录上一次多圈位置
	}
//    /*---- 1. 静态副本：上一次机械角 ----*/
//    static float mech_last = 0.0f;
//    /*---- 2. 机械角差分 + 跨圈修正（核心修正）----*/
//    float dMech = *Mechanical_Angle_ - mech_last;
//    if (dMech > PI) dMech -= _2PI;
//    if (dMech < -PI) dMech += _2PI;
//    /*---- 3. 速度计算（用修正后的差分）----*/
//    *speed_ = dMech / TS_timer2;
//    /*---- 4. 更新静态副本 ----*/
//    mech_last = *Mechanical_Angle_;
//    /*---- 5. 记录上一次多圈角度(1ms)----*/
//    *Theta_lastAbsoluteangle_ = *Theta_Absoluteangle_;
}

/*
用于停止电机运行
释放电机,电机将会被断开 自然停下
*/
void stop(void)
{
    MOTO.state &= ~(state_FOCSVPWM + state_FOCloopID+state_FOCloopIQ);	//关闭使能
	reg_moto[REG_MOTO_MODE]=MOTO.state;
    ID_PID.Ref        = 0;
    ID_PID.Integrator = 0;
    MOTO.UD           = 0.0f;
    IQ_PID.Ref        = 0;
    IQ_PID.Integrator = 0;
    MOTO.UQ           = 0.0f;
	SPEED_PID.Integrator = 0;
	SPEED_PID.Result = 0;
	SPEED_PID.Ref =0;
	POS_PID.Integrator = 0;
	POS_PID.Result = 0;
//	MOTO.Safety=1;
	GPIO_Config(GPIO1, GPIO_PinSource_2		, GPIO_Mode_OUT, GPIO_AF_GPIO);  //A相IN  
	GPIO_Config(GPIO1, GPIO_PinSource_4		, GPIO_Mode_OUT, GPIO_AF_GPIO);  //B相IN
	GPIO_Config(GPIO1, GPIO_PinSource_6		, GPIO_Mode_OUT, GPIO_AF_GPIO);  //C相IN
	GPIO_Config(GPIO2, GPIO_PinSource_12	, GPIO_Mode_OUT, GPIO_AF_GPIO);  //A相IN
	GPIO_Config(GPIO2, GPIO_PinSource_13	, GPIO_Mode_OUT, GPIO_AF_GPIO);  //B相IN
	GPIO_Config(GPIO2, GPIO_PinSource_14	, GPIO_Mode_OUT, GPIO_AF_GPIO);  //C相IN
	ALL_GPIO(GPIO1,GPIO_Pin_2	,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);
	ALL_GPIO(GPIO1,GPIO_Pin_4	,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);
	ALL_GPIO(GPIO1,GPIO_Pin_6	,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);
	ALL_GPIO(GPIO2,GPIO_Pin_12	,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);
	ALL_GPIO(GPIO2,GPIO_Pin_13	,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);
	ALL_GPIO(GPIO2,GPIO_Pin_14	,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);
	TURN_PIN(GPIO1,GPIO_Pin_2	,0);
	TURN_PIN(GPIO1,GPIO_Pin_4	,0);
	TURN_PIN(GPIO1,GPIO_Pin_6	,0);
	TURN_PIN(GPIO2,GPIO_Pin_12	,0);
	TURN_PIN(GPIO2,GPIO_Pin_13	,0);
	TURN_PIN(GPIO2,GPIO_Pin_14	,0);
}
//对电机从新上电 用于清除故障后的自动恢复,将引脚配置为PWM复用
void Power_on_moto(void)
{
	reg_moto[REG_MOTO_MODE]=MOTO.state;
    ID_PID.Ref        = 0;
    ID_PID.Integrator = 0;
    MOTO.UD           = 0.0f;
    IQ_PID.Ref        = 0;
    IQ_PID.Integrator = 0;
    MOTO.UQ           = 0.0f;
	SPEED_PID.Integrator = 0;
	SPEED_PID.Result = 0;
	SPEED_PID.Ref =0;
	POS_PID.Integrator = 0;
	POS_PID.Result = 0;
	POS_PID.Ref=MOTO.Theta_Absoluteangle;	//令目标位置等于此时位置
	
	MOTO.Safety=0;
	ABC_OUT_PWM(PWMCCR/2, PWMCCR/2, PWMCCR/2);
	GPIO_Config(GPIO1, GPIO_PinSource_2		, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //A相IN
	GPIO_Config(GPIO1, GPIO_PinSource_4		, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //B相IN
	GPIO_Config(GPIO1, GPIO_PinSource_6		, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //C相IN
	GPIO_Config(GPIO2, GPIO_PinSource_12	, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //A相IN
	GPIO_Config(GPIO2, GPIO_PinSource_13	, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //B相IN
	GPIO_Config(GPIO2, GPIO_PinSource_14	, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //C相IN
}

/* 磁编码器校准数据离线处理 + 运行时快速查表插值
 * 作者 : 杨光宇
 * 日期 : 2025-11-4
 * 用法 : 先调用 Data_preprocessing() 一次，再在中断里调用 enc2mech_fpu()
 */
#include <math.h>

/* ---------- 校准三件套 + 实际点数 ---------- */
#define NP_MAX  72
static float NC[NP_MAX*3];			// 编码器值(传感器测量的校准值)
static float K[NP_MAX*3];	// 区间斜率
static float WC[NP_MAX*3];		// 电角值(外部传感器/认为高精度/认为准确)

/* ---------- 离线只跑一次 ---------- */
void Data_preprocessing(void)
{
//	MOTO.PP * 3//锁角点数
//	reg_moto[REG_A_offset0 + i]的第0个点就是offset 电角度校准值
	int i;
	for(i=0;i<MOTO.PP * 3;i++)
	{
		NC[i]=*(float *)&reg_moto[REG_A_offset0 + i];	//获取编码器校准值
		if((NC[i]<NC[i-1])&& i>0  ) //非递增
			NC[i]+=_2PI;	//向上平移一圈
		WC[i]= _2PI / (MOTO.PP * 3) * i; 	//准确值 MOTO.PP * 3//锁角点数
	}
	//拓展宽度 到-2PI~+4PI
	for(i=0;i<MOTO.PP * 3;i++)
	{
		NC[i+MOTO.PP * 3*2]=NC[i]+_2PI;
		NC[i+MOTO.PP * 3]=NC[i];
		NC[i]-=_2PI;
		WC[i+MOTO.PP * 3*2]=WC[i]+_2PI;
		WC[i+MOTO.PP * 3]=WC[i];
		WC[i]-=_2PI;
	}
	//求K 公式:WC[I]+(IN-NC[I])*(WC[I+1]-WC[I])/(NC[I+1]-NC[I])
	//K[I]=(WC[I+1]-WC[I])/(NC[I+1]-NC[I])
	for(i=0;i<MOTO.PP*3*3;i++)
	{
		K[i]=(WC[i+1]-WC[i])/(NC[i+1]-NC[i]);
	}
	
	
//    int i, n = MOTO.PP * 3;          // 锁角点数
//    float p[NP_MAX + 1], in[NP_MAX + 1];
//	if(n>72) n=72;
//    /* 1. 搬数据 */
//    for (i = 0; i < n; ++i) {
//        p[i] = *(float *)&reg_moto[REG_A_offset0 + i]; // 原始编码器值
//        in[i] = _2PI / n * i;                          // 理论机械角
//    }

//    /* 2. unwrap 消除 2π 跳变 */
//    for (i = 1; i < n; ++i) {
//        float diff = p[i] - p[i - 1];
//        if (diff < -PI)      p[i] += _2PI;
//        else if (diff > PI)  p[i] -= _2PI;
//    }

//    /* 3. 整体平移到 [0,2π) 内（减小后续运算） */
//    float offset = fmodf(p[0], _2PI);
//    if (offset < 0) offset += _2PI;
//    for (i = 0; i < n; ++i) p[i] -= offset;

//    /* 4. 封尾：让最后区间斜率 = 首段斜率，实现环形闭合 */
//    p[n]  = p[0] + _2PI;
//    in[n] = in[0] + _2PI;

//    /* 5. 一次性算斜率和基点 */
//    for (i = 0; i < n; ++i) {
//        float dx = p[i + 1] - p[i];
//        float dy = in[i + 1] - in[i];
//        enc_base[i] = p[i];
//        base[i]     = in[i];
//        k_xielv[i]        = dy / dx;
//    }
//    k_xielv[n - 1] = k_xielv[0];        // 强制首尾斜率一致，消除 360° 跳变

//    cal_n = n;              // 记录实际点数
}

/* ---------- 中断级高速调用 ---------- */
float enc2mech_fpu(float x)
{	//输入是编码器原始值
	static int offset_i=0; //offset_i 保存了上一次的角度区间,因为角度不会突变,除非超高转速
	// 1. 正向查找（如果角度变大了）
    while (offset_i < (MOTO.PP * 3 * 3 - 1) && x > NC[offset_i+1]) {		//这里+1,是为了对齐K[offset_i]
        offset_i++;
    }
    // 2. 反向查找（如果电机反转，角度变小了）
    while (offset_i > 0 && x < NC[offset_i]) {	//这里不需要+1对齐,因为本身就已经对齐了
        offset_i--;
    }
	return WC[offset_i]+(x-NC[offset_i])*K[offset_i];//(WC[offset_i+1]-WC[offset_i])/(NC[offset_i+1]-NC[offset_i]);
//	return base[offset_i-1]+(x- *(float *)&reg_moto[REG_A_offset0 + offset_i-1] )/(*(float *)&reg_moto[REG_A_offset0 + offset_i] - *(float *)&reg_moto[REG_A_offset0 + offset_i-1])*(base[offset_i]-base[offset_i-1]);

}

float maf_f32(MAF_t *m, float x){
    m->sum += x - m->fifo[m->idx];   // 新样进，老样出
    m->fifo[m->idx] = x;
    m->idx = (m->idx + 1) % WIN;
    return m->sum / WIN;             // 平均输出
}
// ---------------------- 参数初始化 ----------------------
void set_const_init(void)
{
	MOTO.DIV=reg_moto[REG_DIV];		//电机线序
	MOTO.PP=reg_moto[REG_PP];		//磁极对数
	MOTO.Theat_Offset=*(float*)&reg_moto[REG_offsetTheta];	//电角度校准值
	MOTO.L=*(float*)&reg_moto[REG_L];						//从内存获取电感
	MOTO.R=*(float*)&reg_moto[REG_R];						//获取电阻
	MOTO.DEADZONE_CUR_THR=*(float*)&reg_moto[REG_DEADZONE_CUR_THR];	//死区补偿值  //电流大于0.3A的时候触发电流判断死区补偿,小于0.3A触发电压判断死区补偿  目前是禁止基于电流判断的死区补偿
	MOTO.DEADZONE_VOL_THR=*(float*)&reg_moto[REG_DEADZONE_VOL_THR];	//死区补偿电压值
	MOTO.K_dead_time=(MOTO.DEADZONE_VOL_THR+Vdc_dead_time)/MOTO.DEADZONE_VOL_THR;	//通过DEADZONE_VOL_THR计算出过零线性补偿斜率
	
	//电流环的输出限幅已经做了等比例限幅，把电压输出限制在电压圆内 限制值为： Vmax
	if(MOTO.L<0.00001f)		//电感太小
		MOTO.F_PI=100;
	else if(MOTO.L>0.01f)	//电感太大
		MOTO.F_PI=1000;
	else					//线性区电感
		MOTO.F_PI=MOTO.L*1000000;
	if((*(float*)&reg_moto[REG_LOOP_DQ_P]) <0.000000000001f )
	{
		ID_PID.Kp=MOTO.L*MOTO.F_PI*_2PI;	//带宽(角速度)
		IQ_PID.Kp=MOTO.L*MOTO.F_PI*_2PI;	//带宽(角速度)
	}else
	{
		ID_PID.Kp=*(float*)&reg_moto[REG_LOOP_DQ_P];
		IQ_PID.Kp=*(float*)&reg_moto[REG_LOOP_DQ_P];
	}
	if((*(float*)&reg_moto[REG_LOOP_DQ_I]) <0.000000000001f )
	{
		ID_PID.Ki=MOTO.R*MOTO.F_PI*_2PI;	//带宽(角速度)
		IQ_PID.Ki=MOTO.R*MOTO.F_PI*_2PI;	//带宽(角速度)
	}
	else
	{
		ID_PID.Ki=*(float*)&reg_moto[REG_LOOP_DQ_I];
		IQ_PID.Ki=*(float*)&reg_moto[REG_LOOP_DQ_I];
	}
	MOTO.REF_I=*(float*)&reg_moto[REG_Output_currentQ];		//Q轴转矩电流保存参数
	
	SPEED_PID.Kp=*(float*)&reg_moto[REG_LOOP_SPEED_P];					//速度环P参数获取
	SPEED_PID.Ki=*(float*)&reg_moto[REG_LOOP_SPEED_I];					//速度环I参数获取
	SPEED_PID.Integrator_Limit=Vmax;	//积分限幅给最大电压,已经被电流环输出控制了 搜索 SPEED_PID.Integrator_Limit 即可查到
	
	POS_PID.Kp=*(float*)&reg_moto[REG_LOOP_POS_P];						//位置环P参数获取
	POS_PID.Ki=*(float*)&reg_moto[REG_LOOP_POS_I];						//位置环I参数获取
	POS_PID.OUT_Limit=*(float*)&reg_moto[REG_Output_speed];				//位置环输出限幅 取于速度环输入保存参数
	POS_PID.Integrator_Limit=*(float*)&reg_moto[REG_Output_speed];		//位置环积分限幅 取于速度环输入保存参数

	PCB.Overtemperature=*(float*)&reg_moto[REG_Overcurrent];	//过流保护
	PCB.Overcurrent=  *(float*)&reg_moto[REG_Overcurrent]  ;	//加载过流保护值
	
	reg_moto[REG_ERR]=0;	//故障码清零
	reg_moto[REG_Overtime]=0xffffffff;	//超时判断配置默认 最长时间 若有需要,可初始化调短
}
// ---------------------- 参数初始化 ----------------------
// ---------------------- 寄存器和通信 ----------------------
void reg_Parse(u16 reg_addr,float Value)
{
//	static u16 sta_re=0;//为了状态不重复设置
	reg_moto[reg_addr]=Value;	//以u32格式保存在寄存器中(不需要小数)
	PCB.Over_ctrl_timer=reg_moto[REG_Overtime];		//刷新超时时间
	switch(reg_addr)		//reg_moto[]是32位int寄存器
	{
		case REG_addr				:	//设置设备地址
		{//	StrinW1;2;10
			reg_moto[REG_addr]=Value+0.5f;
		}break;
		case REG_Output_VoltageD	:	//输出电压D  无电流闭环
		{
			MOTO.UD=Value;
		}break;
		case REG_Output_VoltageQ	:	//输出电压Q  无电流闭环
		{
			if(MOTO.state&state_FOC_Current_protec)	//如果有电流环,那么就用Q轴电压加限幅的占空比模式
				MOTO.OUT_U=Value;
			else		//如果没有电流环,说明是调试模式 为DQ电压模式
				MOTO.UQ	=Value;
		}break;
		case REG_Output_currentD	:	//电流环模式设置D轴分量
		{
			ID_PID.Ref	=Value;
		}break;
		case REG_Output_currentQ	:	//电流环模式设置Q轴分量
		{
			if( MOTO.state&state_F )	//IF开环模式下更改PID输入
			{
				IQ_PID.Ref	=Value;
			}
			else if(MOTO.state&state_FOC_Current_protec)	//其他模式下作为 力矩设置
			{
				MOTO.REF_I=Value;	//作为电流限幅操作
				reg_moto[REG_Output_currentQ]=*(u32*)&Value;	//float模式保存
			}
			else
				IQ_PID.Ref	=Value;
		}break;
		case REG_Output_speed	:		//速度输入
		{
			if(MOTO.state&state_FOCloopspeed && !(MOTO.state&state_FOCloopsPOS))	//速度环模式下使用
				SPEED_PID.Ref	=Value;
			else if(MOTO.state&state_F)
				MOTO.A_openloop=Value;	//电角度开环速度设置
			if(MOTO.state&state_FOCloopsPOS)
			{
				POS_PID.Integrator_Limit=Value;	//设置为速度环输出限幅
				POS_PID.OUT_Limit=Value;
				reg_moto[REG_Output_speed]=*(u32*)&Value;	//float模式保存
			}
		}break;
		case REG_Output_POS	:			//位置输入
		{
			POS_PID.Ref	=Value;
			reg_moto[REG_Output_POS]=*(u32*)&Value;	//float模式保存
		}break;
		
		case REG_MOTO_MODE	:			//控制模式
		{
			MOTO.state=Value;	//模式设置 查找： state_Calibration_RL 可得到详细介绍 在state_Calibration_RL的下面
		}break;
		
		case REG_LOOP_DQ_P	:			//电流环P参数
		{
			ID_PID.Kp	=Value;
			IQ_PID.Kp	=Value;
			reg_moto[REG_LOOP_DQ_P]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_LOOP_DQ_I	:			//电流环I参数
		{
			ID_PID.Ki	=Value;
			IQ_PID.Ki	=Value;
			reg_moto[REG_LOOP_DQ_I]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_LOOP_SPEED_P	:		//速度环P参数
		{
			SPEED_PID.Kp=Value;
			reg_moto[REG_LOOP_SPEED_P]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_LOOP_SPEED_I	:		//速度环I参数
		{
			SPEED_PID.Ki=Value;
			reg_moto[REG_LOOP_SPEED_I]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_LOOP_POS_P	:			//位置环P参数
		{
			POS_PID.Kp=Value;
			reg_moto[REG_LOOP_POS_P]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_LOOP_POS_I	:			//位置环I参数
		{
			POS_PID.Ki=Value;
			reg_moto[REG_LOOP_POS_I]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_Calibration_A	:		//校准电压 校准模式
		{
			MOTO.state=state_Calibration_A;
			moto_Calibration.UIN=Value;
//			moto_Calibration_LR.UIN=Value;	//该值已经默认
		}break;
		case REG_Overtemperature	:	//过温保护值
		{
			reg_moto[REG_Overtemperature]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_DEADZONE_CUR_THR  	:	//电流死区过零判断配置
		{
			reg_moto[REG_DEADZONE_CUR_THR]=*(u32*)&Value;	//float模式保存
			MOTO.DEADZONE_CUR_THR=*(float*)&reg_moto[REG_DEADZONE_CUR_THR];	//死区补偿值  //电流大于0.3A的时候触发电流判断死区补偿,小于0.3A触发电压判断死区补偿  目前是禁止基于电流判断的死区补偿
		}break;
		case REG_DEADZONE_VOL_THR  	:	//电压死区过零判断配置
		{
			reg_moto[REG_DEADZONE_VOL_THR]=*(u32*)&Value;	//float模式保存
			MOTO.DEADZONE_VOL_THR=*(float*)&reg_moto[REG_DEADZONE_VOL_THR];	//死区补偿电压值
			MOTO.K_dead_time=(MOTO.DEADZONE_VOL_THR+Vdc_dead_time)/MOTO.DEADZONE_VOL_THR;	//通过DEADZONE_VOL_THR计算出过零线性补偿斜率
		}break;
		case REG_BATTERY_MAX_VOLTAGE:	//过压 即电池最高电压
		{
			reg_moto[REG_BATTERY_MAX_VOLTAGE]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_BATTERY_MIN_VOLTAGE:	//欠压 即电池最低电压
		{
			reg_moto[REG_BATTERY_MIN_VOLTAGE]=*(u32*)&Value;	//float模式保存
		}break;
		case REG_debug				:	//自动向串口发送数据
		{	//	StrinW1;21;1	(1关闭 0打开)
			reg_moto[REG_debug]=Value+0.5f;
		}break;
		case REG_save				:{	NVR_save();}break;	//保存数据 	StrinW1;21;1
		case REG_reset				:{	NVIC_SystemReset();}break;				//单片机复位
		case REG_flash_EE			:{	NVR_EraseSector(ADDR0);NVR_EraseSector(ADDR1);NVIC_SystemReset();}break;	//恢复出厂设置
		case REG_ERR				:{	MOTO.state=0X5380; reg_moto[REG_ERR]=0; Power_on_moto(); }break;		//故障码设置为0时清除故障 并且进入电机短路刹车模式
	}
	static u16 led_set=0;
	led_set++;
	if(led_set>100)
		TURN_PIN(led3,!GPIO_ReadOutputDataBit(led3));	//led翻转
}


///* =====================================================================
// * 功能: 无感FOC 滑模观测器(SMO) + 锁相环(PLL) 预测电角度
// * 输入:
// *   R    : 电机相电阻(欧姆)           	-> MOTO.R / REG_R
// *   L    : 电机相电感(H)              	-> MOTO.L / REG_L
// *   Ia,Ib: αβ轴电流(A)               -> &MOTO.Ia &MOTO.Ib
// *   Ua,Ub: αβ轴电压(V)(死区补偿后)   -> &MOTO.Ua &MOTO.Ub
// *   PP   : 磁极对数                  	-> MOTO.PP
// * 输出:
// *   Theta_pred: 预测电角度(rad, 0~2π) -> &MOTO.Theta_adc
// *   Omega_pred: 预测机械角速度(rad/s) -> &MOTO.Aw
// * 说明:
// *   1. 只需 R、L 两个电机参数即可运行(滑模对参数误差不敏感)
// *   2. 每个 PWM 周期(TS_PWM=50us)调用一次, 在电流采样完成后、SVPWM 之前调用
// *   3. 启动阶段需配合 IF/VF 开环拖动, 转速起来后切入本函数
// *   4. Ua/Ub 务必用死区补偿后的电压, 否则低速反电动势估计被死区污染
// * 调用示例:
// *   Sensorless_SMO(&SMO_Param, MOTO.R, MOTO.L, MOTO.PP, &MOTO.Ia, &MOTO.Ib, &MOTO.Ua, &MOTO.Ub, &MOTO.Theta_adc, &MOTO.Aw);
// * ===================================================================== */

///* SMO 参数结构体实例: 可调参数可按电机手动整定;
//   若填好 n_max_rpm/Ke_Vs/I_rated 并置 param_auto=1,
//   则在第一次切入无感时自动覆盖可调参数(见 Sensorless_SMO 首播块),
//   之后转速误差过大只重播种、不重算参数 */
//SMO_Param_t SMO_Param =
//{
//	/* ---- 可调参数(手动整定默认值) ---- */
//	0,			/* LPF_FC:    反电动势LPF截止频率(Hz), 推荐(3~8)*fe_max且<=f_sw/10 */
//	0,			/* K_RATIO:   滑模增益系数, 取1.1~1.5(略大于1保证收敛) */
//	0,			/* K_MIN:     滑模增益下限(V), 约1.2*Ke*we_min(最低稳定转速处BEMF幅值) */
//	0,			/* DELTA:     饱和函数边界(A), 约(1~3)%*额定电流幅值 */
//	0,			/* KP_PLL:    PLL比例, 约2*zeta*wn(zeta=0.707, f_pll=50~150Hz) */
//	0,			/* KI_PLL:    PLL积分, 约wn^2=(2PI*f_pll)^2 */
//	0.1,		/* EPS:       防除零 */
//	0,			/* Aw_err_limit: PLL误差限幅(rad), 大惯量电机取0.1~0.3防震荡 */
//	0,			/* PHI_COMP:  相位超前补偿系数(0~1.0), 从0.3保守起步, 验证Id~=0 */
//	0,			/* PHI_COMP_MAX: 补偿角限幅(rad,约45度), 防过补偿正反馈失控 */
//	0,			/* DPHI_MAX:  补偿角每周期变化率限幅(rad) */
//	0,			/* OMEGA_S_FC: 补偿用速度平滑截止频率(Hz), 滤PLL抖振 */
//	0,			/* OMEGA_MAX: PLL电气速度钳位(rad/s), 推荐1.2*2PI*PP*n_max/60 */
//	0,			/* PHI_ERR:   补偿角-误差负反馈系数, err<0(估计超前)自动减小补偿角 */
//	/* ---- 低速引导/过零穿越参数(param_auto=1时由自动块覆盖) ---- */
//	0.045f,		/* E_boot_thr:    BEMF不可观门限(V); #30 降Target速至75rpm后同步下调: 真Ke≈0.0008@78rad/s→Emag≈0.062V>0.045可切出; 失锁判据0.5*E_boot_thr=0.0225V */
//	104.7f,		/* Omega_boot:    引导电角速度(rad/s)≈2PI*PP*100rpm/60(机械约100rpm, PP=10); #32 78->105 提目标拉大BEMF裕量(注: param_auto=1时自动块以PP实算覆盖此值) */
//	200.0f,		/* Boot_accel:    引导电角加速度(rad/s^2); #30 400->200: 0->78rad/s约390ms, 更缓+J*α转矩需求减半, 大惯量才跟得上 */
//	100.0f,		/* Boot_switch_N: 引导→PLL切换确认周期数(~5ms); #28 40->100 防BEMF噪声/未拖起时"假切出" */
//	200.0f,		/* Boot_retry_N:  PLL失锁回引导周期数(~10ms) */
//	15000.0f,	/* Boot_timeout_N: 引导超时周期数(~750ms); #32 12000->15000 目标提到100rpm后爬坡(≈525ms)加长 */
//	0.3f,		/* K_smooth:      滑模增益K一阶平滑系数 */
//	0,			/* Dir_ref_src:   期望方向源: 0=auto(跟随MOTO.UQ符号) */
//	/* ---- 自动计算用电机参数(需按电机填写) ---- */
//	0,			/* n_max_rpm: 最高机械转速(rpm) */
//	0,			/* Ke_Vs:     相反电动势系数(V*s/rad)=磁链(Wb) */
//	0,			/* I_rated:   额定电流幅值(A) */
//	1,			/* param_auto: 1=首次切入无感时用电机参数自动覆盖可调参数 */
//	157.0f,		/* Dir_flip_omega_thr: 反转过零接管速度阈值(rad/s)≈0.5*Omega_boot */
//	50.0f,		/* Err_fake_N: fake-balance (err clamped & Omega~0) confirm ticks ~2.5ms */
//	300.0f,		/* Lock_hold_N: #25 lock-hold window ticks: after guide->PLL only a/e may re-boot, mask b/c/d to stop guide<->lock cycle (0=off) */
//	30.0f,		/* Dir_opp_N: #26 方向权威确认拍数(30≈1.5ms): UQ方向明确且Omega_pll*dir_ref<0持续此拍数→回引导拖过零 */
//	47.0f		/* Dir_opp_omega_thr: #26 方向相悖速度下限(rad/s)≈0.15*Omega_boot, 与BEMF判据a盲区无缝 */
//};

///* ============================================================================
// * Sensorless_SMO 说明(低速引导/方向修复已内置, 2009改):
// *  1) PLL误差乘方向符号w_dir(由BEMF叉积判向), 反转(-UQ)时统一为负反馈→不再发散;
// *     判向仅在BEMF可观区更新, 低速方向由"期望方向"dir_ref(跟随MOTO.UQ符号)播种。
// *  2) 低速引导bootstrap: |E|<E_boot_thr 判定为BEMF不可观(0速启动/正反过零/失锁恢复),
// *     角度由开环ramp按dir_ref方向拖动(等效I/F同步), 拖动到Omega_boot转速、BEMF可测且
// *     误差收敛连续Boot_switch_N拍后, 平滑切回PLL(角度同一积分器无跳变)。
// *     若引导超时(Boot_timeout_N)拖不动→0速冷却后重试, 防长时间堵转电流。
// *  3) 运行中突然±UQ: 电机减速→|E|跌回引导区→ramp按新方向拖过零点→PLL反向锁定, 全程err负反馈。
// *  4) 观测点: 上位机看 Omega 爬升→切出瞬间 Omega 连续; err 由打满→收敛即启动成功。
// *     调参: 启不动加大UQ; 低速抖把Omega_boot调低; 切出跳变看Boot_switch_N/E_boot_thr。
// *  5) 失锁接管增强(#24改): 锁定区新增三条互补回引导判据, 解决原"err连续打满200拍"被噪声
// *     清零导致状态机僵死PLL(boot_open长期0)的问题:
// *     c) err高频往返(滑窗统计符号翻转)  d) 假平衡(err打满且|Omega_pll|≈0, 锁死错误点)
// *     e) 反转监视窗(dir_ref跨死区翻号后 |Omega_pll|跌入Dir_flip_omega_thr 或反向→拖过零, 保守方案②)
// *     相位补偿在角度不可信(unlk_flag)时冻结; Omega_int 与指令方向反向且过大时按比例衰减防过冲。
// *     新参数: Dir_flip_omega_thr(默认0.5*Omega_boot≈157)/Err_fake_N(默认50拍)。
// *  6) Voltage open-loop stability fix (#25): 1) PLL BW f_pll=100->25Hz (KP=8.9*25~222, KI=(2pi*25)^2~2.47e4),
// *     Aw_err_limit 0.2->0.3: err no longer slams limit (swing KP*0.2=178rad/s = the big blue swing);
// *     2) voltage open-loop (no state_FOCloopID/IQ) disables phi-comp (phi_comp=0), Theta_out=Theta_pll
// *        removing speed->comp-angle->speed positive feedback;
// *     3) guide->PLL switch starts Lock_hold_N=300 tick window: inside only a)BEMF drop / e)reverse
// *        may re-boot (b/c/d masked), plus c) criterion threshold 0.6->0.9: breaks the
// *        guide<->lock cycle (seen as Omega_pll mean 67 but spikes at Omega_boot=314).
// * ============================================================================ */
///* 失锁接管增强(#24新增): 固定统计常数(不开放到SMO_Param, 需要时改这里) */
//#define SMO_DIR_FLIP_WIN_N   500		/* dir_ref跨死区翻号后, 反转穿越监视窗拍数(25ms) */
//#define SMO_ERR_FLIP_WIN_N   20			/* err高频往返统计窗拍数(1ms) */
//#define SMO_ERR_FLIP_MAX     17			/* 窗内err符号翻转>此值判err高频往返(角度不可信) */

//void Sensorless_SMO(SMO_Param_t *smo, float R, float L, int PP, float *Ia, float *Ib, float *Ua, float *Ub, float *Theta_pred, float *Omega_pred)
//{
//	/* 观测器内部状态(static 保持, 避免每次重新定义) */
//	static float Ihat_a=0.0f, Ihat_b=0.0f;	// 估计电流 A/B 轴
//	static float Ea=0.0f, Eb=0.0f;			// 反电动势估计 A/B 轴(低通后)
//	static float Theta_pll=0.0f;			// PLL 预测电角度
//	static float Omega_pll=0.0f;			// PLL 预测电气角速度
//	static float Omega_int=0.0f;			// PLL 积分项
//	static float Theta_out=0.0f;			// 补偿后输出电角度(用于播种检测)
//	static char  first=1;					// 首次切入无感标志
//	static float Omega_s=0.0f;				// 平滑电速度(相位补偿用)
//	static float phi_prev=0.0f;				// 上一周期相位补偿角
//	
//	static float Ea_prev=0.0f, Eb_prev=0.0f;	// 上一周期反电动势(叉积判向用)
//	static float w_dir=1.0f;					// 旋转方向(+1正转/-1反转), BEMF叉积判向+引导播种
//	static float K_raw=0.0f;					// 滑模增益K平滑存储
//	/* ---- 低速引导(bootstrap)/过零穿越状态: BEMF不可观时的开环同步拖动 ---- */
//	static char  boot_open=1;					// 1=开环引导(0速启动/过零穿越/失锁恢复)
//	static float dir_ref=1.0f;					// 期望方向(+1/-1): 跟随电压指令符号(MOTO.UQ)
//	static float boot_omega=0.0f;				// 引导角速度(rad/s, 加速度限幅缓升/缓降)
//	static float boot_ok_cnt=0.0f;				// 引导→PLL 切换条件连续满足计数
//	static float boot_retry_cnt=0.0f;			// PLL失锁(误差打满)连续计数
//	static float boot_time_cnt=0.0f;			// 引导超时计数
//	/* ---- 失锁接管增强状态(#24新增): dir_ref翻转监视/err高频往返/假平衡统计 ---- */
//	static float dir_prev=0.0f;				// 上一拍期望方向(翻转边沿检测用)
//	static int   dir_flip_win=0;			// 反转监视窗剩余拍数(0=不在窗内)
//	static float err_last=0.0f;				// 上一拍PLL误差(符号翻转检测用)
//	static int   err_flip_cnt=0;			// 统计窗内err符号翻转计数
//	static int   err_flip_win=0;			// 高频往返统计窗剩余拍数
//	static int   fake_cnt=0;				// 假平衡连续计数
//	static int   unlk_flag=0;				// 锁定区"角度不可信"综合标志(供相位补偿冻结)
//	static float lock_hold=0.0f;			// #25 lock-hold window remaining ticks: start on guide->PLL switch, only a/e may re-boot inside
//	static float emag_low_cnt=0.0f;		// #32 判据a的Emag低连续确认计数(防低速小BEMF瞬时毛刺误回引导)


//	/* 每周期临时变量(static 复用, 减小栈深度, 避免每次重新定义) */
//	static float sa=0.0f, sb=0.0f, za=0.0f, zb=0.0f;	// 滑模面 / 开关输出
//	static float Emag=0.0f;								// 反电动势幅值
//	static float err=0.0f;								// PLL 误差信号
//	static float err_sm=0.0f;							// #33 PLL误差一阶平滑(引导切出判据抗抖振尖峰)
//	static float K=0.0f;								// 滑模增益
//	static float w_s_coef=0.0f, w_lpf=0.0f, phi_comp=0.0f, dphi=0.0f;	// 相位补偿块临时变量
//	static float lpf_a=0.0f;							// 反电动势低通系数

//	static float dir_opp_cnt=0.0f;		// #26 方向权威: 反向持续运行(与dir_ref相悖)确认计数
//	if(PP<=0) PP=1;				// 防御: 磁极对数非法按1处理

//	/* 每周期: 期望方向参考 = Q轴电压指令符号(外部真值, 独立于SMO自反馈)
//	       0速启动与运行中突然±UQ 都靠它决定开环拖动/过零穿越的方向 */
//	if(smo->Dir_ref_src==0)
//	{
//		if(MOTO.UQ> 0.02f)      dir_ref= 1.0f;	// +UQ: 期望正转
//		else if(MOTO.UQ<-0.02f) dir_ref=-1.0f;	// -UQ: 期望反转
//		/* |UQ|<=0.02: 无方向指令, 维持上次 dir_ref */
//	}
//	else dir_ref=1.0f;

//	/* dir_ref 跨死区翻转边沿检测(#24): 有效电压指令反向(±UQ) → 开启反转穿越监视窗
//	      边沿统一在此更新(引导/锁定两区共用), 避免回引导后重复误触发;
//	      监视窗在锁定区递减并判定是否接管(见状态机 e)条) */
//	if(dir_prev!=0.0f && dir_ref!=dir_prev && fabsf(MOTO.UQ)>0.02f)
//	{
//		dir_flip_win=SMO_DIR_FLIP_WIN_N;
//	}
//	dir_prev=dir_ref;

//	/* 第一次切入无感: 自动计算参数(仅执行一次) + 播种角度/速度 */
//	if(first)
//	{
//		/* ================================================================
//		 * 自动参数计算(仅首次切入无感执行; 之后转速误差过大只重播种, 不重算):
//		 * 需要电机参数: R,L,PP 已由入参传入; n_max_rpm/Ke_Vs/I_rated 在 SMO_Param 填写
//		 *   Ke_Vs 测法: 反拖到 n(rpm) 测线反电动势有效值 E_line(V),
//		 *                Ke = E_line/sqrt(3) / (2PI*PP*n/60)
//		 * 公式:
//		 *   fe_max = PP*n_max/60 (Hz)                    最高电频率
//		 *   LPF_FC = (3~8)*fe_max, 且<=f_sw/10           滤滑模抖振
//		 *   K_MIN  = 1.2*Ke*we_min, we_min=2PI*PP*n_min/60  最低稳定转速兜底
//		 *   K_RATIO = 1.1~1.5                            K=K_RATIO*|E| 略大于BEMF
//		 *   DELTA  = (1~3)%*I_rated                      饱和函数边界
//		 *   f_pll  = 50~150Hz, zeta=0.707
//		 *   KP_PLL = 2*zeta*wn ~= 8.9*f_pll             KI_PLL = wn^2=(2PI*f_pll)^2
//		 *   OMEGA_MAX = 1.2*2PI*PP*n_max/60             电气速度钳位
//		 * ================================================================ */
//		if(smo->param_auto)
//		{
//			smo->n_max_rpm=    	6000.0f;	//目标最高机械转速(用户确认3000~6000rpm取上限6000; 实际跑不到可改小此值以收紧上限)
//			smo->Ke_Vs=       	0.01667f;	//反电动势系数  ★疑似偏大~20倍(按观测BEMF反推真实约0.0008), 建议反拖实测后替换(测法见函数头注释)
//			smo->Ke_Vs=smo->Ke_Vs/PP;		//和磁极对数有关
//			smo->I_rated=	    3.0f;		//额定电流
//			
//			/* #26: LPF_FC 固定250Hz(实测锁定值). 原auto公式=fe_max*4(PP=10,n_max=6000→4kHz)被钳到2kHz:
//			 * 带宽太宽几乎不过滤滑模开关+电流观测噪声→Ea/Eb抖振→PLL err打满±0.3→锁定区
//			 * b/c/d误触发回引导→引导/锁定循环(蓝色速度大摆+boot_open翻转). 实测LPF_FC<=300Hz
//			 * 自起+稳定+方向纠偏全正常, 取250Hz留裕量. 起动切出段BEMF基频远低于fc(增益≈1),
//			 * Emag>E_boot_thr切出判据不受影响. 高速限制: fe=1kHz@6000rpm时250Hz一阶低通
//			 * BEMF幅值衰减≈0.28/相位滞后≈76°, 电压开环phi_comp=0无补偿→高速段有失稳风险 */
//			smo->LPF_FC=		250.0f;		//#26 实测稳定区(<300Hz取250Hz留裕量); 高速限制与后续动态方案见FOC.h注释
//			smo->K_RATIO=		1.2f;									//固定1.2即可
//			//smo->K_MIN=		1.2f*smo->Ke_Vs*_2PI*(float)PP*100.0f/60.0f;	//Ke实测无误后可放开此公式
//			smo->K_MIN=	        0.05f;		//滑模增益下限(V): 显式兜底避开错误Ke; 太小低速噪声大,太大低速抖振/电流噪声
//			smo->DELTA=		    0.1f*smo->I_rated;								//预测电流饱和边界(下限)
//			smo->KP_PLL=		8.9f*25.0f;				//#25: f_pll 100->25Hz, KP=8.9*25~222; old 100Hz amplifies BEMF ripple into err full-swing/Omega swing
//			
//			
//			smo->KI_PLL=		(_2PI*25.0f)*(_2PI*25.0f);				
//			
//			
//			
//			
//			smo->EPS=			0.1f;		
//			smo->Aw_err_limit=	0.3f;		//#25: err limit 0.2->0.3 with lower PLL BW (big inertia small / small inertia large)
//			smo->PHI_COMP=		0.3f;		//相位补偿 0.3是保守值
//			smo->PHI_COMP_MAX=	0.785f;		//相位补偿最大值0.785弧度
//			smo->DPHI_MAX=		0.02f;		//默认值
//			smo->OMEGA_S_FC=	150.0f;		//默认值
//			smo->OMEGA_MAX=	    1.2f*_2PI*(float)PP*smo->n_max_rpm/60.0f;		//随6000rpm≈7540rad/s, 用来防角度飞走
//			smo->PHI_ERR=		0.6f;		
//			/* ---- 低速引导/过零穿越参数 ---- */
//			smo->E_boot_thr=	0.045f;		//#30: 目标速降至75rpm同步下调: 真Ke≈0.0008@78rad/s→Emag≈0.062V>0.045能切出; 失锁判据0.5*E_boot_thr=0.0225V(≈2.7rpm机械以下判不可观)
//			smo->Omega_boot=	_2PI*(float)PP*125.0f/60.0f;	//#30: 157->75rpm解决拖不动; #32 75->100rpm(手拨114rpm可进无感); #33 100->125rpm: 100rpm时Emag≈0.083V偏小, err被/Emag放大的噪声尖峰频繁越0.24切出门限→boot_ok_cnt凑不满100拍→boot_open恒1切不出; 125rpm→Emag≈0.105V(切出门限0.045的2.3倍/失锁0.0225的4.7倍), 配合err_sm平滑切出稳
//			smo->Boot_accel=	200.0f;		//#30: 引导电角加速度200rad/s^2(原400)→斜坡0→78约390ms, 更缓+J*α需求减半, 大惯量才跟得上(仍保持"缓慢0.4s上升")
//			smo->Boot_switch_N=	100.0f;		//#28: 引导→PLL切换确认 100拍≈5ms(原40≈2ms太短, BEMF噪声易假切出)
//			smo->Boot_retry_N=	200.0f;		//PLL失锁回引导 200拍≈10ms
//			smo->Boot_timeout_N=19000.0f;	//#28: 引导超时; #32 15000(750ms)配100rpm(爬坡≈525ms); #33 目标提到125rpm爬坡0→131rad/s@200≈655ms, 提到19000(950ms)留~300ms给切出确认+重试裕量
//			smo->K_smooth=		0.3f;		//滑模增益K一阶平滑系数: 抑抖振
//			smo->Dir_ref_src=	0;			//期望方向源: auto=跟随MOTO.UQ符号(±UQ双向启动/反转)
//			smo->Dir_flip_omega_thr=smo->Omega_boot*0.5f;	//反转过零接管速度阈值≈157rad/s(0.5*314, 保守防误触发)
//			smo->Err_fake_N=	50.0f;		//假平衡确认拍数: 50拍≈2.5ms
//			smo->Lock_hold_N=	300.0f;		//#25 lock-hold window ticks (300~15ms; enlarge if still cyclic or 0=off)
//			smo->Dir_opp_N=		30.0f;		//#26 方向权威确认拍数(30≈1.5ms)
//			smo->Dir_opp_omega_thr=	smo->Omega_boot*0.15f;	//#26 方向相悖速度下限≈47rad/s(≈45rpm机械), 与BEMF判据a盲区无缝
//			smo->param_auto=    1;			//初始化完成
//		}
//		/* 播种: 用切入瞬间的角度/速度初始化PLL, 避免从0锁相导致失步 */
//		Ihat_a=*Ia;
//		Ihat_b=*Ib;
//		Theta_pll=*Theta_pred;
//		Theta_out=*Theta_pred;
//		Omega_pll=*Omega_pred*(float)PP;
//		Omega_int=Omega_pll;	// 积分项同步播种=电气速度
//		Omega_s=Omega_pll;		// 速度平滑同步播种
//		phi_prev=smo->PHI_COMP*atanf(Omega_s/(_2PI*smo->LPF_FC+smo->EPS));	// 补偿角同步播种
//		if(phi_prev> smo->PHI_COMP_MAX) phi_prev= smo->PHI_COMP_MAX;
//		if(phi_prev<-smo->PHI_COMP_MAX) phi_prev=-smo->PHI_COMP_MAX;
//		Ea=0.0f;
//		Eb=0.0f;
//		err_sm=0.0f;			// #33 误差平滑播种清零
//		K_raw=smo->K_MIN;		// K平滑存储播种到下限, 避免启动首拍K=0
//		/* 首次播种: 按切入速度决定初始运行模式
//		       已有较高转速(>0.5*Omega_boot, 如VF开环拖起后切入无感)→直接PLL;
//		       0速/低速→开环引导, 转向由电压指令dir_ref决定(±UQ双向启动) */
//		if(fabsf(Omega_pll) > smo->Omega_boot*0.5f) boot_open=0;
//		else boot_open=1;
//		boot_omega=Omega_pll;			//ramp从当前速度连续起步
//		boot_ok_cnt=0.0f;
//		boot_retry_cnt=0.0f;
//		boot_time_cnt=0.0f;
//		if(fabsf(MOTO.UQ)>0.02f) w_dir=dir_ref;			//方向播种: 优先电压指令方向
//		else w_dir=(Omega_pll>=0.0f)?1.0f:-1.0f;		//无指令时按播种速度方向
//		first=0;
//	}
//	
//	
//	/* 失步/锁相失败兜底: 由下方引导状态机负责(err长期打满→自动回开环引导重新同步),
//	       不再使用旧Theta差检测(Theta_out与*Theta_pred恒等, 原判据检测无效) */

//	/* 1. 反电动势幅值(用上一周期估计值) */
//	Emag=FastSqrt(Ea,Eb,200.0f)+smo->EPS;  //反电动势模(幅值)
//	
//	
//	/* 2. 自适应滑模增益: K 略大于反电动势幅值, 收敛且抖振最小 */
//	K=smo->K_RATIO*Emag;
//	if(K<smo->K_MIN) K=smo->K_MIN;
//	/* 2.1 K 一阶平滑: 小L电机TS/L≈12 会把增益抖动放大, 平滑抑制抖振; 平滑后仍保下限 */
//	if(smo->K_smooth>0.0f)
//	{
//		K_raw+=(smo->K_smooth)*(K-K_raw);
//		K=K_raw;
//		if(K<smo->K_MIN) K=smo->K_MIN;
//	}
//	else K_raw=K;

//	/* 3. 滑模面(估计电流-实测电流) + 饱和函数开关 z=K*sat(s) */
//	sa=(Ihat_a-*Ia);		//当s为0,说明电流预测准确
//	sb=(Ihat_b-*Ib);
//	if(sa> smo->DELTA)      za= K;
//	else if(sa<-smo->DELTA) za=-K;
//	else                za= K*sa/smo->DELTA;
//	if(sb> smo->DELTA)      zb= K;
//	else if(sb<-smo->DELTA) zb=-K;
//	else                zb= K*sb/smo->DELTA;
//	/* 4. 电流观测器: L*di/dt = U - R*i - z  (一阶欧拉离散) */
//	Ihat_a+=(TS_PWM/L)*(*Ua-R*Ihat_a-za);		//通过积分计算出预测电流
//	Ihat_b+=(TS_PWM/L)*(*Ub-R*Ihat_b-zb);

//	/* 5. 反电动势提取: 滑模开关输出经一阶低通滤波 */
//	{
//		lpf_a=_2PI*smo->LPF_FC*TS_PWM;	// 低通系数 a=2PI*fc*Ts
//		Ea+=lpf_a*(za-Ea);
//		Eb+=lpf_a*(zb-Eb);
//	}

//	/* 5.1 旋转方向检测: BEMF叉积 cross=Ea*ΔEb-Eb*ΔEa ≈ -|E|^2*ω_e*Ts
//	       cross>0 → 反转(w_dir=-1), cross<0 → 正转(w_dir=+1)
//	       门限用 E_boot_thr: 低于该门限(BEMF是噪声)不判向, 方向由引导状态机接管 */
//	{
//		float cross=Ea*Eb_prev-Eb*Ea_prev;
//		float cross_thr=0.05f*Emag*Emag;
//		Ea_prev=Ea; Eb_prev=Eb;				//始终更新: 保证退出引导首拍判向立即可用
//		if(Emag>smo->E_boot_thr)			//BEMF太小(噪声主导)时不更新方向
//		{
//			if(cross> cross_thr)      w_dir=-1.0f;
//			else if(cross<-cross_thr) w_dir= 1.0f;
//		}
//	}
//	
//	
//	/* 6. PLL 误差: err=sin(te-t)=-(Ea*cos t + Eb*sin t)/|E|  (无需Ke)
//	      注意: 公式符号必须正确, 任何cos形式(如 Eb*cos t-Ea*sin t)都会
//	      锁到反向180度, 导致ID发散、电机反转 */
//	Emag=FastSqrt(Ea,Eb,200.0f)+smo->EPS;						//反电动势最大不会操作30这里给200足够用
//	/* PLL误差: err=-w_dir*sin(th_e-th) = -w_dir*(Ea*cos(th)+Eb*sin(th))/|E|
//	       w_dir=+1正转与原公式一致; 反转(we<0)时BEMF相位关系翻转,
//	       乘w_dir后err重新为负反馈, 否则反转必正反馈发散(-UQ发散根因) */
//	err=-w_dir*(Ea*cosf(Theta_pll)+Eb*sinf(Theta_pll))/Emag;	//当 Theta_pll 就是准确角度且uq=0 ud!=0得出err=-1,若uq!=0,ud=0得出err=0
//	
//	
//	if(err> smo->Aw_err_limit) err= smo->Aw_err_limit;	// 误差限幅(收窄, 更保守) 限幅为了抑制大惯量电机震荡一般给0.1-0.5
//	if(err<-smo->Aw_err_limit) err=-smo->Aw_err_limit;
//	err_sm+=0.15f*(err-err_sm);	// #33 err一阶低通(时间常数~7拍): 滤滑模/Emag噪声单拍尖峰, 供引导切出判据

//	/* 7/8. 角度与速度推进:
//	       引导区(boot_open=1): BEMF不可观(0速启动/过零/失锁恢复), 用ramp按期望方向dir_ref
//	       匀速拖动角度→等效开环同步; 切出前PLL积分/输出速度都跟ramp, 保证角度连续无跳变
//	       锁定区(boot_open=0): PLL锁相 w=Kp*err+Ki*∫err, 方向由w_dir纠正→±UQ双向不散 */
//	{
//		if(boot_open)
//		{
//			/* ===== 引导区: 开环ramp角度推进 ===== */
//			float boot_tar=smo->Omega_boot*dir_ref;		//目标速度=期望方向×引导速度
//			if(boot_omega<boot_tar)						//加速度限幅缓升/缓降, 防电流冲击与拖失步
//			{
//				boot_omega+=smo->Boot_accel*TS_PWM;
//				if(boot_omega>boot_tar) boot_omega=boot_tar;
//			}
//			else if(boot_omega>boot_tar)
//			{
//				boot_omega-=smo->Boot_accel*TS_PWM;
//				if(boot_omega<boot_tar) boot_omega=boot_tar;
//			}
//			boot_time_cnt+=1.0f;					//拖不动/堵转超时→0速冷却后重新爬升(防长时间堵转电流)
//			if(boot_time_cnt>smo->Boot_timeout_N)
//			{
//				boot_time_cnt=0.0f;
//				boot_ok_cnt=0.0f;
//				boot_omega=0.0f;					//速度回0
//				boot_retry_cnt=50.0f;				//冷却50拍(2.5ms)后重新尝试
//			}
//			if(boot_retry_cnt>0.0f)					//冷却期间保持0速
//			{
//				boot_retry_cnt-=1.0f;
//				boot_omega=0.0f;
//			}
//			Omega_int=boot_omega;		//引导期PLL积分跟随ramp: 切出时速度无缝衔接
//			Omega_pll=boot_omega;
//			Theta_pll+=Omega_pll*TS_PWM;	//ramp角直接积分(等效开环旋转磁场角)
//			/* 切出判定: BEMF已可测(>门限) 且 误差收敛(转子已被拖动同步) 连续Boot_switch_N拍 */
//			/* #28 切出需 ramp 已拖到接近目标速度(>90%Omega_boot): 防转子尚未被拖起/速度未到无感区间
//		       就因 BEMF噪声/err偏小 假切出PLL → 表现为 Omega_pll=0、角度不动 */
//			if(Emag>smo->E_boot_thr && fabsf(boot_omega)>0.9f*smo->Omega_boot &&
//			   fabsf(err_sm)<0.8f*smo->Aw_err_limit)   /* #30 低速小BEMF下err噪声相对大, 切出门限0.5->0.8*limit; #33 改用err_sm平滑值: 滤单拍抖振尖峰, 避免噪声打断100拍连续确认 */
//			{
//				boot_ok_cnt+=1.0f;
//				if(boot_ok_cnt>smo->Boot_switch_N)
//				{
//					boot_open=0;		//切回PLL: 角度连续(同一积分器) 速度已同步 无跳变
//					boot_ok_cnt=0.0f;
//					boot_time_cnt=0.0f;
//					boot_retry_cnt=0.0f;	//清冷却/失锁标志
//					dir_flip_win=0;		//清反转监视窗: 窗口仅用于锁定区过零检测, 引导期边沿不消费
//					w_dir=dir_ref;		//方向播种: 保证切回首拍err即为负反馈(防反转误判发散)
//					phi_prev=0.0f;		//清残留相位补偿角, 防输出角度跳变
//					lock_hold=smo->Lock_hold_N;		//#25 start lock-hold window (only a/e may re-boot, mask b/c/d)
//					emag_low_cnt=0.0f;			//#32 清Emag低计数(切出瞬间BEMF刚过线)
//				}
//			}
//			else boot_ok_cnt=0.0f;
//		}
//		else
//		{
//			/* ===== lock region: loss-of-lock/zero-cross detect, decide re-boot (#24 rewrite/#25 lock-hold) =====
//			 * Criteria (any do_boot => re-boot; unified boot_omega=Omega_pll continuous start + w_dir=dir_ref seed):
//			 *   a) BEMF drops into unobservable (zero-cross/stall): Emag<0.5*E_boot_thr (original, immediate always)
//			 *   b) err clamped continuous Boot_retry_N (original, keep as fallback)
//			 *   c) err high-freq flip: windowed sign-flip count > SMO_ERR_FLIP_MAX -> estimate unreliable
//			 *      (fixes stale PLL from continuous counter being noise-cleared; boot_open stuck at 0)
//			 *   d) fake balance: err clamped & |Omega_pll|<0.3*Omega_boot for Err_fake_N
//			 *      -> locked at wrong point (Omega~0 but err saturated, motor not spinning), fast re-boot
//			 *   e) reverse watch window: dir_ref crosses deadzone then |Omega_pll| below Dir_flip_omega_thr
//			 *      or reversed -> guide drags through zero in new direction
//			 * #25 lock-hold window (core): right after guide->PLL (lock_hold=Lock_hold_N>0) BEMF/err may
//			 *      still have transients; inside window only a)BEMF drop / e)reverse may re-boot (b/c/d masked),
//			 *      preventing guide<->lock cycling from err-clamped b/d false-trigger under high PLL gain
//			 *      (that cycle = the observed "speed always up/down", Omega_pll mean 67 spike at Omega_boot=314) */
//			int do_boot=0;
//			unlk_flag=0;			//"angle unreliable" flag: freeze phi-comp (no lead angle)
//			if(Emag<0.5f*smo->E_boot_thr)		//a) BEMF unobservable → re-boot
//			{
//				/* #32: 78rad/s时真BEMF≈0.062V贴0.5*E_boot_thr门限太近, 切出后观测瞬时
//				   毛刺把Emag打到门限下会立即误触发a回引导(a在lock_hold内也不屏蔽)
//				   →切出永远守不住(锯齿); 手拨到高速BEMF≈0.095V有4倍裕量→守得住.
//				   加~1.5ms(30拍)连续确认: 真堵转/过零Emag持续0仅延迟1.5ms, 无碍 */
//				if(emag_low_cnt>30.0f) do_boot=1;
//				else emag_low_cnt+=1.0f;
//			}
//			else
//			{
//				emag_low_cnt=0.0f;		//#32 Emag正常(≥门限)→清低计数
//				if(lock_hold>0.0f)			//#25 lock-hold window: just switched to PLL, mask b/c/d to stop guide<->lock cycle
//				{
//					lock_hold-=1.0f;
//					boot_retry_cnt=0.0f;		//clear: no residual count left to misfire after window
//					fake_cnt=0;
//					err_flip_cnt=0;
//					err_flip_win=0;
//				}
//				else				//window over: b/c/d active
//				{
//					if(fabsf(err)>0.9f*smo->Aw_err_limit)		//err clamped (loss-of-lock / fake balance)
//					{
//						unlk_flag=1;				//err at limit => unreliable => freeze phi-comp (no wait for counter)
//						boot_retry_cnt+=1.0f;
//						if(fabsf(Omega_pll)<0.3f*smo->Omega_boot)		//d) fake balance detect
//						{
//							fake_cnt++;
//							if(fake_cnt>smo->Err_fake_N) do_boot=1;
//						}
//						else fake_cnt=0;
//						if(!do_boot && boot_retry_cnt>smo->Boot_retry_N) do_boot=1;		//b) clamped fallback
//					}
//					else
//					{
//						boot_retry_cnt=0.0f;
//						fake_cnt=0;
//					}
//					/* c) err high-freq flip detect (#25 threshold 0.6->0.9 limit, less false trigger) */
//					if(fabsf(err)>0.9f*smo->Aw_err_limit && err_last*err<0.0f) err_flip_cnt++;
//					err_last=err;
//					if(err_flip_win>0) err_flip_win--;
//					else
//					{
//						if(err_flip_cnt>SMO_ERR_FLIP_MAX) { unlk_flag=1; do_boot=1; }
//						err_flip_cnt=0;
//						err_flip_win=SMO_ERR_FLIP_WIN_N;
//					}
//				}
//				/* e) reverse watch window (active also inside lock-hold): wait until reversed voltage slows to thr / reversed */
//				if(!do_boot && dir_flip_win>0)
//				{
//					dir_flip_win--;
//					if(fabsf(Omega_pll)<smo->Dir_flip_omega_thr || Omega_pll*dir_ref<0.0f)
//					{
//						do_boot=1;
//					}
//				}
//				/* f) 方向权威(#26): UQ方向明确(|UQ|>0.02)且实际旋转与指令方向持续相悖
//				 *    (外力拖反并被PLL锁住"记住"不回正) → 回引导按dir_ref ramp拖过零回正.
//				 *    仅dir_flip_win<=0时启用(主动±UQ翻转穿越由e)窗口处理, 避免高速翻负
//				 *    时f)抢跑回引导拖不住; 低LPF锁相牢靠后b/c/d不再误触发, f)显式补回
//				 *    LPF=2k抖振误触发时的偶然回正能力. 对称覆盖±UQ; 计数连续防0速抖动误判 */
//				if(!do_boot && dir_flip_win<=0 && fabsf(MOTO.UQ)>0.02f &&
//				   Omega_pll*dir_ref<0.0f && fabsf(Omega_pll)>smo->Dir_opp_omega_thr)
//				{
//					dir_opp_cnt+=1.0f;
//					if(dir_opp_cnt>smo->Dir_opp_N) { do_boot=1; unlk_flag=1; }
//				}
//				else dir_opp_cnt=0.0f;
//			}
//			if(do_boot)		//回引导: 统一置位+ramp连续起步+方向播种
//			{
//				boot_open=1;
//				boot_omega=Omega_pll;
//				boot_ok_cnt=0.0f;
//				boot_time_cnt=0.0f;
//				boot_retry_cnt=0.0f;
//				fake_cnt=0;
//				err_flip_cnt=0;
//				err_flip_win=0;
//				dir_flip_win=0;
//				dir_opp_cnt=0.0f;	//#26 清方向权威计数
//				w_dir=dir_ref;		//方向播种: ±UQ反转后按新指令方向拖过零
//				lock_hold=0.0f;			//#25 clear lock-hold: back to guide, next switch restarts
//				emag_low_cnt=0.0f;		//#32 清Emag低计数
//			}

//			if(boot_open)		//本周期刚判定回引导: 立即按引导ramp推进一拍(角度连续)
//			{
//				float boot_tar=smo->Omega_boot*dir_ref;
//				if(boot_omega<boot_tar)
//				{
//					boot_omega+=smo->Boot_accel*TS_PWM;
//					if(boot_omega>boot_tar) boot_omega=boot_tar;
//				}
//				else if(boot_omega>boot_tar)
//				{
//					boot_omega-=smo->Boot_accel*TS_PWM;
//					if(boot_omega<boot_tar) boot_omega=boot_tar;
//				}
//				Omega_int=boot_omega;
//				Omega_pll=boot_omega;
//				Theta_pll+=Omega_pll*TS_PWM;
//			}
//			else				//正常PLL锁相
//			{
//				Omega_int+=smo->KI_PLL*err*TS_PWM;
//				if(Omega_int> smo->OMEGA_MAX) Omega_int= smo->OMEGA_MAX;	//积分抗饱和:防PLL速度积分跑飞
//				if(Omega_int<-smo->OMEGA_MAX) Omega_int=-smo->OMEGA_MAX;
//				/* 指令方向约束(#24): UQ方向明确 且 Omega_int与指令方向相反 且幅值>Omega_boot
//				   →按比例衰减拉回, 消除±UQ反转时旧方向积分残留导致的转速过冲/发散 */
//				if(fabsf(MOTO.UQ)>0.02f && Omega_int*dir_ref<0.0f && fabsf(Omega_int)>smo->Omega_boot)
//				{
//					Omega_int*=0.95f;
//				}
//				Omega_pll =smo->KP_PLL*err+Omega_int;	//得到角速度
//				/* #31 lock-hold(刚切出)窗口内速度软限幅: 切出瞬间 err噪声/瞬态×高KP_PLL.
//				   (实测78→132峰值)顶出超调→角度飞走→锁定区判据拉回引导(锯齿).
//				   限幅到目标±25%让PLL从容锁相, 压掉切出过冲; lock_hold递减期间生效后放开 */
//				if(lock_hold>0.0f)
//				{
//					float om_lim=1.25f*fabsf(smo->Omega_boot);
//					if(Omega_pll> om_lim) Omega_pll= om_lim;
//					else if(Omega_pll<-om_lim) Omega_pll=-om_lim;
//				}
//				Theta_pll+=Omega_pll*TS_PWM;			//积分得到电角度
//			}
//		}
//	}
//	/* 速度平滑(引导与锁定共用): 供相位补偿计算滞后角, 滤PLL抖振 */
//	w_s_coef = _2PI*smo->OMEGA_S_FC*TS_PWM;	// 速度平滑系数
//	Omega_s+= w_s_coef*(Omega_pll-Omega_s);
//	/* 8. 角度归一化到 0~2PI */
//	if(Theta_pll> _2PI) Theta_pll-=_2PI;
//	else if(Theta_pll<0) Theta_pll+=_2PI;

//	/* 9. 相位超前补偿(带防正反馈保护):
//	       滞后角 phi=atan(we/w_lpf). 若补偿角过大/过快, 在"Q轴给目标电压+D轴闭环电流"
//	       模式下会正反馈失控: 补偿超前->旋转坐标系分解错位->实际力矩变大->we升高
//	       ->phi再变大->更超前->越转越快直至崩掉. 因此必须:
//	       a) 用平滑速度Omega_s算phi(滤PLL抖振, 防补偿角跳动)
//	       b) 幅值限幅PHI_COMP_MAX(45度)  c) 每周期变化率限幅DPHI_MAX
//	       d) PLL失锁冻结(误差打满说明估计不可信, 防误超前) */
//	{
//		if(!boot_open)		/* no comp in guide: ramp angle is open-loop, adding lead corrupts it */
//		{
//			if((MOTO.state&(state_FOCloopID|state_FOCloopIQ))==0)
//			{
//				/* #25: pure voltage open-loop (0X24104: no state_FOCloopID/IQ current loop) disables phi-comp. */
//				/* phi_comp=PHI_COMP*atan(we/w_lpf)+PHI_ERR*err was designed for closed current loop LPF lag */
//				/* compensation; open-loop has no current feedback, lead angle -> dq mis-decompose -> torque/we */
//				/* rise -> phi grows -> positive feedback runaway -> must keep phi_comp=0 here */
//				phi_comp=0.0f;
//				phi_prev=0.0f;
//			}
//			else			//closed current loop: normal LPF-lag compensation
//			{
//				w_lpf    = _2PI*smo->LPF_FC;			// LPF corner angular freq (rad/s)
//				/* d) loss-of-lock protect (#24): err clamped/high-freq/fake (unlk_flag) => freeze comp angle */
//				if(unlk_flag || fabsf(err)>0.9f*smo->Aw_err_limit) phi_comp=phi_prev;
//				else
//				{
//					phi_comp = smo->PHI_COMP*atanf(Omega_s/(w_lpf+smo->EPS)) + smo->PHI_ERR*err;		// target lead + err feedback (anti over-comp)
//				}
//				if(phi_comp> smo->PHI_COMP_MAX) phi_comp= smo->PHI_COMP_MAX;		// b) clamp
//				if(phi_comp<-smo->PHI_COMP_MAX) phi_comp=-smo->PHI_COMP_MAX;
//				dphi=phi_comp-phi_prev;				// c) rate limit
//				if(dphi> smo->DPHI_MAX) dphi= smo->DPHI_MAX;
//				if(dphi<-smo->DPHI_MAX) dphi=-smo->DPHI_MAX;
//				phi_comp=phi_prev+dphi;
//				phi_prev=phi_comp;
//			}
//			Theta_out     = Theta_pll + phi_comp;			// output angle (phi_comp=0 in open-loop)
//		}
//		else Theta_out = Theta_pll;					// guide: direct ramp angle
//		
//		if(Theta_out> _2PI) Theta_out-=_2PI;
//		else if(Theta_out<0) Theta_out+=_2PI;
//		*Theta_pred    = Theta_out;
//	}

////	*Theta_pred    = Theta_pll;
//	/* 10. 输出: 机械角速度 */
//	*Omega_pred=Omega_pll/(float)PP;	//输出机械角速度
//}



//sm_Param_t sm_GET=
//{
//	.Ls=0.00001f		//10uH(实测两相线电感/2); 离散稳定条件 Rs*T/Ls<2: 原1uH时=2.5发散, 改10uH后=0.25稳定
//	,.Rs=0.05f
//	,.Kp=0.02f		//按极点配置起步: zeta=(Rs+Kp)/(2*sqrt(Ls*Ki))≈0.7 (抖振大则减小)
//	,.Ki=250.0f		//wn=sqrt(Ki/Ls)=5000rad/s≈Rs/Ls (原10过阻尼zeta=3收敛太慢; 保守可先试100)
//};	// SMO参数实例(在FOC.c中初始化, 上电时可按电机修改)
///*
//SMO_Sensorless(EKF_GET,MOTO.Ia, MOTO.Ib, MOTO.Ua,MOTO.Ub,&MOTO.Theta_adc, &MOTO.Aw);
//*/
//void SMO_Sensorless(sm_Param_t * SMO_,float ia,float ib,float ua,float ub,float *angle,float *angular)
//{
//	static float T=0.00005f;	// 20kHz; ★必须与实际PWM/ADC采样周期严格一致
//	static float lpf_a=0.15f;	// 反电动势低通系数 a=2PI*fc*T (fc≈480Hz)
//	static float EMAG_MIN=0.05f;// 反电动势有效门限(V): 低于此值角度纯噪声(0速/极低速不可观)
//	static float EAS,EBS;		//积分
//	static float th_prev=0.0f;	//上一拍角度(差分求速度)
//	static float ea_raw,eb_raw,emag,th,dth,half;	//每周期临时

//	/* 1. 电流观测器: L*di/dt = u - R*i - e (前向欧拉; 本机 T/Ls=5, Rs*T/Ls=0.25<2 稳定) */
//	SMO_->ia_est+=T*(-SMO_->Rs*SMO_->ia_est+(ua-SMO_->ua_est))/SMO_->Ls;		//预估电流
//	SMO_->ib_est+=T*(-SMO_->Rs*SMO_->ib_est+(ub-SMO_->ub_est))/SMO_->Ls;

//	/* 2. 电流误差 -> PI -> 反电动势(软开关替代sign); 积分累加后立即限幅写回(抗饱和windup) */
//	SMO_->ia_err=(SMO_->ia_est-ia);
//	SMO_->ib_err=(SMO_->ib_est-ib);
//	EAS+=SMO_->ia_err*T*SMO_->Ki;
//	EBS+=SMO_->ib_err*T*SMO_->Ki;
//	EAS=limit(EAS,-12.0f,12.0f);
//	EBS=limit(EBS,-12.0f,12.0f);
//	ea_raw=limit(EAS+SMO_->ia_err*SMO_->Kp,-12.0f,12.0f);//比例积分和限幅控制器得到反电动势补偿量
//	eb_raw=limit(EBS+SMO_->ib_err*SMO_->Kp,-12.0f,12.0f);

//	/* 3. 低通滤波 -> 反电动势估计(滤掉比例项高频抖振); 滤波结果同时反馈给电流观测器 */
//	SMO_->ua_est+=lpf_a*(ea_raw-SMO_->ua_est);
//	SMO_->ub_est+=lpf_a*(eb_raw-SMO_->ub_est);

//	/* 4. 角度提取: 必须四象限! 原atanf只有±PI/2, 会每半周跳变PI→FOC失控
//	      e_a=-Ke*w*sin(th), e_b=Ke*w*cos(th)  =>  th=FastAtan2(-e_a, e_b)
//	      ★若实测转向相反, 改成 FastAtan2(e_a, -e_b)(建议开环拖动对比真实角度核对) */
////	emag=FastSqrt(SMO_->ua_est,SMO_->ub_est,17.0f);	//反电动势幅值; xmax取17=12*√2(两轴同时到限幅时模16.97>xmax会溢出反号)
//	th=FastAtan2(-SMO_->ua_est,SMO_->ub_est);		//工程DSP加速版atan2(y,x), 输出[-PI,PI]
//	if(th<0.0f) th+=_2PI;							//归一化到 [0,2PI)


//	*angle=th;
//	
//	
//	/* 5. 电角速度: 角度差分(处理跨越2PI缠绕) */
////	dth=th-th_prev;
////	half=_2PI*0.5f;
////	if(dth> half) dth-=_2PI;
////	else if(dth<-half) dth+=_2PI;
////	th_prev=th;

////	/* 6. 输出: 反电动势够大才更新, 避免0速时纯噪声乱输出 */
////	if(emag>EMAG_MIN)
////	{
////		*angle=th;
////		*angular=dth/T;			//电角速度(rad/s); 机械角速度 = 此值/PP
////	}
//}


