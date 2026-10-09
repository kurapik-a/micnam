#include "lks32mc45x.h"                 // Device header
#include "main.h"
#include "mymode.h"
/*
说明
	使用DSP计算再转换为浮点数
注意:
	需要调用 CORDIC_Enable(); 来开启DSP时钟
*/

// 修正版：弧度 → 0…65535 相位，零误差、无分支、无浮点乘
#include <stdint.h>

#define RADTICKS 10430.378350470f          // 65536/(2π) 精确值

static inline uint16_t rad_to_u16(float rad)
{
    int32_t tmp = (int32_t)(rad * RADTICKS);
    return (uint16_t)((tmp + 65536) & 0xFFFFu);   // 保证 ≥0 再取模
}

float sinf(float w)
{
    int16_t ph = (int16_t)rad_to_u16(w);
    return CORDIC_CalcSin(ph) * (1.0f / 32768.0f);
}

float cosf(float w)
{
    int16_t ph = (int16_t)rad_to_u16(w);
    return CORDIC_CalcCos(ph) * (1.0f / 32768.0f);
}

/* ============================================================
 * FastSqrt: 硬件 CORDIC 求模长 √(x2+y2)
 * 输入: x,y 浮点分量(量纲一致, 如电压), xmax 为允许的最大量纲值
 *       |x|,|y| 必须 ≤ xmax, 否则 Q15 溢出反号
 * 输出: √(x2+y2), 与输入同量纲
 * 原理: CORDIC MOD 模式对 s16(Q15) 两输入一次求模, 远快于软浮点 sqrtf
 * 定标: 32767 ? xmax (与 sinf/cosf 的 32768?1.0 同一套 Q15)
 * ============================================================ */
float FastSqrt(float x, float y, float xmax)
{
	if(xmax<=0.0f) xmax=1.0f;
	short qx=(short)(x*(32767.0f/xmax));
	short qy=(short)(y*(32767.0f/xmax));
	return (float)CORDIC_CalcMod(qx,qy)*(xmax/32767.0f);
}

/* ============================================================
 * atanf / FastAtan2: 硬件 CORDIC 反正切加速 (替代数学库软浮点)
 * DSP 库函数: s16 CORDIC_CalcArctan(s16 x, s16 y) = atan2(y, x)
 *   输入为 Q15 坐标(int16), 输出 int16 相位, 定标: 满圈 2π = 65536
 *   (与 sinf/cosf 的角度格式一致), 相位->弧度: ph/65536*2π
 * 注意: 使用前需 CORDIC_Enable() 开启 DSP 时钟
 * ============================================================ */
#define ATAN_PH2RAD  (6.28318530717958f / 65536.0f)   /* 1LSB相位对应弧度 */

float atanf(float x)        // 单参数反正切, 等价库函数 atan(x), 输出[-π/2,π/2]
{
    int16_t xi, yi;
    float ax = (x > 0.0f) ? x : -x;

    if (ax <= 1.0f)
    {                       // |x|<=1: 直接按 y=x、x=1 映射到 Q15, 精度最高
        xi = 32767;
        yi = (int16_t)(x * 32767.0f);
    }
    else                    // |x|>1: 反向压缩比值, 防 Q15 溢出, 仍保 atan(x) 结果
    {
        yi = (x > 0.0f) ? 32767 : -32767;
        xi = (int16_t)(32767.0f / ax);
        if (xi < 1) xi = 1;
    }
    return (float)CORDIC_CalcArctan(xi, yi) * ATAN_PH2RAD;
}

float FastAtan2(float y, float x)   // 四象限反正切, 等价库函数 atan2f(y,x), 输出[-π,π]
{
    if (x == 0.0f && y == 0.0f) return 0.0f;
    float ax = (x > 0.0f) ? x : -x;
    float ay = (y > 0.0f) ? y : -y;
    float k  = 32767.0f / ((ax > ay) ? ax : ay);    // 公共缩放保比值, 避免超 Q15
    int16_t xi = (int16_t)limit(x * k, -32767.0f, 32767.0f);
    int16_t yi = (int16_t)limit(y * k, -32767.0f, 32767.0f);
    return (float)CORDIC_CalcArctan(xi, yi) * ATAN_PH2RAD;
}






/*
举例:
ALL_GPIO(GPIO1,GPIO_Pin_1,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);
GPIO_ReadInputDataBit(GPIO1,GPIO_Pin_1);
作者:杨光宇 2025-7-23
*/
void ALL_GPIO(GPIO_TypeDef *GPIOx,u32 GPIO_Pin_x,GPIO_Mode_TypeDef GPIO_Mode,GPIO_PuPd_TypeDef PD)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct); // 初始化结构体
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode; // GPIO输出模式
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_x;
	GPIO_InitStruct.GPIO_PuPd = PD;		//拉电阻
	GPIO_InitStruct.GPIO_PFLT=0;		// 滤波使能
	if(GPIO_Mode==GPIO_Mode_INOUT)		//双向口
		GPIO_InitStruct.GPIO_PODEna=1;	// 开漏
	else GPIO_InitStruct.GPIO_PODEna=0;	//不开漏
	GPIO_Init(GPIOx, &GPIO_InitStruct); 
}
/*
举例:
TURN_PIN(GPIO1,GPIO_Pin_1,1);
作者:杨光宇 2025-7-23
*/
void TURN_PIN(GPIO_TypeDef *GPIOx,u32 GPIO_Pin_x,u8 of)
{
	if(of)	GPIOx->BSRR = GPIO_Pin_x;
	else	GPIOx->BRR = GPIO_Pin_x;
}

