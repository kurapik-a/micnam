/**
 * @file
 * @defgroup cmp
 * @brief        比较器库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_cmp.h"
#include "string.h"
#include "lks32mc45x_sys.h"
#include "lks32mc45x_afe.h"
/**
 * @brief    比较器初始化
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void CMP_Init(CMP_InitTypeDef *this)
{
    typedef union
    {
        float f;
        struct
        {
            u32 mag : 23; // 符号位
            u32 exp : 8;  // 符号位
            u32 sign : 1; // 符号位
        } b;
    } stru_float_t;
    stru_float_t clk[3];
    s32 div2[3], div16[3], i;

    this->CLK01_DIV = (this->CLK01_DIV < 1) ? 1 : (this->CLK01_DIV > 2048) ? 2048
                                                                           : this->CLK01_DIV;
    this->CLK23_DIV = (this->CLK23_DIV < 1) ? 1 : (this->CLK23_DIV > 2048) ? 2048
                                                                           : this->CLK23_DIV;
    this->CLK45_DIV = (this->CLK45_DIV < 1) ? 1 : (this->CLK45_DIV > 2048) ? 2048
                                                                           : this->CLK45_DIV;
    clk[0].f = (float)this->CLK01_DIV;
    clk[1].f = (float)this->CLK23_DIV;
    clk[2].f = (float)this->CLK45_DIV;
    for (i = 0; i < 3; i++)
    {
        // clk[i].b.exp -= 1;
        div2[i] = clk[i].b.exp - 127 - 3;
        if (div2[i] < 0)
        {
            div16[i] = ((clk[i].b.mag | (1 << 23)) >> (20 - div2[i]));
            div2[i] = 0;
        }
        else
        {
            div16[i] = ((clk[i].b.mag | (1 << 23)) >> 20);
        }
    }
    CMP->IE = (u32)(this->cmp0.IE) | (u32)(this->cmp1.IE << 1) | (u32)(this->cmp2.IE << 2) | (u32)(this->cmp3.IE << 3) | (u32)(this->cmp4.IE << 4) | (u32)(this->cmp5.IE << 5) |
              (u32)(this->cmp0.RE << 8) | (u32)(this->cmp1.RE << 9) | (u32)(this->cmp2.RE << 10) | (u32)(this->cmp3.RE << 11) | (u32)(this->cmp4.RE << 12) | (u32)(this->cmp5.RE << 13);
    CMP->TCLK = (0X80808) | (u32)(div2[0] << 0) | (u32)(div16[0] << 4) | (u32)(div2[1] << 8) | (u32)(div16[1] << 12) | (u32)(div2[2] << 16) | (u32)(div16[2] << 20);
    CMP->CFG = (((u32)(this->cmp0.POL << 0) | (u32)(this->cmp0.IN_EN << 1) | (u32)(this->cmp0.IRQ_TRIG << 2) | (u32)(this->cmp0.W_PWM_POL << 3)) << 0) |
               (((u32)(this->cmp1.POL << 0) | (u32)(this->cmp1.IN_EN << 1) | (u32)(this->cmp1.IRQ_TRIG << 2) | (u32)(this->cmp1.W_PWM_POL << 3)) << 4) |
               (((u32)(this->cmp2.POL << 0) | (u32)(this->cmp2.IN_EN << 1) | (u32)(this->cmp2.IRQ_TRIG << 2) | (u32)(this->cmp2.W_PWM_POL << 3)) << 8) |
               (((u32)(this->cmp3.POL << 0) | (u32)(this->cmp3.IN_EN << 1) | (u32)(this->cmp3.IRQ_TRIG << 2) | (u32)(this->cmp3.W_PWM_POL << 3)) << 12) |
               (((u32)(this->cmp4.POL << 0) | (u32)(this->cmp4.IN_EN << 1) | (u32)(this->cmp4.IRQ_TRIG << 2) | (u32)(this->cmp4.W_PWM_POL << 3)) << 16) |
               (((u32)(this->cmp5.POL << 0) | (u32)(this->cmp5.IN_EN << 1) | (u32)(this->cmp5.IRQ_TRIG << 2) | (u32)(this->cmp5.W_PWM_POL << 3)) << 20);
    CMP->BLCWIN0 = (u32)(((u32)(this->cmp0.CHN0P_PWM0 << 0) | (u32)(this->cmp0.CHN1P_PWM0 << 1) | (u32)(this->cmp0.CHN2P_PWM0 << 2) | (u32)(this->cmp0.CHN3P_PWM0 << 3)) << 0) |
                   (u32)(((u32)(this->cmp1.CHN0P_PWM0 << 0) | (u32)(this->cmp1.CHN1P_PWM0 << 1) | (u32)(this->cmp1.CHN2P_PWM0 << 2) | (u32)(this->cmp1.CHN3P_PWM0 << 3)) << 4) |
                   (u32)(((u32)(this->cmp2.CHN0P_PWM1 << 0) | (u32)(this->cmp2.CHN1P_PWM1 << 1) | (u32)(this->cmp2.CHN2P_PWM1 << 2) | (u32)(this->cmp2.CHN3P_PWM1 << 3)) << 8) |
                   (u32)(((u32)(this->cmp3.CHN0P_PWM1 << 0) | (u32)(this->cmp3.CHN1P_PWM1 << 1) | (u32)(this->cmp3.CHN2P_PWM1 << 2) | (u32)(this->cmp3.CHN3P_PWM1 << 3)) << 12);
    CMP->BLCWIN1 = (u32)(((u32)(this->cmp4.CHN0P_PWM0 << 0) | (u32)(this->cmp4.CHN1P_PWM0 << 1) | (u32)(this->cmp4.CHN2P_PWM0 << 2) | (u32)(this->cmp4.CHN3P_PWM0 << 3)) << 0) |
                   (u32)(((u32)(this->cmp4.CHN0P_PWM1 << 0) | (u32)(this->cmp4.CHN1P_PWM1 << 1) | (u32)(this->cmp4.CHN2P_PWM1 << 2) | (u32)(this->cmp4.CHN3P_PWM1 << 3)) << 4) |
                   (u32)(((u32)(this->cmp5.CHN0P_PWM0 << 0) | (u32)(this->cmp5.CHN1P_PWM0 << 1) | (u32)(this->cmp5.CHN2P_PWM0 << 2) | (u32)(this->cmp5.CHN3P_PWM0 << 3)) << 8) |
                   (u32)(((u32)(this->cmp5.CHN0P_PWM1 << 0) | (u32)(this->cmp5.CHN1P_PWM1 << 1) | (u32)(this->cmp5.CHN2P_PWM1 << 2) | (u32)(this->cmp5.CHN3P_PWM1 << 3)) << 12);
    CMP->IF = 0x3f;

    AFE_CmpTimeConfig((u32)((this->IT_CMP << 1) | this->FT));
    AFE_CmpHysteresisConfig(this->HYS);

    AFE_ModuleClockCmd(AFE_MODULE_CMP0, this->cmp0.EN);
    AFE_CmpPassagewayConfig(AFE_MODULE_CMP0, this->cmp0.SELP, this->cmp0.SELN);

    AFE_ModuleClockCmd(AFE_MODULE_CMP1, this->cmp1.EN);
    AFE_CmpPassagewayConfig(AFE_MODULE_CMP1, this->cmp1.SELP, this->cmp1.SELN);

    AFE_ModuleClockCmd(AFE_MODULE_CMP2, this->cmp2.EN);
    AFE_CmpPassagewayConfig(AFE_MODULE_CMP2, this->cmp2.SELP, this->cmp2.SELN);

    AFE_ModuleClockCmd(AFE_MODULE_CMP3, this->cmp3.EN);
    AFE_CmpPassagewayConfig(AFE_MODULE_CMP3, this->cmp3.SELP, this->cmp3.SELN);

    AFE_ModuleClockCmd(AFE_MODULE_CMP4, this->cmp4.EN);
    AFE_CmpPassagewayConfig(AFE_MODULE_CMP4, this->cmp4.SELP, this->cmp4.SELN);

    AFE_ModuleClockCmd(AFE_MODULE_CMP5, this->cmp5.EN);
    AFE_CmpPassagewayConfig(AFE_MODULE_CMP5, this->cmp5.SELP, this->cmp5.SELN);
}
/**
 * @brief    比较器配置结构体初始化
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void CMP_StructInit(CMP_InitTypeDef *this)
{
    memset(this, 0, sizeof(CMP_InitTypeDef));
}
/**
 * @brief    清CMP中断
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void CMP_ClearIRQFlag(u8 CMPx)
{
    CMP->IF = 1 << CMPx;
}
/**
 * @brief    读CMP中断
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
u8 CMP_GetIRQFlag(u8 CMPx)
{
    if (CMP->IE & CMP->IF & (1 << CMPx))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    读CMP中断
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
u8 CMP_GetFlag(u8 CMP_CHN_x)
{
    if (CMP->IF & (1 << CMP_CHN_x))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void CMP_Enable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_CMP, ENABLE);
}
/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/08/13  YangZJ  创建
 */
