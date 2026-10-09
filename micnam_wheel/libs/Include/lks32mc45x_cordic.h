/**
 * @file
 * @brief        三角计算模块库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
 *  */
#ifndef __LKS32MC45x_CORDIC_H
#define __LKS32MC45x_CORDIC_H

///< Includes ------------------------------------------------------------------
#include "basic.h"
#include "lks32mc45x.h"
#include "lks32mc45x_sys.h"

typedef struct
{
    __IO u32 SC;     ///<  0x00 DSP状态控制寄存器
    __IO u32 THETA;  ///<  0x04 DSP sin/cos输入角度寄存器
    __IO u32 X;      ///<  0x08 DSP arctan/module计算输入坐标X寄存器
    __IO u32 Y;      ///<  0x0C DSP arctan/module计算输入坐标Y寄存器
    __IO u32 SIN;    ///<  0x10 DSP sin/cos计算结果sin寄存器
    __IO u32 COS;    ///<  0x14 DSP sin/cos计算结果cos寄存器
    __IO u32 MOD;    ///<  0x18 DSP arctan计算结果 sqrt(X2+Y2) 寄存器
    __IO u32 ARCTAN; ///<  0x1C DSP arctan计算结果arctan(Y/X) 角度寄存器
} CORDIC_TypeDef;
#ifndef CORDIC0
#define CORDIC0 ((CORDIC_TypeDef *)(CORDIC_BASE))
#endif
void CORDIC_Enable(void);
void CORDIC_Disable(void);
s16 CORDIC_CalcSin(s16 val);
s16 CORDIC_CalcCos(s16 val);
u16 CORDIC_CalcMod(s16 x, s16 y);
s16 CORDIC_CalcArctan(s16 x, s16 y);

// 增加CORDIC_CALC系列的接口，以降低参数传递带来的时间损耗
#define CORDIC_CALC_SIN(sin, angle)    \
    {                                  \
        int32_t flg = __get_PRIMASK(); \
        __disable_irq();               \
        CORDIC0->SC = BIT2;            \
        CORDIC0->THETA = (u16)angle;   \
        sin = CORDIC0->SIN;            \
        __set_PRIMASK(flg);            \
    }
#define CORDIC_CALC_COS(cos, angle)    \
    {                                  \
        int32_t flg = __get_PRIMASK(); \
        __disable_irq();               \
        CORDIC0->SC = BIT2;            \
        CORDIC0->THETA = (u16)angle;   \
        sin = CORDIC0->COS;            \
        __set_PRIMASK(flg);            \
    }
#define CORDIC_CALC_ARCTAN(arctan, x, y) \
    {                                    \
        int32_t flg = __get_PRIMASK();   \
        __disable_irq();                 \
        CORDIC0->SC = 0;                 \
        CORDIC0->Y = y;                  \
        arctan = CORDIC0->ARCTAN;        \
        __set_PRIMASK(flg);              \
    }
#define CORDIC_CALC_MOD(mod, x, y)     \
    {                                  \
        int32_t flg = __get_PRIMASK(); \
        __disable_irq();               \
        CORDIC0->SC = 0;               \
        CORDIC0->X = x;                \
        CORDIC0->Y = y;                \
        mod = CORDIC0->MOD;            \
        __set_PRIMASK(flg);            \
    }
// 兼容其他系列芯片里dsp库函数的操作方式
#define DSP_Init CORDIC_Enable
#define DSP_Sin CORDIC_CalcSin
#define DSP_Cos CORDIC_CalcCos
#define DSP_Mod CORDIC_CalcMod
#define DSP_Arctan CORDIC_CalcArctan

#define DSP_CALC_SIN CORDIC_CALC_SIN
#define DSP_CALC_COS CORDIC_CALC_COS
#define DSP_CALC_ARCTAN CORDIC_CALC_ARCTAN
#define DSP_CALC_MOD CORDIC_CALC_MOD
#endif /*__lks32mc45x_cmp_H */