/*
void UART0_IRQHandler(void)
{
	if (UART_GetIRQFlag(UART0, UART_IF_SendOver))// /发送完成中断
	{	
		UART0_.T_c=1;	//标志位置位,下一次发送做准备
		UART_ClearIRQFlag(UART0, UART_IF_SendOver);
	}
	if (UART_GetIRQFlag(UART0, UART_IF_RcvOver)) //接收完成中断/
	{
		UART_ClearIRQFlag(UART0, UART_IF_RcvOver);	
	}
	if(UART_GetIRQFlag(UART0,BIT8))	//空闲中断
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
注意 串口有空闲中断,但是:开启接收完成中断会导致空闲中断失效,DMA接收也会导致空闲中断失效
*/
uart_stru UART0_;
void UART0_INIT(u32 baud)
{
	UART_InitTypeDef uart0_config;
	DMA_InitTypeDef uart0_dma_config;
	UART_StructInit(&uart0_config);
	
	
//	ALL_GPIO(GPIO0,GPIO_Pin_2,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	// TX
//	GPIO_PinAFConfig(GPIO0, GPIO_PinSource_1, GPIO_AF_UART);
//	ALL_GPIO(GPIO0,GPIO_Pin_1,GPIO_Mode_IN,GPIO_PuPd_NOPULL);	// RX
//	GPIO_PinAFConfig(GPIO0, GPIO_PinSource_2, GPIO_AF_UART);
	
	// 使用CAN时用04 05
	ALL_GPIO(GPIO0,GPIO_Pin_5,GPIO_Mode_OUT,GPIO_PuPd_NOPULL);	// TX
	GPIO_PinAFConfig(GPIO0, GPIO_PinSource_5, GPIO_AF_UART);
	ALL_GPIO(GPIO0,GPIO_Pin_4,GPIO_Mode_IN,GPIO_PuPd_NOPULL);	// RX
	GPIO_PinAFConfig(GPIO0, GPIO_PinSource_4, GPIO_AF_UART);
	
	
	uart0_config.BAUDRATE     = baud;				// 波特率
	// 这些是默认配置，简单的应用只配置波特率即可
	uart0_config.DUPLEX       = DISABLE;  	// 半双工模式使能，tx_data
	uart0_config.MD_EN        = DISABLE;  	// Multi-drop 使能
	uart0_config.CK_EN        = DISABLE;  	// 数据校验使能
	uart0_config.CK_TYPE      = 0;        	// 奇偶校验配置       0:偶校验（EVEN）;1: 奇校验（ODD）
	uart0_config.BIT_ORDER    = 0;        	// 数据发送顺序配置   0:LSB;1:MSB
	uart0_config.STOP_LEN     = 0;        	// 停止位长度配置     0:1-Bit;1:2-Bit
	uart0_config.BYTE_LEN     = 0;        	// 数据长度配置       0:8-Bit;1:9-Bit
	uart0_config.ADR          = 0;        	// 多机通讯时的从机地址
	uart0_config.TX_BUF_EMPTY = DISABLE;  	// 发送缓冲区空 DMA 请求使能
	uart0_config.RX_DONE      = ENABLE;   	// 接收完成 DMA 请求使能
	uart0_config.TX_DONE      = ENABLE;   	// 发送完成 DMA 请求使能
	uart0_config.TXD_INV      = DISABLE;  	// TXD 输出极性取反
	uart0_config.RXD_INV      = DISABLE;  	// RXD 输入极性取反
	uart0_config.IE           = BIT8+UART_IRQEna_SendOver; // 中断配置 发送完成和 接收空闲
	UART_Init(UART0, &uart0_config);

	DMA_StructInit(&uart0_dma_config); 
	uart0_dma_config.DMA_Channel_EN = ENABLE;  			/* DMA 通道使能*/
	uart0_dma_config.DMA_IRQ_EN     = DISABLE; 			/* DMA 中断使能 */
	uart0_dma_config.DMA_RMODE      = ENABLE;  			/* 多轮传输使能 */
	uart0_dma_config.DMA_CIRC       = DISABLE; 			/* 循环模式使能 */
	uart0_dma_config.DMA_SINC       = DISABLE;  		/* 源地址递增使能 */
	uart0_dma_config.DMA_DINC       = ENABLE; 			/* 目的地址递增使能 */
	uart0_dma_config.DMA_SBTW       = 0;				/* 源地址访问位宽， 0:byte, 1:half-word, 2:word */
	uart0_dma_config.DMA_DBTW       = 0;				/* 目的地址访问位宽， 0:byte, 1:half-word, 2:word */
	uart0_dma_config.DMA_REQ_EN     = DMA_REQ_UART0_RX;	/* 通道 x 硬件 DMA 请求使能，高有效 */
	uart0_dma_config.DMA_TIMES      = RX_SIZE;				/* DMA 通道 x 数据搬运次数 */	//发送时会重新更改发送次数
	uart0_dma_config.DMA_SADR       = (u32)&UART0_BUFF;	/* DMA 通道 x 源地址 */
	uart0_dma_config.DMA_DADR       = (u32)&UART0_.RX_BUFF;	/* DMA 通道 x 目的地址 */
	DMA_Init(DMA_CH1, &uart0_dma_config);
	DMA_Trigger(DMA_CH1);	//触发一次
	
	
	DMA_StructInit(&uart0_dma_config); 
	uart0_dma_config.DMA_Channel_EN = ENABLE;  			/* DMA 通道使能*/
	uart0_dma_config.DMA_IRQ_EN     = DISABLE; 			/* DMA 中断使能 */
	uart0_dma_config.DMA_RMODE      = ENABLE;  			/* 多轮传输使能 */
	uart0_dma_config.DMA_CIRC       = DISABLE; 			/* 循环模式使能 */
	uart0_dma_config.DMA_SINC       = ENABLE;  			/* 源地址递增使能 */
	uart0_dma_config.DMA_DINC       = DISABLE; 			/* 目的地址递增使能 */
	uart0_dma_config.DMA_SBTW       = 0;				/* 源地址访问位宽， 0:byte, 1:half-word, 2:word */
	uart0_dma_config.DMA_DBTW       = 0;				/* 目的地址访问位宽， 0:byte, 1:half-word, 2:word */
	uart0_dma_config.DMA_REQ_EN     = DMA_REQ_UART0_TX;	/* 通道 x 硬件 DMA 请求使能，高有效 */
	uart0_dma_config.DMA_TIMES      = 1;				/* DMA 通道 x 数据搬运次数 */	//发送时会重新更改发送次数
	uart0_dma_config.DMA_SADR       = (u32)&UART0_.TX_BUFF;	/* DMA 通道 x 源地址 */
	uart0_dma_config.DMA_DADR       = (u32)&UART0_BUFF;	/* DMA 通道 x 目的地址 */
	DMA_Init(DMA_CH0, &uart0_dma_config);
	UART0_.T_c=1;
	DMA_Trigger(DMA_CH0);	//触发一次

	NVIC_EnableIRQ(UART0_IRQn);			//开启中断
	NVIC_SetPriority(UART0_IRQn, 2);	//优先级
}
/*
UART0发送函数
写入发送数组地址和发送长度
作者:杨光宇 2025-7-23
UART0_.TX_BUFF[0]='a';
UART0_SEND(UART0_.TX_BUFF,10);

//vofa+发送格式
	UART0_.floatnumber[0]=3.14f;
	UART0_.floatnumber[1]=4.14f;
	UART0_.floatnumber[2]=5.14f;
	UART0_.floatnumber[3]=6.14f;
	UART0_.floatnumber[4]=7.14f;
	memcpy(UART0_.TX_BUFF, (uint8_t *)&UART0_.floatnumber, 4*5);	//复制(float类型)内存到发送数组
	UART0_.TX_BUFF[4*5+2]=0x80;
	UART0_.TX_BUFF[4*5+3]=0x7f;
	UART0_SEND(UART0_.TX_BUFF,4*5+4);
*/
void UART0_SEND(char *t,uint16_t len)
{
	if(UART0_.T_c)//轮询,若没有发送完成将不会进行下一步
	{
		UART0_.T_c=0;	//标志复位
		DMA_InitTypeDef uart0_dma_config;
		uart0_dma_config.DMA_Channel_EN = ENABLE;           /* DMA 通道使能*/
		uart0_dma_config.DMA_IRQ_EN     = DISABLE;          /* DMA 中断使能 */
		uart0_dma_config.DMA_RMODE      = ENABLE;           /* 多轮传输使能 */
		uart0_dma_config.DMA_CIRC       = DISABLE;          /* 循环模式使能 */
		uart0_dma_config.DMA_SINC       = ENABLE;           /* 源地址递增使能 */
		uart0_dma_config.DMA_DINC       = DISABLE;          /* 目的地址递增使能 */
		uart0_dma_config.DMA_SBTW       = 0;                /* 源地址访问位宽， 0:byte, 1:half-word, 2:word */
		uart0_dma_config.DMA_DBTW       = 0;                /* 目的地址访问位宽， 0:byte, 1:half-word, 2:word */
		uart0_dma_config.DMA_REQ_EN     = DMA_REQ_UART0_TX; /* 通道 x 硬件 DMA 请求使能，高有效 */
		uart0_dma_config.DMA_TIMES      = len;              /* DMA 通道 x 数据搬运次数 */
		uart0_dma_config.DMA_SADR       = (uint32_t)t;           /* DMA 通道 x 源地址 */
		uart0_dma_config.DMA_DADR       = (uint32_t)&UART0_BUFF; /* DMA 通道 x 目的地址 */
		DMA_Init(DMA_CH0, &uart0_dma_config);
		DMA_Trigger(DMA_CH0);
	}
}
/*
UART0发送函数
格式化发送数据
作者:杨光宇 2025-8-4
*/
void UART0_PRINT(char *stringg, ...)//格式化发送
{
	if(UART0_.T_c)//轮询,若没有发送完成将不会进行下一步
	{
		static char string[TX_SIZE];
		unsigned short i1;
		va_list arg;
		va_start(arg,stringg);
		vsprintf(string,stringg,arg);
		va_end(arg);
		i1=0;
		while(string[i1]!=0)
			i1++;//得出string的长度
		UART0_SEND(string,i1);
	}
}

