/**
 * @file
 * @defgroup cordic
 * @brief        三角计算模块库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_cordic.h"
/**
 * @brief    使用DSP计算正弦
 * @param u16  val     角度0-65535对应0-360度
 * @return s16 result  计算结果Q15格式
 * @par 更新记录  V1.0  2022.03.10  YangZJ  创建
 */
s16 CORDIC_CalcSin(s16 val)
{
    s16 result;
    int32_t flg = __get_PRIMASK();
    __disable_irq();
    DSP_SC = BIT2;
    DSP_CORDIC_THETA = (u32)val;
    result = (s16)DSP_CORDIC_SIN;
    __set_PRIMASK(flg);
    return result;
}
/**
 * @brief    使用08x的DSP计算余弦
 * @param s16  val    角度0-65535对应0-360度
 * @return s16  result 计算结果Q15格式
 * @par 更新记录  V1.0  2022.03.10  YangZJ  创建
 */
s16 CORDIC_CalcCos(s16 val)
{
    s16 result;
    int32_t flg = __get_PRIMASK();
    __disable_irq();
    DSP_SC = BIT2;
    DSP_CORDIC_THETA = (u32)val;
    result = (s16)DSP_CORDIC_COS;
    __set_PRIMASK(flg);
    return result;
}
/**
 * @brief    使用DSP计算均方根 求模 sqrt(a^2+b^2)
 * @param s16 a,b
 * @par 更新记录  V1.0  2022.03.10  YangZJ  创建
 */
u16 CORDIC_CalcMod(s16 a, s16 b)
{
    u16 result;
    int32_t flg = __get_PRIMASK();
    __disable_irq();
    DSP_SC = 0;
    DSP_CORDIC_X = a;
    DSP_CORDIC_Y = b;
    result = (u16)DSP_CORDIC_MOD;
    __set_PRIMASK(flg);
    return result;
}
/**
 * @brief    使用DSP计算反正切
 * @param s16  x,y
 * @return 计算结果 角度0-65535对应0-360度
 * @par 更新记录  V1.0  2022.03.10  YangZJ  创建
 */
s16 CORDIC_CalcArctan(s16 x, s16 y)
{
    s16 result = 0;
    int32_t flg = __get_PRIMASK();
    __disable_irq();
    DSP_SC = 0;
    DSP_CORDIC_X = (u16)x;
    DSP_CORDIC_Y = (u16)y;
    result = (s16)DSP_CORDIC_ARCTAN;
    __set_PRIMASK(flg);
    return result;
}
/**
 * @brief 三角计算模块时钟使能
 */
void CORDIC_Enable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_CORDIC, ENABLE);
}
/**
 * @brief 三角计算模块时钟关闭
 */
void CORDIC_Disable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_CORDIC, DISABLE);
}

/*! @} */
