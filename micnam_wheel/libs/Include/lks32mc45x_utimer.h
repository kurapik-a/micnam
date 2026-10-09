/**
 * @file
 * @brief        定时器库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __lks32mc45x_TIM_H
#define __lks32mc45x_TIM_H

///< Includes ------------------------------------------------------------------

#include "basic.h"

typedef struct
{
    __IO u32 CFG;  ///< 0x00 Timer0配置寄存器
    __IO u32 TH;   ///< 0x04 Timer0计数门限寄存器
    __IO u32 CNT;  ///< 0x08 Timer0计数值寄存器
    __IO u32 CMP0; ///< 0x0C Timer0比较/捕获寄存器0
    __IO u32 CMP1; ///< 0x10 Timer0比较/捕获寄存器1
    __IO u32 EVT;  ///< 0x14 Timer0外部事件选择寄存器
    __IO u32 FLT;  ///< 0x18 Timer0滤波控制寄存器
    __IO u32 IE;   ///< 0x1C Timer0中断使能寄存器
    __IO u32 IF;   ///< 0x20 Timer0中断标志寄存器
} UTIMER_TypeDef;

#ifndef UTIMER0
#define UTIMER0 ((UTIMER_TypeDef *)TIMER0_BASE)
#endif
#ifndef UTIMER1
#define UTIMER1 ((UTIMER_TypeDef *)TIMER1_BASE)
#endif
#ifndef UTIMER2
#define UTIMER2 ((UTIMER_TypeDef *)TIMER2_BASE)
#endif
#ifndef UTIMER3
#define UTIMER3 ((UTIMER_TypeDef *)TIMER3_BASE)
#endif
#ifndef UTIMER4
#define UTIMER4 ((UTIMER_TypeDef *)TIMER4_BASE)
#endif

#define UTIMER_Clk_Div1 0x00 /*Timer工作频率128M*/
#define UTIMER_Clk_Div2 0x01 /*Timer工作频率64M*/
#define UTIMER_Clk_Div4 0x02 /*Timer工作频率32M*/
#define UTIMER_Clk_Div8 0x03 /*Timer工作频率16M*/

/*中断使能配置定义*/
typedef enum
{
    UTIMER_IRQEna_None = 0,
    UTIMER_IRQEna_CH0 = BIT0,  /*使能CH0中断，含比较、捕获中断*/
    UTIMER_IRQEna_CH1 = BIT1,  /*使能CH1中断，含比较、捕获中断*/
    UTIMER_IRQEna_Zero = BIT2, /*使能过零中断*/
    UTIMER_IRQEna_All = 0x07   /*使能Timer全部中断*/
} UTIMER_IRQEnaDef;

/*中断标志定义*/
#define UTIMER_IF_CH0 BIT0  /*Timer0 CH0中断，比较或捕获中断*/
#define UTIMER_IF_CH1 BIT1  /*Timer0 CH1中断，比较或捕获中断*/
#define UTIMER_IF_ZERO BIT2 /*Timer0 过零中断*/
/*中断标志定义*/
#define UTIMER_IE_CH0 BIT0  /*Timer0 CH0中断，比较或捕获中断*/
#define UTIMER_IE_CH1 BIT1  /*Timer0 CH1中断，比较或捕获中断*/
#define UTIMER_IE_ZERO BIT2 /*Timer0 过零中断*/

/*捕获模式定义*/
#define UTIMER_CapMode_None (0x00)     /*无捕获*/
#define UTIMER_CapMode_Rise (0x01)     /*上升沿捕获*/
#define UTIMER_CapMode_Fall (0x02)     /*下降沿捕获*/
#define UTIMER_CapMode_RiseFall (0x03) /*双沿捕获*/
#define UTIMER_CAP0_RST_CNT BIT0       ///< 当发生捕捉0事件，清零Timer计数器
#define UTIMER_CAP1_RST_CNT BIT1       ///< 当发生捕捉0事件，清零Timer计数器

/*捕获来源定义*/
#define UTIMER_CAP_CH0 (0x00)       ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CH1 (0x01)       ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CMP0 (0x02)      ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CMP1 (0x03)      ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CMP2 (0x04)      ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CMP3 (0x05)      ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CMP4 (0x06)      ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CMP5 (0x07)      ///< 捕捉来源 Timer 通道0
#define UTIMER_CAP_CH0xorCH1 (0x08) ///< 捕捉来源 Timer 通道0