/*
定时器单元配置
UTIMER0->CNT	是计数值 这里没有分频 自增频率为192Mhz
延时操作:
举例:-------------------------------------------------------------
TIMer_base(UTIMER0,65536,65536);
UTIMER0->CFG|=1<<31;//开启循环计数
UTIMER0->CFG|=1<<25;//开启单次计数
UTIMER0->IE|=UTIMER_IE_ZERO;//开启溢出中断

UTIMER0->CFG|=1<<31;//开启循环计数
UTIMER0->CNT=0;	//复位计数值
while((UTIMER0->CNT/192)<100);	//100us延时

	TIMer_base(UTIMER2,192000,192000);	//配置空闲定时器 0.001s
	UTIMER2->IE=UTIMER_IRQEna_Zero;		//过零中断
	NVIC_EnableIRQ(TIMER2_IRQn);		//开启中断
	NVIC_SetPriority(TIMER2_IRQn, 4);	//优先级

注意:	UTIMER0\UTIMER1\UTIMER4是16位计数器
		UTIMER2\UTIMER3是32位计数器
对于16位计数器,最大计时时间为:0.00034133333 s	4800hz周期:0.000208333 s	可以用于UART的空闲判断
对于32位计数器,最大计时时间为:22.36962133 s		
中断函数:
void TIMER1_IRQHandler(void)
{
	if(UTIMER1_IF & UTIMER_IE_CH0)	//比较值中断
	{
		UTIMER1_IF = UTIMER_IE_CH0;  // 清除UTimer中断标志位
	}
	if(UTIMER1_IF & UTIMER_IE_ZERO)	//过零中断
	{
		UTIMER1_IF = UTIMER_IE_ZERO;  // 清除UTimer中断标志位
	}
}
*/
void TIMer_base(UTIMER_TypeDef *UTIMERx,u32 TH,u32 COMP)
{
	UTIMER_InitTypeDef TIM_InitStruct;
    UTIMER_StructInit(&TIM_InitStruct); /* Timer结构体初始化*/
    TIM_InitStruct.EN       = ENABLE;                  	// Timer 模块整体使能，高有效
	TIM_InitStruct.ETON     = 0;                       		// Timer 计数器计数使能配置 0:自动运行 1:等待外部事件触发计数
    TIM_InitStruct.CLK_DIV  = UTIMER_Clk_Div1;         		// Timer 计数器分频设置
    TIM_InitStruct.CLK_SRC  = UTIMER_CLK_SRC_MCLK;     		// Timer 时钟源
    TIM_InitStruct.CH0_POL  = 0;                       		// Timer通道1在比较模式下的输出极性控制，当计数器计数值回零时的输出值
    TIM_InitStruct.CH0_MODE = UTIMER_MODE_CMP;         		// Timer通道1的工作模式选择，默认值为0
    TIM_InitStruct.TH       = TH-1;                       	// Timer 计数器计数门限。
    TIM_InitStruct.CMP0     = COMP-1;                    	// Timer 通道0工作在比较模式时，当计数器计数值等于CMP0时，发生比较事件。
    TIM_InitStruct.FLT      = 20;                          	// 通道0/1信号滤波宽度选择。取值范围0~255
//    TIM_InitStruct.IE       = UTIMER_IE_CH0 | UTIMER_IE_ZERO;	/* 开启Timer模块比较中断*/
    UTIMER_Init(UTIMERx,&TIM_InitStruct);
}

/**
初始化两个spi
* @brief 初始化SPI接口（SPI0和SPI1），SPI0:  SPI1:
 * @note 配置为全双工主模式，支持8位数据传输，时钟极性和相位适配常见外设
 */
