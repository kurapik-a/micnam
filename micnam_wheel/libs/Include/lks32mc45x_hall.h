/**
 * @file
 * @brief        霍尔库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */

#ifndef __lks32mc45x_HALL_H
#define __lks32mc45x_HALL_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO u32 CFG;
    __IO u32 INFO;
    __IO u32 WIDTH;
    __IO u32 TH;
    __IO u32 CNT;

} HALL_TypeDef;

typedef struct
{
    u32 CountTH;         /*HALL计数器门限值*/
    u16 FilterLen;       /*滤波长度,0对应长度1,1023对应长度1024滤波长度*/
    u8 ClockDivision;    /*分频 0~3:/1 /2 /4 /8*/
    u8 Filter75_Ena;     /*使能第一级7/5滤波,高电平有效*/
    u8 HALL_Ena;         /*使能HALL,高电平有效*/
    u8 Capture_IRQ_Ena;  /*HALL信号变化中断使能,高电平有效*/
    u8 softIE;           ///< 软件中断使能
    u8 OverFlow_IRQ_Ena; /*HALL计数器溢出中断使能,高电平有效*/
} HALL_InitTypeDef;

#define HALL_IF_OV BIT17  ///< HALL 计数器溢出事件标志
#define HALL_IF_CHG BIT16 ///< HALL 信号变化事件标志

#define HALL_IE_SW BIT30  ///< 软件触发 HALL 信号变化中断使能
#define HALL_IE_OV BIT29  ///< HALL 计数器溢出中断使能
#define HALL_IE_CHG BIT28 ///< HALL 信号变化中断使能

#define HALL_RE_SW BIT27  ///< 软件触发 HALL 信号变化 DMA 请求使能
#define HALL_RE_OV BIT26  ///< HALL 计数器溢出 DMA 请求使能
#define HALL_RE_CHG BIT25 ///< HALL 信号变化 DMA 请求使能

#define HALL_CLK_DIV1 ((u32)0x00)
#define HALL_CLK_DIV2 ((u32)0x01)
#define HALL_CLK_DIV4 ((u32)0x02)
#define HALL_CLK_DIV8 ((u32)0x03)

#define HALL_CAPTURE_EVENT ((u32)0x00010000)
#define HALL_OVERFLOW_EVENT ((u32)0x00020000)

void HALL_Init(HALL_TypeDef *HALLx, HALL_InitTypeDef *HALL_InitStruct);
void HALL_StructInit(HALL_InitTypeDef *HALL_InitStruct);

u32 HALL_GetFilterValue(HALL_TypeDef *HALL);
u32 HALL_GetCaptureValue(HALL_TypeDef *HALL);
u32 HALL_GetCount(HALL_TypeDef *HALL);

u32 HALL_IsCaptureEvent(HALL_TypeDef *HALL);
u32 HALL_IsOverFlowEvent(HALL_TypeDef *HALL);

void HALL_ClearIRQFlag(HALL_TypeDef *HALLx, u16 HALL_IF_x);
u8 HALL_GetIRQFlag(HALL_TypeDef *HALLx, u16 HALL_IF_x);
u8 HALL_GetFlag(HALL_TypeDef *HALLx, u16 HALL_IF_x);

void HALL_Enable(HALL_TypeDef *HALLx);
void HALL_Disable(HALL_TypeDef *HALLx);
void HALL_Trigger(HALL_TypeDef *HALLx);

#endif /*__lks32mc45x_HALL_H */