/*工作模式定义*/
#define UTIMER_MODE_CAP (0x01) /*捕获模式*/
#define UTIMER_MODE_CMP (0x00) /*比较模式*/
/*时钟源定义*/
#define UTIMER_CLK_SRC_MCLK 0       ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER0_0 2  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER0_1 3  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER1_0 4  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER1_1 5  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER2_0 6  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER2_1 7  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER3_0 8  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER3_1 9  ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER4_0 10 ///< 芯片内部时钟
#define UTIMER_CLK_SRC_UTIMER4_1 11 ///< 芯片内部时钟
typedef struct
{
    u8 EN;            ///< Timer 模块整体使能，高有效
    u8 CAP1_CLR_EN;   ///< 当发生CAP1捕获事件时，清零Timer计数器，高有效
    u8 CAP0_CLR_EN;   ///< 当发生CAP0捕获事件时，清零Timer计数器，高有效
    u8 ONE_TRIG;	    ///< 在比较模式下，且 EN 为 0 时，写 1 触发 Timer 发送一个周期的特定占空比的脉冲，此位在脉冲发送期间内读为 1，一个 Timer 周期后，自动清零。
    u8 ETON;          ///< Timer 计数器计数使能配置 0:自动运行 1:等待外部事件触发计数
    u8 CLK_DIV;       ///< Timer 计数器分频设置
    u8 CLK_SRC;       ///< Timer 时钟源
    u8 SRC1;          ///< Timer 捕获模式通道 1 信号来源。默认为 3’h0
    u8 CH1_POL;       ///< Timer通道1在比较模式下的输出极性控制，当计数器计数值回零时的输出值
    u8 CH1_MODE;      ///< Timer通道1的工作模式选择，默认值为0
    u8 CH1_FE_CAP_EN; ///< Timer通道1下降沿捕获事件使能
    u8 CH1_RE_CAP_EN; ///< Timer通道1上升沿捕获事件使能
    u8 SRC0;          ///< Timer 捕获模式通道 1 信号来源。默认为 3’h0
    u8 CH0_POL;       ///< Timer通道1在比较模式下的输出极性控制，当计数器计数值回零时的输出值
    u8 CH0_MODE;      ///< Timer通道1的工作模式选择，默认值为0
    u8 CH0_FE_CAP_EN; ///< Timer通道1下降沿捕获事件使能
    u8 CH0_RE_CAP_EN; ///< Timer通道1上升沿捕获事件使能
    u32 TH;           ///< Timer 计数器计数门限。
    u32 CMP0;         ///< Timer 通道0工作在比较模式时，当计数器计数值等于CMP0时，发生比较事件。
    u32 CMP1;         ///< Timer 通道1工作在比较模式时，当计数器计数值等于CMP1时，发生比较事件。
    u8 EVT_SRC;       ///< Timer外部事件选择寄存器
    u8 FLT;           ///< 通道0/1信号滤波宽度选择。取值范围0~255
    u8 IE;            ///< 中断配置
    u8 RE;            ///< DMA请求
} UTIMER_InitTypeDef;

/*Timer初始化*/
void UTIMER_Init(UTIMER_TypeDef *TIMERx, UTIMER_InitTypeDef *TIM_TimerInitStruct);
void UTIMER_StructInit(UTIMER_InitTypeDef *TIM_TimerInitStruct);

/*TIM中断相关*/
u16 UTIMER_GetIRQFlag(UTIMER_TypeDef *UTIMERx, u32 TIMER_IF_x);
void UTIMER_ClearIRQFlag(UTIMER_TypeDef *UTIMERx, u32 TIMER_IF_x);
u16 UTIMER_GetFlag(UTIMER_TypeDef *UTIMERx, u32 TIMER_IF_x);

/*TIM读 计数|捕获 值*/
u32 UTIMER_GetCount(UTIMER_TypeDef *UTIMERx);
u32 UTIMER_GetCMPT0(UTIMER_TypeDef *UTIMERx);
u32 UTIMER_GetCMPT1(UTIMER_TypeDef *UTIMERx);

///< 延时
void UTIMER_DelayUs(UTIMER_TypeDef *UTIMERx, u32 xUs);
void UTIMER_Enable(UTIMER_TypeDef *UTIMERx);
void UTIMER_Disable(UTIMER_TypeDef *UTIMERx);

#endif /*__lks32mc45x_TIM_H */
