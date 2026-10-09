/**
 * @file
 * @brief        FMAC库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __LKS32MC45X_FLASH__
#define __LKS32MC45X_FLASH__

#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO u32 X1BUFCFG; ///<  0x00 FMAC X1 buffer配置寄存器
    __IO u32 X2BUFCFG; ///<  0x04 FMAC X2 buffer配置寄存器
    __IO u32 YBUFCFG;  ///<  0x08 FMAC Y buffer配置寄存器
    __IO u32 PARAM;    ///<  0x0C FMAC参数寄存器
    __IO u32 CR;       ///<  0x10 FMAC控制寄存器
    __IO u32 SR;       ///<  0x14 FMAC状态寄存器
    __IO u32 WDATA;    ///<  0x18 FMAC写数据寄存器
    __IO u32 RDATA;    ///<  0x1C FMAC读数据寄存器
} FMAC_TypeDef;
#ifndef FMAC0
#define FMAC0 ((FMAC_TypeDef *)(FMAC_BASE))
#endif

#define FMAC_IE_SAT BIT4  ///< [4] 累加器饱和错误中断使能      0：禁用    1：使能，当 FMAC_SR.SAT 置位时产生中断请求
#define FMAC_IE_UNFL BIT3 ///< [3] Y buffer 下溢出中断使能    0：禁用    1：使能，当 FMAC_SR.UNFL 置位时产生中断请求
#define FMAC_IE_OVFL BIT2 ///< [2] X1 buffer 上溢出中断使能   0：禁用    1：使能，当 FMAC_SR.OVFL 置位时产生中断请求
#define FMAC_IE_W BIT1    ///< [1] X1 buffer 写请求中断使能   0：禁用    1：使能，当 X1 buffer 不满时，向 CPU 发出写请求中断
#define FMAC_IE_R BIT0    ///< [0] Y buffer 读请求使能        0：禁用    1：使能，当 Y buffer 不空时，向 CPU 发出读请求中断

#define FMAC_IF_SAT BIT10 ///< [10] 饱和错误标志
#define FMAC_IF_UNFL BIT9 ///< [9] Y buffer 下溢出错误标志
#define FMAC_IF_OVFL BIT8 ///< [8] X1 buffer 上溢出错误标志
#define FMAC_IF_W BIT1    ///< [1] X1 buffer 满标志
#define FMAC_IF_R BIT0    ///< [0] Y buffer 空标志

void FMAC_ClearIRQFlag(u16 FMAC_IF_x);
u8 FMAC_GetIRQFlag(u16 FMAC_IF_x);
u8 FMAC_GetFlag(u16 FMAC_IF_x);

void FMAC_InitX1(FMAC_TypeDef *FMACx, u16 *data, u8 size);
void FMAC_InitX2(FMAC_TypeDef *FMACx, u16 *data, u8 size);
void FMAC_InitY(FMAC_TypeDef *FMACx, u16 *data, u8 size);
#endif