void CMP_Disable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_CMP, DISABLE);
}
/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/08/13  YangZJ  创建
 */
void CMP_AnalogDisable(u8 CMP_CHN_x)
{
    u32 AFE_MODULE_CMPx;
    switch (CMP_CHN_x)
    {
    case CMP_CHN_0:
        AFE_MODULE_CMPx = AFE_MODULE_CMP0;
        break;
    case CMP_CHN_1:
        AFE_MODULE_CMPx = AFE_MODULE_CMP1;
        break;
    case CMP_CHN_2:
        AFE_MODULE_CMPx = AFE_MODULE_CMP2;
        break;
    case CMP_CHN_3:
        AFE_MODULE_CMPx = AFE_MODULE_CMP3;
        break;
    case CMP_CHN_4:
        AFE_MODULE_CMPx = AFE_MODULE_CMP4;
        break;
    case CMP_CHN_5:
        AFE_MODULE_CMPx = AFE_MODULE_CMP5;
        break;
    default:
        AFE_MODULE_CMPx = 0;
        break;
    }
    AFE_ModuleClockCmd(AFE_MODULE_CMPx, DISABLE);
}

/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/08/13  YangZJ  创建
 */
void CMP_AnalogEnable(u8 CMP_CHN_x)
{
    u32 AFE_MODULE_CMPx;
    switch (CMP_CHN_x)
    {
    case CMP_CHN_0:
        AFE_MODULE_CMPx = AFE_MODULE_CMP0;
        break;
    case CMP_CHN_1:
        AFE_MODULE_CMPx = AFE_MODULE_CMP1;
        break;
    case CMP_CHN_2:
        AFE_MODULE_CMPx = AFE_MODULE_CMP2;
        break;
    case CMP_CHN_3:
        AFE_MODULE_CMPx = AFE_MODULE_CMP3;
        break;
    case CMP_CHN_4:
        AFE_MODULE_CMPx = AFE_MODULE_CMP4;
        break;
    case CMP_CHN_5:
        AFE_MODULE_CMPx = AFE_MODULE_CMP5;
        break;
    default:
        AFE_MODULE_CMPx = 0;
        break;
    }
    AFE_ModuleClockCmd(AFE_MODULE_CMPx, ENABLE);
}

/*! @} */