void spi0_init(u32 speed_bps)
{
	SPI_InitTypeDef spiconfig;
//	GPIO_InitTypeDef GPIO_InitStruct;
//	// 配置P1.11	SPI0_CS 
//	GPIO_StructInit(&GPIO_InitStruct);         // 初始化结构体
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; // GPIO输出模式
//	GPIO_InitStruct.GPIO_Pin  = SPI0_CS_Pin;
//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(SPI0_CS_GPIO, &GPIO_InitStruct);
	
	GPIO_Config(SPI0_MOSI_GPIO,SPI0_MOSI_Source, GPIO_Mode_OUT, GPIO_AF_SPI);  // SPI0_MOSI
	GPIO_Config(SPI0_MISO_GPIO,SPI0_MISO_Source, GPIO_Mode_IN,  GPIO_AF_SPI);  // SPI0_MISO
	GPIO_Config(SPI0_SCLK_GPIO,SPI0_SCLK_Source, GPIO_Mode_OUT, GPIO_AF_SPI);  // SPI0_SCK
	GPIO_Config(SPI0_CS_GPIO,SPI0_CS_Source, GPIO_Mode_OUT, GPIO_AF_SPI);  // SPI0_SCK
	SPI_StructInit(&spiconfig);
	spiconfig.EN         = 1; //  SPI模块使能：0，关闭；1，开启
	spiconfig.CPHA       = 1; //  相位选择：0对应0；1对应1
	spiconfig.CPOL       = 1; //  极性选择：0对应0；1对应1
	spiconfig.ByteLength = 16; //  SPI传输数据长度 8 - 16 有效
	spiconfig.BaudRate   = 3; //  波特率设置 注意，这里的波特率指的是用户手册里的SPI传输波特率配置寄存器，SPI 实际传输速度计算公式为：SPI 传输速度 = 系统时钟 / (2*(BAUD + 1))
	spiconfig.DataOrder  = 0; //  传输顺序：0，高位先传；1，低位先传
	spiconfig.IRQEna     = 1; //  SPI中断使能：0，关闭；1，开启------------------
	spiconfig.Mode       = 1; //  主从模式选择：0，从模式；1，主模式
	spiconfig.Duplex     = 0; //  全双工、半双工工作模式选择：0，全双工；2，半双工仅发送；3，半双工仅接收
	spiconfig.CS         = 0; //  从设备下片选信号来源    0，恒有效    1，来源于主设备
	spiconfig.Trig       = 0; //  传输触发选择：0，内部自动执行（仅主模式有效）；1，外部触发
	spiconfig.TRANS_MODE = 1; //  SPI数据搬运方式：0，DMA搬运；1，MCU搬运
	SPI_Init(SPI0, &spiconfig);
	SPI_SetBaudRate(SPI0, speed_bps);	//速度
	SPI0_IE|=SPI_IE_TranDone|SPI_IE_Enable;					//中断配置寄存器
	NVIC_EnableIRQ(SPI0_IRQn);      // 开启中断
	NVIC_SetPriority(SPI0_IRQn, 1); // 优先级
	
	
//	DMA_Enable();
//    {volatile u8 a[32]; // 需要被DMA搬移的数据需要加volatile
//        DMA_InitTypeDef DMAInitStruct;
//        DMAInitStruct.DMA_Channel_EN = ENABLE;    /* DMA 通道使能*/
//        DMAInitStruct.DMA_IRQ_EN     = 0;         /* DMA 中断使能 */
//        DMAInitStruct.DMA_RMODE      = ENABLE;   /* 多轮传输使能 */
//        DMAInitStruct.DMA_CIRC       = DISABLE;   /* 循环模式使能 */
//        DMAInitStruct.DMA_SINC       = ENABLE;    /* 源地址递增使能 */
//        DMAInitStruct.DMA_DINC       = DISABLE;    /* 目的地址递增使能 */
//        DMAInitStruct.DMA_SBTW       = 0;         /* 源地址访问位宽， 0:byte, 1:half-word, 2:word */
//        DMAInitStruct.DMA_DBTW       = 0;         /* 目的地址访问位宽， 0:byte, 1:half-word, 2:word */
//        DMAInitStruct.DMA_REQ_EN     = DMA_REQ_SPI0_TX;         /* 通道 x 硬件 DMA 请求使能，高有效 */
//        DMAInitStruct.DMA_TIMES      = sizeof(a); /* DMA 通道 x 数据搬运次数 */
//        DMAInitStruct.DMA_SADR       = (u32)a;    /* DMA 通道 x 源地址 */
//        DMAInitStruct.DMA_DADR       = (u32)&SPI0_TX_DATA;    /* DMA 通道 x 目的地址 */
//        DMA_Init(DMA_CH0, &DMAInitStruct);
//    }
}
void spi1_init(u32 speed_bps)
{
	SPI_InitTypeDef spiconfig;
	GPIO_InitTypeDef GPIO_InitStruct;
	// 配置P0.14	SPI1_CS 
	GPIO_StructInit(&GPIO_InitStruct);         // 初始化结构体
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; // GPIO输出模式
	GPIO_InitStruct.GPIO_Pin  = SPI1_CS_Pin;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SPI1_CS_GPIO, &GPIO_InitStruct);  
	GPIO_Config(SPI1_MOSI_GPIO,SPI1_MOSI_Source, GPIO_Mode_OUT, GPIO_AF_SPI);  // SPI1_MOSI
	GPIO_Config(SPI1_MISO_GPIO,SPI1_MISO_Source, GPIO_Mode_IN,  GPIO_AF_SPI);  // SPI1_MISO
	GPIO_Config(SPI1_SCLK_GPIO,SPI1_SCLK_Source, GPIO_Mode_OUT, GPIO_AF_SPI);  // SPI1_SCK
	SPI_StructInit(&spiconfig);
	spiconfig.EN         = 1; //  SPI模块使能：0，关闭；1，开启
	spiconfig.CPHA       = 1; //  相位选择：0对应0；1对应1
	spiconfig.CPOL       = 1; //  极性选择：0对应0；1对应1
	spiconfig.ByteLength = 8; //  SPI传输数据长度 8 - 16 有效
	spiconfig.BaudRate   = 3; //  波特率设置 注意，这里的波特率指的是用户手册里的SPI传输波特率配置寄存器，SPI 实际传输速度计算公式为：SPI 传输速度 = 系统时钟 / (2*(BAUD + 1))
	spiconfig.DataOrder  = 0; //  传输顺序：0，高位先传；1，低位先传
	spiconfig.IRQEna     = 1; //  SPI中断使能：0，关闭；1，开启
	spiconfig.Mode       = 1; //  主从模式选择：0，从模式；1，主模式
	spiconfig.Duplex     = 0; //  全双工、半双工工作模式选择：0，全双工；2，半双工仅发送；3，半双工仅接收
	spiconfig.CS         = 0; //  从设备下片选信号来源    0，恒有效    1，来源于主设备
	spiconfig.Trig       = 0; //  传输触发选择：0，内部自动执行（仅主模式有效）；1，外部触发
	spiconfig.TRANS_MODE = 1; //  SPI数据搬运方式：0，DMA搬运；1，MCU搬运
	SPI_Init(SPI1, &spiconfig);
	SPI_SetBaudRate(SPI1, speed_bps);	
}
/**
 * @brief 通过SPI接口发送并接收一个字节（全双工通信）
 * @param SPIx: SPI模块指针（如 SPI2 、SPI3，需已初始化）
 * @param byte: 待发送的8位数据
 * @return uint8_t: 从SPI接口接收到的8位数据
 * @note 等待发送缓冲区空后发送数据，再等待接收缓冲区非空后读取数据
 *       适用于SPI主模式下与从设备的同步通信（如读取编码器数据）
 */
uint8_t MC_MCU_SPIxSendByte(SPI_TypeDef *SPIx,uint8_t byte)
{
  SPI_SendData(SPIx, byte);
  while (SPI_GetFlag(SPIx, SPI_IF_TranDone) == 0);  // 等待SPI发送完成
	SPIx->IE &= 0xFFFF; // 写1清除SPI_IF_TranDone
  return SPI_ReadData(SPIx);
}

