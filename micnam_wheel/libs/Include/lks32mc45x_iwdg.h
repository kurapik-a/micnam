/**
 * @file
 * @brief        独立看门狗库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __lks32mc45x_IWDG_H
#define __lks32mc45x_IWDG_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"
typedef struct
{
    __IO u32 PSW; ///< 0x00 独立看门狗密码寄存器
    __IO u32 CFG; ///< 0x04 独立看门狗配置寄存器
    __IO u32 CLR; ///< 0x08 独立看门狗清零寄存器
    __IO u32 WTH; ///< 0x0C 独立看门狗计数器定时唤醒门限值寄存器
    __IO u32 RTH; ///< 0x10 独立看门狗计数器超时复位门限值寄存器
    __IO u32 CNT; ///< 0x14 独立看门狗计数器当前计数值寄存器
} IWDG_TypeDef;
#ifndef IWDG
#define IWDG ((IWDG_TypeDef *)(AON_BASE + 0xC0))
#endif
typedef struct
{
    u32 DWK_EN; ///< 深度休眠定时唤醒使能
    u32 WDG_EN; ///< 独立看门狗使能
    u32 WTH;    ///< 看门狗定时唤醒时间（21位计数器，但低12恒位0）
    u32 RTH;    ///< 看门狗超时复位时间
} IWDG_InitTypeDef;

void IWDG_Init(IWDG_InitTypeDef *this);
void IWDG_StrutInit(IWDG_InitTypeDef *this);
void IWDG_Enable(void);
void IWDG_Disable(void);
void IWDG_FeedDog(void);
void IWDG_SetResetCnt(u32 cnt);

#endif /*__lks32mc03x_IWDG_H */
