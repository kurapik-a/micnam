/**
 * @file
 * @brief        窗看门狗库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __LKS32MC45X_WWDG_H
#define __LKS32MC45X_WWDG_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO u32 CR;  ///< 0x00 窗口看门狗控制寄存器
    __IO u32 CFG; ///< 0x04 窗口看门狗配置寄存器
    __IO u32 IF;  ///< 0x08 窗口看门狗中断标志寄存器
} WWDG_TypeDef;

#ifndef WWDG0
#define WWDG0 ((WWDG_TypeDef *)WWDT_BASE)
#endif

#define WWDG_IF_EW BIT0 ///< 窗看门狗超前中断标志

#define WWDG_IE_EW BIT7 ///< 窗看门狗超前中断使能
typedef struct
{
    u32 WDG_EN; ///< 窗口看门狗复位使能
    u32 T;      ///< 窗口看门狗计数门限
    u32 WDGTB;  ///< 窗口看门狗时钟预分频系数
    u32 EWI;    ///< 超前中断使能
    u32 W;      ///< 窗口看门狗喂狗窗口
} WWDG_InitTypeDef;

void WWDG_Init(WWDG_TypeDef *WWDGx, WWDG_InitTypeDef *this);
void WWDG_StructInit(WWDG_InitTypeDef *this);

void WWDG_ClearIRQFlag(u16 WWDG_IF_x);
u8 WWDG_GetIRQFlag(u16 WWDG_IF_x);
u8 WWDG_GetFlag(u16 WWDG_IF_x);

void WWDG_Enable(void);
void WWDG_Disable(void);
void WWDG_FeedDog(u16);
void WWDG_SetResetCnt(u16);

#endif /*__LKS32MC45X_WWDG_H */
