/**
 * @file
 * @brief        编码器库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __LKS32MC45X_QEP_H
#define __LKS32MC45X_QEP_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "lks32mc45x_sys.h"
#include "basic.h"

typedef struct
{
    __IO u32 CFG; ///< 0x00  QEP0配置寄存器
    __IO u32 TH;  ///< 0x04  QEP0计数门限寄存器
    __IO u32 CNT; ///< 0x08  QEP0计数值寄存器
    __IO u32 IE;  ///< 0x0C  QEP0中断使能寄存器
    __IO u32 IF;  ///< 0x10  QEP0中断标志寄存器
} QEP_TypeDef;
#define TIM_QEPTypeDef QEP_TypeDef
#ifndef QEP0
#define QEP0 ((QEP_TypeDef *)QEP0_BASE)
#endif
#ifndef QEP1
#define QEP1 ((QEP_TypeDef *)QEP1_BASE)
#endif
#ifndef QEP2
#define QEP2 ((QEP_TypeDef *)QEP2_BASE)
#endif
#ifndef QEP3
#define QEP3 ((QEP_TypeDef *)QEP3_BASE)
#endif

/*编码器模式定义*/
#define QEP_Mode_T1 0             ///< counting on T1
#define QEP_Mode_T1_T2 1          ///< counting on T1&T2   ///< ABZ信号的编码器 最常见的模式
#define QEP_Mode_CCWSIGN 2        ///< CCW+SIGN, 仅计数信号上升沿
#define QEP_Mode_CCWCW 3          ///< CCW+CW, 仅计数信号上升沿
#define QEP_Mode_CCWSIGN_UpDown 4 ///< CCW+SIGN updown, 计数信号的上下沿
#define QEP_Mode_CCWCW_UpDown 5   ///< CCW+CW updown, 计数信号的上下沿

/*编码器中断使能配置定义*/
#define QEP_IE_NONE 0  ///< 无使能
#define QEP_IE_OF BIT1 ///< 使能上溢中断
#define QEP_IE_UF BIT0 ///< 使能下溢中断

/*编码器中断标志定义*/
#define QEP_IF_OF BIT1 ///< 使能上溢中断
#define QEP_IF_UF BIT0 ///< 使能下溢中断

/*编码器中断标志定义*/
#define QEP_ZC_NONE 0x0    ///< 不使用Z信号
#define QEP_ZC_EDGE 0xD    ///< Z信号跳变即清零计数器
#define QEP_ZC_POSEDGE 0x5 ///< Z信号上升沿清零计数器
#define QEP_ZC_NEGEDGE 0x9 ///< Z信号下降沿清零计数器
#define QEP_ZC_HIGH 0x6    ///< Z信号高电平清零计数器
#define QEP_ZC_LOW 0xA     ///< Z信号高电平清零计数器

typedef struct
{
    u8 EN;   ///< QEP模块整体使能
    u8 MODE; ///< 编码器模式选择
    u16 IE;  ///< 编码器中断配置
    u32 TH;  ///< QEP计数门限TH
    u32 ZC;  ///< Z信号配置
} QEP_InitTypeDef;

/*QEP初始化*/
void QEP_Init(QEP_TypeDef *QEPx, QEP_InitTypeDef *TIM_QEPInitStruct);
void QEP_StructInit(QEP_InitTypeDef *TIM_QEPInitStruct);
void QEP_ClearIRQFlag(QEP_TypeDef *QEPx, u16 QEP_IF_x);
u8 QEP_GetIRQFlag(QEP_TypeDef *QEPx, u16 QEP_IF_x);
u8 QEP_GetFlag(QEP_TypeDef *QEPx, u16 QEP_IF_x);

u32 QEP_GetCount(QEP_TypeDef *QEPx);
void QEP_Enable(QEP_TypeDef *QEPx);
void QEP_Disable(QEP_TypeDef *QEPx);
#endif /*__LKS32MC45X_QEP_H */