/*
配置3相互补PWM
死区时间需要用示波器看
输入 pwm 的比较最大值 时钟为192 000 000 输出频率为; 192000000/2/R
*/
void FOC_PWM(u32 R)
{
	MCPWM_InitTypeDef MCPWM_InitStructure;
	MCPWM_StructInit(&MCPWM_InitStructure);
	// GPIO_P1.2、P1.4、P1.6设置为输出模式，MCPWM复用
	GPIO_Config(GPIO1, GPIO_PinSource_2, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //A相IN
	GPIO_Config(GPIO1, GPIO_PinSource_4, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //B相IN
	GPIO_Config(GPIO1, GPIO_PinSource_6, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //C相IN
	
	GPIO_Config(GPIO2, GPIO_PinSource_12, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //A相IN
	GPIO_Config(GPIO2, GPIO_PinSource_13, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //B相IN
	GPIO_Config(GPIO2, GPIO_PinSource_14, GPIO_Mode_OUT, GPIO_AF_MCPWM);  //C相IN

	MCPWM_InitStructure.TimeBase0_PERIOD       = R-1;     					/* 时期0周期设置 20KHZ*/
	MCPWM_InitStructure.TimeBase1_PERIOD       = 0;                      	/* 时期1周期设置*/
	MCPWM_InitStructure.CLK_DIV                = 0;                      	/* MCPWM 分频系数 */
	MCPWM_InitStructure.MCLK_EN                = ENABLE;                 	/* MCPWM 时钟使能开关 */
	MCPWM_InitStructure.TMR2_TimeBase_Sel      = 0;                      	/* TMR2 比较门限寄存器 时基选择 0:时基0 | 1:时基1 */
	MCPWM_InitStructure.TMR3_TimeBase_Sel      = 0;                      	/* TMR3 比较门限寄存器 时基选择 0:时基0 | 1:时基1 */
	MCPWM_InitStructure.MCPWM_Cnt0_EN          = ENABLE;                 	/* MCPWM 时基0主计数器使能开关 */
	MCPWM_InitStructure.TimeBase0_Trig_Enable  = 0;                      	/* 时基0 外部触发使能 */
	MCPWM_InitStructure.TimeBase0Init_CNT      = 0;                      	/* 时基0 计数器初始值 */
	MCPWM_InitStructure.TimeBase_TrigEvt0      = 0;                      	/* 时基0 外部触发事件选择 */
	MCPWM_InitStructure.MCPWM_Cnt1_EN          = 0;                      	/* MCPWM 时基1主计数器使能开关 */
	MCPWM_InitStructure.TimeBase1_Trig_Enable  = 0;                      	/* 时基1 外部触发使能*/
	MCPWM_InitStructure.TimeBase1Init_CNT      = 0;                      	/* 时基1 计数器初始值 */
	MCPWM_InitStructure.TimeBase_TrigEvt1      = 0;                      	/* 时基1 外部触发事件选择 */
	MCPWM_InitStructure.MCPWM_WorkModeCH0      = MCPWM_CENTRAL_PWM_MODE; 	/* MCPWM CH0工作模式：边沿对齐/中心对齐 */
	MCPWM_InitStructure.MCPWM_WorkModeCH1      = MCPWM_CENTRAL_PWM_MODE; 	/* MCPWM CH0工作模式：边沿对齐/中心对齐 */
	MCPWM_InitStructure.MCPWM_WorkModeCH2      = MCPWM_CENTRAL_PWM_MODE; 	/* MCPWM CH0工作模式：边沿对齐/中心对齐 */
	MCPWM_InitStructure.MCPWM_WorkModeCH3      = MCPWM_CENTRAL_PWM_MODE; 	/* MCPWM CH0工作模式：边沿对齐/中心对齐 */
	MCPWM_InitStructure.TriggerPoint0          = 0;                		/* PWM触发ADC事件0，时间点设置 */
	MCPWM_InitStructure.TriggerPoint1          = 0;                		/* PWM触发ADC事件1，时间点设置 */
	MCPWM_InitStructure.TriggerPoint2          = 0;                		/* PWM触发ADC事件2，时间点设置 */
	MCPWM_InitStructure.TriggerPoint3          = 0;                		/* PWM触发ADC事件3，时间点设置 */
	MCPWM_InitStructure.DeadTimeCH0N           = 50;              		// CH0N死区时间设置　	1为5.1ns
	MCPWM_InitStructure.DeadTimeCH0P           = 50;              		// CH0P死区时间设置　	10为51ns
	MCPWM_InitStructure.DeadTimeCH1N           = 50;              		// CH1N死区时间设置　	100为700ns
	MCPWM_InitStructure.DeadTimeCH1P           = 50;              		// CH1P死区时间设置　	50大约是434ns
	MCPWM_InitStructure.DeadTimeCH2N           = 50;              		// CH2N死区时间设置　	80 590ns
	MCPWM_InitStructure.DeadTimeCH2P           = 50;              		// CH2P死区时间设置　	
	MCPWM_InitStructure.DeadTimeCH3N           = 50;              		// CH3N死区时间设置　
	MCPWM_InitStructure.DeadTimeCH3P           = 50;              		// CH3P死区时间设置　
	MCPWM_InitStructure.CH0N_Polarity_INV      = 0;                		/* CH0N输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.CH0P_Polarity_INV      = 0;                		/* CH0P输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.CH1N_Polarity_INV      = 0;                		/* CH1N输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.CH1P_Polarity_INV      = 0;                		/* CH1P输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.CH2N_Polarity_INV      = 0;                		/* CH2N输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.CH2P_Polarity_INV      = 0;                		/* CH2P输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.CH3N_Polarity_INV      = 0;                		/* CH3N输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.CH3P_Polarity_INV      = 0;						/* CH3P输出极性取反，0:正常输出；1:取反输出 */
	MCPWM_InitStructure.Switch_CH0N_CH0P       = ENABLE;				/* 交换CH0N, CH0P信号输出使能开关 */
	MCPWM_InitStructure.Switch_CH1N_CH1P       = ENABLE;				/* 交换CH1N, CH1P信号输出使能开关 */
	MCPWM_InitStructure.Switch_CH2N_CH2P       = ENABLE;				/* 交换CH2N, CH2P信号输出使能开关 */
	MCPWM_InitStructure.Switch_CH3N_CH3P       = DISABLE;            	/* 交换CH3N, CH3P信号输出使能开关 */
	MCPWM_InitStructure.MCPWM_UpdateInterval   = 0;                  	/* MCPWM T0/T1事件更新间隔 */
	MCPWM_InitStructure.MCPWM_Base0T0_UpdateEN = ENABLE;             	/* MCPWM 时基0 T0事件更新使能 */
	MCPWM_InitStructure.MCPWM_Base0T1_UpdateEN = DISABLE;            	/* MCPWM 时基0 T1事件更新使能 */
	MCPWM_InitStructure.MCPWM_Base1T0_UpdateEN = DISABLE;            	/* MCPWM 时基1 T0事件更新使能 */
	MCPWM_InitStructure.MCPWM_Base1T1_UpdateEN = DISABLE;            	/* MCPWM 时基1 T1事件更新使能 */
	MCPWM_InitStructure.MCPWM_Auto_ERR_EN      = DISABLE;            	/* MCPWM 更新事件是否自动打开MOE, 使能开关 */
	MCPWM_InitStructure.DebugMode_PWM_out      = ENABLE;             	/* Debug时，MCU进入Halt, MCPWM信号是否正常输出 */
	MCPWM_InitStructure.GPIO_BKIN_Filter       = 10;                 	/* GPIO输入滤波时钟设置1-16 */
	MCPWM_InitStructure.CMP_BKIN_Filter        = 10;                 	/* 比较器CMP输入滤波时钟设置1-16 */
	MCPWM_InitStructure.FAIL0_INPUT_EN   = DISABLE; 										 /* FAIL0 输入功能使能,FAIL0比较器信号来源是CMP0 */
	MCPWM_InitStructure.FAIL0_INT_EN     = DISABLE; 										 /* FAIL0事件中断使能 */
	MCPWM_InitStructure.FAIL0_Signal_Sel = 0;       										 /* FAIL0 信号选择，来源于比较器0 */
	MCPWM_InitStructure.FAIL0_Polarity   = 0;       										 /* FAIL0 信号极性设置，高有效或低有效 */
	MCPWM_InitStructure.FAIL1_INPUT_EN   = DISABLE;  										 /* FAIL1 输入功能使能,FAIL1比较器信号来源是CMP1 */
	MCPWM_InitStructure.FAIL1_INT_EN     = DISABLE; 										 /* FAIL1事件中断使能 */
	MCPWM_InitStructure.FAIL1_Signal_Sel = 1;       										 /* FAIL1 信号选择，比较器或GPIO  1：比较器 */    
	MCPWM_InitStructure.FAIL1_Polarity   = 1;       										 /* FAIL1 信号极性设置，高有效 */
	MCPWM_InitStructure.FAIL2_INPUT_EN   = DISABLE; 										 /* FAIL2 输入功能使能 */
	MCPWM_InitStructure.FAIL2_INT_EN     = DISABLE; 										 /* FAIL2事件中断使能 */
	MCPWM_InitStructure.FAIL2_Signal_Sel = 0;       										 /* FAIL2 信号选择，比较器0或GPIO */
	MCPWM_InitStructure.FAIL2_Polarity   = 0;       										 /* FAIL2 信号极性设置，高有效或低有效 */
	MCPWM_InitStructure.FAIL3_INPUT_EN   = DISABLE; 										 /* FAIL3 输入功能使能 */
	MCPWM_InitStructure.FAIL3_INT_EN     = DISABLE; 										 /* FAIL3事件中断使能 */
	MCPWM_InitStructure.FAIL3_Signal_Sel = 0;       										 /* FAIL3 信号选择，比较器0或GPIO */
	MCPWM_InitStructure.FAIL3_Polarity   = 0;       										 /* FAIL3 信号极性设置，高有效或低有效 */
	MCPWM_InitStructure.CH0P_default_output = 0; 												 /* CH0P MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.CH0N_default_output = 0; 												 /* CH0N MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.CH1P_default_output = 0; 												 /* CH1P MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.CH1N_default_output = 0; 												 /* CH1N MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.CH2P_default_output = 0; 												 /* CH2P MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.CH2N_default_output = 0; 												 /* CH2N MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.CH3P_default_output = 0; 												 /* CH3P MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.CH3N_default_output = 0; 												 /* CH3N MOE为0时或发生FAIL事件时，默认电平输出 */
	MCPWM_InitStructure.T0_Update_INT_EN_BASE0  = 0;                     /* T0更新事件中斷使能 */
	MCPWM_InitStructure.T1_Update_INT_EN_BASE0  = 1;                     /* T1更新事件中斷使能 */
	MCPWM_InitStructure.TMR0_Match_INT_EN_BASE0 = 0;                     /* TMR0计数事件匹配事件中断使能 */
	MCPWM_InitStructure.TMR1_Match_INT_EN_BASE0 = 0;                     /* TMR1计数事件匹配事件中断使能 */ //MCPWM_IF_T1
	MCPWM_InitStructure.AUEN              = 		MCPWM_AUEN_TH00 |
													MCPWM_AUEN_TH01 |
													MCPWM_AUEN_TH10 |
													MCPWM_AUEN_TH11 |
													MCPWM_AUEN_TH20 |
													MCPWM_AUEN_TH21;         /* 自动更新使能寄存器 */
	MCPWM_Init(MCPWM0, &MCPWM_InitStructure);
	MCPWM0->TH00 =0;
	MCPWM0->TH01 =0;
	MCPWM0->TH10 =0;
	MCPWM0->TH11 =0;
	MCPWM0->TH20 =0;
	MCPWM0->TH21 =0;
	MCPWM_OutputMode(MCPWM0, MCPWM_OUT_CHN_012, MCPWM_OUT_MODE_PWM);
	NVIC_EnableIRQ(MCPWM00_IRQn);      // 开启中断
	NVIC_SetPriority(MCPWM00_IRQn, 1); // 优先级
	MCPWM_Enable(MCPWM0);	//pwm使能
}

/*
改变3相PWM的占空比
*/
#include "foc.h"
void ABC_OUT_PWM(int A,int B,int C)
{
	MCPWM0->TH00 = A-PWMCCR;
	MCPWM0->TH01 = PWMCCR-A;
	MCPWM0->TH10 = B-PWMCCR;
	MCPWM0->TH11 = PWMCCR-B;
	MCPWM0->TH20 = C-PWMCCR;
	MCPWM0->TH21 = PWMCCR-C;
}
/*
使用了两个ADC 1 2
ADC1用于电压采样和温度采样
ADC2用于电机电流采样
中断:
void ADC2_IRQHandler(void)
{
	if (ADC2_IF & BIT0)                     // 判断是否发生第一采样完成中断
	{
		ADC2_IF = BIT0;
	}
}
*/




void ADC_OPMA_INIT(void)
{
	ADC_InitTypeDef ADC_InitStruct;

	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.IE    = DISABLE;                // ADC中断使能
	ADC_InitStruct.RE    = DISABLE;               // ADC触发DMA使能
	ADC_InitStruct.Align = ADC_ALIGN_LEFT;        // 采样数据对齐方式
	ADC_InitStruct.GAIN  = ADC_GAIN_HIGH_DAT0;    // 通道增益 高增益(1倍,满量程电压对应2.2V)
	ADC_InitStruct.TROVS = ADC_TROVS_ONES;        // 过采样触发模式
	ADC_InitStruct.OVSR  = ADC_OVSR_1;            // 过采率，过采样可以提高信噪比
	ADC_InitStruct.CSMP  = ADC_CSMP_DISABLE;      // 连续采样模式
	ADC_InitStruct.TCNT  = ADC_TCNT_1;            // 触发次数
	ADC_InitStruct.S1    = ADC_S1_2;              // 第一段采样的通道数
	ADC_InitStruct.S2    = ADC_S2_1;              // 第二段采样总通道数，两段采样通道数之和最大为16
	ADC_InitStruct.NSMP  = ADC_NSMP_1;            // ADC 触发模式 单段或双段
	ADC_InitStruct.TRIG  = ADC_TRIG_MCPWM0_T0_EN; // MCPWM0 T0事件触发ADC采样使能，高有效
	ADC_InitStruct.GEN0  = ADC_GEN0_NONE;         // 看门狗 0 通道使能
	ADC_InitStruct.HTH0  = 0;                     // 看门狗 0 上阈值
	ADC_InitStruct.LTH0  = 0;                     // 看门狗 0 下阈值
	ADC_InitStruct.GEN1  = ADC_GEN1_NONE;         // 看门狗 1 通道使能
	ADC_InitStruct.HTH1  = 0;                     // 看门狗 1 上阈值
	ADC_InitStruct.LTH1  = 0;                     // 看门狗 1 下阈值
	ADC_Init(ADC0, &ADC_InitStruct);
	ADC_SetPChanne(ADC0, ADC_DAT_1, ADC_CHN_9);		// W相电压
	ALL_GPIO(GPIO4,GPIO_Pin_14,GPIO_Mode_ANA,GPIO_PuPd_NOPULL);	// W相电压

	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.IE    = DISABLE;                // ADC中断使能
	ADC_InitStruct.RE    = DISABLE;               // ADC触发DMA使能
	ADC_InitStruct.Align = ADC_ALIGN_LEFT;        // 采样数据对齐方式
	ADC_InitStruct.GAIN  = ADC_GAIN_HIGH_DAT0;    // 通道增益 高增益(1倍,满量程电压对应2.2V)
	ADC_InitStruct.TROVS = ADC_TROVS_ONES;        // 过采样触发模式
	ADC_InitStruct.OVSR  = ADC_OVSR_1;            // 过采率，过采样可以提高信噪比
	ADC_InitStruct.CSMP  = ADC_CSMP_DISABLE;      // 连续采样模式
	ADC_InitStruct.TCNT  = ADC_TCNT_1;            // 触发次数
	ADC_InitStruct.S1    = ADC_S1_5;              // 第一段采样的通道数
	ADC_InitStruct.S2    = ADC_S2_1;              // 第二段采样总通道数，两段采样通道数之和最大为16
	ADC_InitStruct.NSMP  = ADC_NSMP_1;            // ADC 触发模式 单段或双段
	ADC_InitStruct.TRIG  = ADC_TRIG_MCPWM0_T0_EN; // MCPWM0 T0事件触发ADC采样使能，高有效
	ADC_InitStruct.GEN0  = ADC_GEN0_NONE;         // 看门狗 0 通道使能
	ADC_InitStruct.HTH0  = 0;                     // 看门狗 0 上阈值
	ADC_InitStruct.LTH0  = 0;                     // 看门狗 0 下阈值
	ADC_InitStruct.GEN1  = ADC_GEN1_NONE;         // 看门狗 1 通道使能
	ADC_InitStruct.HTH1  = 0;                     // 看门狗 1 上阈值
	ADC_InitStruct.LTH1  = 0;                     // 看门狗 1 下阈值
	ADC_Init(ADC1, &ADC_InitStruct);
	ADC_SetPChanne(ADC1, ADC_DAT_1, ADC_CHN_7);   // V相电压
	ALL_GPIO(GPIO4,GPIO_Pin_7,GPIO_Mode_ANA,GPIO_PuPd_NOPULL);	// V相电压
	ADC_SetPChanne(ADC1, ADC_DAT_2, ADC_CHN_8);   // U相电压
	ALL_GPIO(GPIO4,GPIO_Pin_6,GPIO_Mode_ANA,GPIO_PuPd_NOPULL);	// U相电压
	ADC_SetPChanne(ADC1, ADC_DAT_3, ADC_CHN_12);   // NTC电压
	ALL_GPIO(GPIO3,GPIO_Pin_4,GPIO_Mode_ANA,GPIO_PuPd_NOPULL);
	ADC_SetPChanne(ADC1, ADC_DAT_4, ADC_CHN_13);   // 母线电压
	ALL_GPIO(GPIO3,GPIO_Pin_3,GPIO_Mode_ANA,GPIO_PuPd_NOPULL);
	
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.IE    = DISABLE;                // ADC中断使能
	ADC_InitStruct.RE    = DISABLE;               // ADC触发DMA使能
	ADC_InitStruct.Align = ADC_ALIGN_LEFT;        // 采样数据对齐方式
	ADC_InitStruct.GAIN  = ADC_GAIN_HIGH_DAT0;    // 通道增益 高增益(1倍,满量程电压对应2.2V)
	ADC_InitStruct.TROVS = ADC_TROVS_ONES;        // 过采样触发模式
	ADC_InitStruct.OVSR  = ADC_OVSR_1;            // 过采率，过采样可以提高信噪比
	ADC_InitStruct.CSMP  = ADC_CSMP_DISABLE;      // 连续采样模式
	ADC_InitStruct.TCNT  = ADC_TCNT_1;            // 触发次数
	ADC_InitStruct.S1    = ADC_S1_1;              // 第一段采样的通道数
	ADC_InitStruct.S2    = ADC_S2_1;              // 第二段采样总通道数，两段采样通道数之和最大为16
	ADC_InitStruct.NSMP  = ADC_NSMP_1;            // ADC 触发模式 单段或双段
	ADC_InitStruct.TRIG  = ADC_TRIG_MCPWM0_T0_EN; // MCPWM0 T0事件触发ADC采样使能，高有效
	ADC_InitStruct.GEN0  = ADC_GEN0_NONE;         // 看门狗 0 通道使能
	ADC_InitStruct.HTH0  = 0;                     // 看门狗 0 上阈值
	ADC_InitStruct.LTH0  = 0;                     // 看门狗 0 下阈值
	ADC_InitStruct.GEN1  = ADC_GEN1_NONE;         // 看门狗 1 通道使能
	ADC_InitStruct.HTH1  = 0;                     // 看门狗 1 上阈值
	ADC_InitStruct.LTH1  = 0;                     // 看门狗 1 下阈值
	ADC_Init(ADC2, &ADC_InitStruct);
	ADC_SetPChanne(ADC2, ADC_DAT_0, ADC_CHN_2);   // ADC2正端信号选择OP2通道，负端默认选择OP1负，对应A相 变成同步采样
	ADC_SetPChanne(ADC1, ADC_DAT_0, ADC_CHN_1);   // ADC1正端信号选择OP1通道，负端默认选择OP3负，对应B相 变成同步采样
	ADC_SetPChanne(ADC0, ADC_DAT_0, ADC_CHN_0);   // ADC0正端信号选择OP0通道，负端默认选择OP2负，对应C相 变成同步采样
	AFE_AdcConfig(AFE_ADC_CLK_96Mhz, AFE_ADC_REF_BGP); //ADC频率和基准选择
	ADC0_IF = 0x0f;     //清除IF标志
	ADC1_IF = 0x0f;     //清除IF标志
	ADC2_IF = 0x0f;     //清除IF标志
	ADC0_CFG |= BIT11;  //复位ADC1
	ADC1_CFG |= BIT11;  //复位ADC2
	ADC2_CFG |= BIT11;  //复位ADC3
	AFE_ModuleClockCmd(AFE_MODULE_OPA0,ENABLE);   // 使能OPA1
	AFE_ModuleClockCmd(AFE_MODULE_OPA1,ENABLE);   // 使能OPA2
	AFE_ModuleClockCmd(AFE_MODULE_OPA2,ENABLE);   // 使能OPA3
	AFE_OpaGainConfig(AFE_MODULE_OPA0,AFE_OPA_RES_40_10);   // OPA1电阻设置，40k:10k,增益大约为4倍//外部电阻0k,实际增益为(40/(10+0))
	AFE_OpaGainConfig(AFE_MODULE_OPA1,AFE_OPA_RES_40_10);   // OPA2电阻设置，40k:10k,增益大约为4倍//外部电阻0k,实际增益为(40/(10+0))
	AFE_OpaGainConfig(AFE_MODULE_OPA2,AFE_OPA_RES_40_10);   // OPA3电阻设置，40k:10k,增益大约为4倍//外部电阻0k,实际增益为(40/(10+0))
	
//	NVIC_EnableIRQ(ADC2_IRQn);      // 使能ADC2中断
//	NVIC_SetPriority(ADC2_IRQn, 2); // ADC2中断优先级配置
//	NVIC_EnableIRQ(ADC1_IRQn);      // 使能ADC2中断
//	NVIC_SetPriority(ADC1_IRQn, 2); // ADC2中断优先级配置
//	NVIC_EnableIRQ(ADC0_IRQn);      // 使能ADC2中断
//	NVIC_SetPriority(ADC0_IRQn, 2); // ADC2中断优先级配置
}
/*
低通滤波器
作者:杨光宇
时间:2025-8-18
*/
void LowPassFilter (LPF_Handle_t *LPF , float in)
{
	float temp1,temp2;
	temp1 = 1.0f / (1.0f + LPF->T_s * _2PI * LPF->F);
	temp2 = 1.0f - temp1;
	LPF->Out = temp1 * LPF->Last + temp2 * in;
	LPF->Last = LPF->Out;
}
/*
PID
作者:杨光宇
时间:2025-8-18
*/
float PID_Control (PID_Handle_t *PID_,float fdbk)
{
	PID_->errlast=PID_->err;	//保存上一次误差
	PID_->err = PID_->Ref - fdbk;	//现在误差
	PID_->Integrator += PID_->err * PID_->Ki * PID_->PID_Ts;	//矩形积分 (还有梯形积分)
	PID_->Integrator=limit(PID_->Integrator,-PID_->Integrator_Limit,PID_->Integrator_Limit);
	PID_->Result = PID_->err * PID_->Kp + PID_->Integrator+limit((PID_->err-PID_->errlast)/PID_->PID_Ts*PID_->Kd,-PID_->D_limit,PID_->D_limit);
	PID_->Result=limit(PID_->Result,-PID_->OUT_Limit,PID_->OUT_Limit);
	return PID_->Result;
}
//float PID_Control(PID_Handle_t *PID_, float fdbk)
//{
//    /* 1. 误差更新 */
//    PID_->errlast = PID_->err;
//    PID_->err     = PID_->Ref - fdbk;
//    /* 2. 积分 + 抗饱和（back-calculation） */
//    PID_->Integrator += PID_->err * PID_->Ki * PID_->PID_Ts;
//    float outPre = PID_->err * PID_->Kp + PID_->Integrator;   /* 未限幅输出 */
//    float satDif = 0.0f;
//    if (outPre >  PID_->OUT_Limit) satDif =  outPre - PID_->OUT_Limit;
//    if (outPre < -PID_->OUT_Limit) satDif =  outPre + PID_->OUT_Limit;
//    PID_->Integrator -= satDif * PID_->Ki * 0.6f;             /* 0.6f 可调 */
//    if (PID_->Integrator >  PID_->Integrator_Limit) PID_->Integrator =  PID_->Integrator_Limit;
//    if (PID_->Integrator < -PID_->Integrator_Limit) PID_->Integrator = -PID_->Integrator_Limit;
//    /* 3. 微分（对反馈差分 + 一阶惯性） */
//    static float dFilter = 0.0f;                              /* 滤波器状态 */
//    float df = (fdbk - PID_->fdbk) * PID_->Kd / PID_->PID_Ts;
//    dFilter += (df - dFilter) * 0.15f;                        /* 一阶惯性 */
//    if (dFilter >  PID_->D_limit) dFilter =  PID_->D_limit;
//    if (dFilter < -PID_->D_limit) dFilter = -PID_->D_limit;
//    PID_->fdbk = fdbk;
//    /* 4. 总输出 */
//    PID_->Result = PID_->err * PID_->Kp + PID_->Integrator - dFilter;
//    if (PID_->Result >  PID_->OUT_Limit) PID_->Result =  PID_->OUT_Limit;
//    if (PID_->Result < -PID_->OUT_Limit) PID_->Result = -PID_->OUT_Limit;
//    return PID_->Result;
//}




// -------------------------- NVR非易失存储器 --------------------------
u32	reg_moto[256];	//寄存器数组 全部保存在NVR区域中  REG_SYS_MODE
/*
寄存器参数保存函数
作者:杨光宇
时间:2025-8-24
*/
void NVR_save(void)
{
	NVR_EraseSector(ADDR0);	//ADC_ADDR 有两个扇区 一个扇区1024字节
	NVR_Program(ADDR0,reg_moto,1024);	//一个扇区
}
void NVR_get(void)
{
	for(int i=0;i<256;i++)
		reg_moto[i]=NVR_Read(ADDR0+i*4);
}
// -------------------------- NVR非易失存储器 --------------------------



// -------------------------- VOFA+JustFloat格式数据发送 --------------------------
/*
	floatnumber[0]	=1	;
	floatnumber[1]	=32	;
	floatnumber[2]	=4	;
	floatnumber[3]	=5	;
	floatnumber[4]	=6	;
	floatnumber[5]	=7	;
	floatnumber[6]	=8	;
	floatnumber[7]	=9	;
	floatnumber[8]	=10	;
	Vofa_Justfloat_send(floatnumber,9);
*/

void Vofa_Justfloat_send(float *p,u16 sum)
{
	UART0_.TX_BUFF[4*sum+2]=0x80;
	UART0_.TX_BUFF[4*sum+3]=0x7f;
	memcpy(UART0_.TX_BUFF, (uint8_t *)p, sum*4);
	UART0_SEND(UART0_.TX_BUFF,4*sum+4);
}
// -------------------------- VOFA+JustFloat格式数据发送 --------------------------
// -------------------------- NTC 33k --------------------------
float TEMP_GET(float in)//PCB温度获取
{
//	static float ZHONG_TEMP;
//	ZHONG_TEMP=in;//原始AD值
//	ZHONG_TEMP/=4095;//归一
//	ZHONG_TEMP=(ZHONG_TEMP*33000/(1-ZHONG_TEMP));//电阻值
//	return(1.0/((1.0/3950)*log(ZHONG_TEMP/33000)+(1/(25+273.15)))-273.15);
	static float ZHONG_TEMP;
	#define R 33000.0f		//电阻值 R
	ZHONG_TEMP=in;//原始AD值
	ZHONG_TEMP/=4096;	//归一
	ZHONG_TEMP*=3.3f;	//基准电压
	ZHONG_TEMP=(ZHONG_TEMP*R/(3.3f-ZHONG_TEMP));
	return(1.0f/((1.0f/3950.0f)*log(ZHONG_TEMP/R)+(1.0f/(25+273.15f)))-273.15f);
	
	/*
		log->e为底对数
		B=3950
		T1=25
		R1=33000
		A1=Temp;//原始AD值
		A1/=4095;//归一
		A1=(A1*33000/(1-A1));//电阻值
		T=(1.0/((1.0/3950)*log(A1/33000)+(1/(25+273.15)))-273.15);
	*/
}
// -------------------------- NTC 33k --------------------------
// -------------------------- CAN总线初始化 --------------------------
void CAN_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct); // 初始化结构体

    GPIO_StructInit(&GPIO_InitStruct);        // 初始化结构体
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN; // GPIO输入模式
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
    GPIO_Init(GPIO0, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);         // 初始化结构体
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; // GPIO输出模式
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
    GPIO_Init(GPIO0, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIO0, GPIO_PinSource_1, GPIO_AF_CAN);
    GPIO_PinAFConfig(GPIO0, GPIO_PinSource_2, GPIO_AF_CAN);
	
	CAN_Initl(0xB, 0x2, 0x5, 0x8); // CAN波特率1000Khz
//	ID0_Filter(0x02, 0xfff, 1);        // 开启ID0接收滤波，接收id=0x02，扩展帧
    ID1_Filter( reg_moto[REG_addr] , 0x000, 0);        // 开启ID2接收滤波，接收id= reg_moto[REG_addr] ，标准帧
//ID2_Filter( reg_moto[REG_addr] , 0xfff, 0);
	NVIC_SetPriority(CAN_IRQn, 2);     // CAN 中断优先级配置
    NVIC_EnableIRQ(CAN_IRQn);          // 使能 CAN 定时器中断
	UTIMER_DelayUs(UTIMER0,1000);
}
// -------------------------- CAN总线初始化 --------------------------
// -------------------------- 看门狗初始化 --------------------------
void IWDG_init(void)
{
    {
        IWDG_InitTypeDef IWDG_Config;
        IWDG_Config.DWK_EN = DISABLE;   // 深度休眠定时唤醒使能
        IWDG_Config.WDG_EN = ENABLE;    // 独立看门狗使能
        IWDG_Config.WTH    = 0;         // 看门狗定时唤醒时间（21位计数器，但低12恒位0）
        IWDG_Config.RTH    = (0xA555);  //  约1s,复位时间   （0x001f000/ 32K）= 3s
        IWDG_Init(&IWDG_Config);
    }
}
// -------------------------- 看门狗初始化 --------------------------
// -------------------------- 卡尔曼滤波 --------------------------
/**
 * @brief 卡尔曼滤波器初始化
 * @param kf: 滤波器结构体指针
 * @param Q: 过程噪声 (建议 0.001 ~ 0.01)
 * @param R: 测量噪声 (建议 1 ~ 5，根据你的波形噪声大小调整)
 * @param P: 初始误差 (建议 1)
 * @param X: 初始值
 */
void Kalman_Init(KalmanFilter *kf, float Q_, float R_, float P_, float X_) 
{
    kf->Q = Q_;
    kf->R_ = R_;
    kf->P = P_;
    kf->X = X_;
    kf->K = 0;
}

/**
 * @brief 卡尔曼滤波更新
 * @param kf: 滤波器结构体指针
 * @param measurement: 当前带有噪声的速度测量值
 * @return 滤波后的速度值
 */
float Kalman_Update(KalmanFilter *kf, float measurement) {
    // 1. 预测阶段 (这里假设速度变化模型很简单，即 X(k) = X(k-1))
    kf->P = kf->P + kf->Q;
    // 2. 更新阶段
    // 计算卡尔曼增益 K
    kf->K = kf->P / (kf->P + kf->R_);
    // 更新估计值 X
    kf->X = kf->X + kf->K * (measurement - kf->X);
    // 更新估计误差 P
    kf->P = (1 - kf->K) * kf->P;
    return kf->X;
}
// -------------------------- 卡尔曼滤波 --------------------------



