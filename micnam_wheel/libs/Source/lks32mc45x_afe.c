/**
 * @file
 * @defgroup afe
 * @brief        模拟部分库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_afe.h"
#include "lks32mc45x_lib.h"
#include "lks32mc45x_sys.h"

/**
 * @brief 运放输出到P3.11使能
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_OpaOutGpio3Pin11(u8 AFE_OUT_P3_11_x)
{
    u32 reg2;
    reg2 = AFE->REG2;

    reg2 &= (u32)~0x3;
    reg2 |= (AFE_OUT_P3_11_x & 0x03);

    SYS->PROTECT = 0x7a83;
    AFE->REG2 = reg2;
    SYS->PROTECT = 0;
}
/**
 * @brief 运放输出到P4.6使能
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_OpaOutGpio4Pin6(u8 AFE_OUT_P4_6_x)
{
    u32 reg2;
    reg2 = AFE->REG2;

    reg2 &= (u32)~0x30;
    reg2 |= (AFE_OUT_P4_6_x & 0x30);

    SYS->PROTECT = 0x7a83;
    AFE->REG2 = reg2;
    SYS->PROTECT = 0;
}
/**
 * @brief 运放增益配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_OpaGainConfig(u32 AFE_OPAx, u32 AFE_OPA_RES_x)
{
    u32 reg0, reg1;
    reg0 = AFE->REG0;
    reg1 = AFE->REG1;
    if (AFE_OPAx & AFE_MODULE_OPA0)
    {
        reg0 &= (u32) ~(7 << 0);
        reg0 |= (u32)(AFE_OPA_RES_x << 0);
    }
    if (AFE_OPAx & AFE_MODULE_OPA1)
    {
        reg0 &= (u32) ~(7 << 4);
        reg0 |= (u32)(AFE_OPA_RES_x << 4);
    }
    if (AFE_OPAx & AFE_MODULE_OPA2)
    {
        reg0 &= (u32) ~(7 << 8);
        reg0 |= (u32)(AFE_OPA_RES_x << 8);
    }
    if (AFE_OPAx & AFE_MODULE_OPA3)
    {
        reg0 &= (u32) ~(7 << 12);
        reg0 |= (u32)(AFE_OPA_RES_x << 12);
    }
    if (AFE_OPAx & AFE_MODULE_OPA4)
    {
        reg1 &= (u32) ~(7 << 0);
        reg1 |= (u32)(AFE_OPA_RES_x << 0);
    }
    if (AFE_OPAx & AFE_MODULE_OPA5)
    {
        reg1 &= (u32) ~(7 << 4);
        reg1 |= (u32)(AFE_OPA_RES_x << 4);
    }
    SYS->PROTECT = 0x7a83;
    AFE->REG0 = reg0;
    AFE->REG1 = reg1;
    SYS->PROTECT = 0;
}

/**
 * @brief 比较器模拟部分比较时间配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_CmpTimeConfig(u32 AFE_CMP_TIME_x)
{
    u32 reg2;

    reg2 = AFE->REG2;
    reg2 &= ~(BIT8 | BIT9);
    reg2 |= (u32)(AFE_CMP_TIME_x << 8);

    SYS->PROTECT = 0x7a83;
    AFE->REG2 = reg2;
    SYS->PROTECT = 0;
}
/**
 * @brief 比较器模拟部分回差配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_CmpHysteresisConfig(u32 AFE_CMP_HYS_x)
{
    u32 reg4;

    reg4 = AFE->REG4;
    reg4 &= ~BIT3;
    reg4 |= (u32)(AFE_CMP_HYS_x << 3);

    SYS->PROTECT = 0x7a83;
    AFE->REG4 = reg4;
    SYS->PROTECT = 0;
}
/**
 * @brief 比较器模拟部分通道配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_CmpPassagewayConfig(u32 AFE_MODULE_CMPx, u32 AFE_CMPx_SELP_x, u32 AFE_CMPx_SELN_x)
{
    u32 reg4, reg5, reg6;
    u32 p = AFE_CMPx_SELP_x;
    u32 n = AFE_CMPx_SELN_x;

    reg4 = AFE->REG4;
    reg5 = AFE->REG5;
    reg6 = AFE->REG6;
    if (AFE_MODULE_CMPx & AFE_MODULE_CMP0)
    {
        reg4 &= (u32) ~(3 << 14);
        reg4 |= (u32)(n << 14);
        reg5 &= (u32) ~(7 << 12);
        reg5 |= (u32)(p << 12);
    }
    if (AFE_MODULE_CMPx & AFE_MODULE_CMP1)
    {
        reg4 &= (u32) ~(3 << 12);
        reg4 |= (u32)(n << 12);
        reg5 &= (u32) ~(7 << 8);
        reg5 |= (u32)(p << 8);
    }
    if (AFE_MODULE_CMPx & AFE_MODULE_CMP2)
    {
        reg5 &= (u32) ~(3 << 2);
        reg5 |= (u32)(n << 2);
        reg6 &= (u32) ~(7 << 4);
        reg6 |= (u32)(p << 4);
    }
    if (AFE_MODULE_CMPx & AFE_MODULE_CMP3)
    {
        reg5 &= (u32) ~(3 << 0);
        reg5 |= (u32)(n << 0);
        reg6 &= (u32) ~(7 << 0);
        reg6 |= (u32)(p << 0);
    }
    if (AFE_MODULE_CMPx & AFE_MODULE_CMP4)
    {
        reg5 &= (u32) ~(3 << 6);
        reg5 |= (u32)(n << 6);
        reg6 &= (u32) ~(7 << 12);
        reg6 |= (u32)(p << 12);
    }
    if (AFE_MODULE_CMPx & AFE_MODULE_CMP5)
    {
        reg5 &= (u32) ~(3 << 4);
        reg5 |= (u32)(n << 4);
        reg6 &= (u32) ~(7 << 8);
        reg6 |= (u32)(p << 8);
    }
    SYS->PROTECT = 0x7a83;
    AFE->REG4 = reg4;
    AFE->REG5 = reg5;
    AFE->REG6 = reg6;
    SYS->PROTECT = 0;
}
/**
 * @brief ADC模拟部分配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_AdcConfig(u32 AFE_ADC_CLK_x, u32 AFE_ADC_REF_x)
{
    u32 reg2, reg3;
    reg2 = AFE->REG2;
    reg3 = AFE->REG3;
    reg2 &= ~(BIT12 | BIT11);
    reg3 &= ~(BIT10 | BIT11 | BIT12);
    reg2 |= (u32)(AFE_ADC_REF_x << 12);
    reg3 |= (u32)(AFE_ADC_CLK_x << 10);
    SYS->PROTECT = 0x7a83;
    AFE->REG2 = reg2;
    AFE->REG3 = reg3;
    SYS->PROTECT = 0;
}
/**
 * @brief DAC输出到P3.4配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_Dac0OutGpio3Pin4(u8 state)
{
    u32 reg4;
    reg4 = AFE->REG4;
    reg4 &= ~BIT4;
    if (state)
    {
        reg4 |= BIT4;
    }
    SYS->PROTECT = 0x7a83;
    AFE->REG4 = reg4;
    SYS->PROTECT = 0;
}

/**
 * @brief DAC输出到P4.7配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_Dac1OutGpio4Pin7(u8 state)
{
    u32 reg4;
    reg4 = AFE->REG4;
    reg4 &= ~BIT5;
    if (state)
    {
        reg4 |= BIT5;
    }
    SYS->PROTECT = 0x7a83;
    AFE->REG4 = reg4;
    SYS->PROTECT = 0;
}
/**
 * @brief DAC输出配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_DacSetVal(u32 AFE_MODULE_DACx, u16 val)
{
    val = (val > 0xfff) ? 0xfff : val;
    if (AFE_MODULE_DACx & AFE_MODULE_DAC0)
    {
        AFE->DAC0 = val;
    }
    if (AFE_MODULE_DACx & AFE_MODULE_DAC1)
    {
        AFE->DAC1 = val;
    }
}
/**
 * @brief DAC输出电压配置
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_DacSetVoltage(u32 AFE_MODULE_DACx, float Voltage)
{
    u16 val;
    float range;
    if (AFE_MODULE_DACx & AFE_MODULE_DAC0)
    {
        if (AFE->REG4 & BIT0)
        {
            range = (float)1.2;
            SYS_AFE_DAC0_AMC = TRIM_Read(TRIM_VAL_DAC0_AMC_12);
            SYS_AFE_DAC0_DC = TRIM_Read(TRIM_VAL_DAC0_DC_12);
        }
        else
        {
            range = (float)3.0;
            SYS_AFE_DAC0_AMC = TRIM_Read(TRIM_VAL_DAC0_AMC_30);
            SYS_AFE_DAC0_DC = TRIM_Read(TRIM_VAL_DAC0_DC_30);
        }
        Voltage = (Voltage > range) ? range : Voltage;
        val = (u16)((Voltage / range) * 0xfff);
        val = (val > 0xfff) ? 0xfff : val;
        AFE->DAC0 = val;
    }
    if (AFE_MODULE_DACx & AFE_MODULE_DAC1)
    {
        if (AFE->REG4 & BIT2)
        {
            range = (float)1.2;
            SYS_AFE_DAC1_AMC = TRIM_Read(TRIM_VAL_DAC1_AMC_12);
            SYS_AFE_DAC1_DC = TRIM_Read(TRIM_VAL_DAC1_DC_12);
        }
        else
        {
            range = (float)3.0;
            SYS_AFE_DAC1_AMC = TRIM_Read(TRIM_VAL_DAC1_AMC_30);
            SYS_AFE_DAC1_DC = TRIM_Read(TRIM_VAL_DAC1_DC_30);
        }
        Voltage = (Voltage > range) ? range : Voltage;
        val = (u16)((Voltage / range) * 0xfff);
        val = (val > 0xfff) ? 0xfff : val;
        AFE->DAC1 = val;
    }
}
/**
 * @brief DAC量程选择
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_DacSetRange(u32 AFE_MODULE_DACx, u16 AFE_DAC_RANGE_x)
{
    u32 reg4;
    reg4 = AFE->REG4;

    if (AFE_MODULE_DACx & AFE_MODULE_DAC0)
    {
        reg4 &= ~BIT0;
        reg4 |= AFE_DAC_RANGE_x;
    }
    if (AFE_MODULE_DACx & AFE_MODULE_DAC1)
    {
        reg4 &= ~BIT2;
        reg4 |= (u32)(AFE_DAC_RANGE_x << 2);
    }

    SYS->PROTECT = 0x7a83;
    AFE->REG4 = reg4;
    SYS->PROTECT = 0;
}

/**
 * @brief 时钟配置（模拟部分）
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 * @par 更新记录  V1.0  2023/09/12  zhangfei  pll使能后添加延时
 */
void AFE_ClkInit(AFE_ClkInitTypeDef *this)
{
    u32 reg1, reg4, regB;
    volatile u32 t_cnt;
    AFE_ModuleClockCmd(AFE_MODULE_PLL, this->PLLPDN);
    AFE_ModuleClockCmd(AFE_MODULE_XTAL, this->XTALPDN);

    for (t_cnt = 0; t_cnt < 100; t_cnt++)
    {
        __NOP();
    }
    reg1 = AFE->REG1;
    reg4 = AFE->REG4;
    regB = AFE->REGB;
    reg1 &= ~(BIT10);
    reg4 &= ~(BIT6 | BIT7);
    regB &= ~(BIT7);
    reg1 |= (u32)(this->XDIV << 10);
    reg4 |= (u32)(this->XTRSEL << 6);
    regB |= (u32)(this->PLLSR_SEL << 7);
    SYS->PROTECT = 0x7a83;
    AFE->REG1 = reg1;
    AFE->REG4 = reg4;
    AFE->REGB = regB;
    SYS->PROTECT = 0;
}
/**
 * @brief 调试输出（模拟部分）
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_DebugInit(AFE_DebugInitTypeDef *this)
{
    u32 reg2, regB;
    reg2 = AFE->REG2;
    regB = AFE->REGB;
    reg2 &= ~(BIT15);
    regB = regB & BIT7;
    reg2 |= (u32)(this->LDOOUT_EN << 15);
    regB |= (u32)(this->PORFIL_EN << 6) | (u32)(this->PVDSEL << 2) | (u32)(this->VSR_PDT << 1) | (u32)(this->PDT_PD << 0);
    SYS->PROTECT = 0x7a83;
    AFE->REG2 = reg2;
    AFE->REGB = regB;
    SYS->PROTECT = 0;
}
/**
 * @brief 模块使能（模拟部分）
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void AFE_ModuleClockCmd(u32 nMODULE, u8 state)
{
    u32 reg9, regA, nMODULE1;
    reg9 = AFE->REG9;
    regA = AFE->REGA;
    regA = regA ^ 0x13;
    nMODULE1 = nMODULE >> 16;
    if (state)
    {
        reg9 |= (0xffff & nMODULE);
        regA |= (0xffff & nMODULE1);
    }
    else
    {
        reg9 &= ~(0xffff & nMODULE);
        regA &= ~(0xffff & nMODULE1);
    }
    regA = regA ^ 0x13;
    reg9 &= ~(BIT6 | BIT7 | BIT14);
    SYS->PROTECT = 0x7a83;
    AFE->REG9 = reg9;
    AFE->REGA = regA;
    SYS->PROTECT = 0;
}
/**
 * @brief    设置晶振电容
 * @param SYS_XTAL_XCSEL_x 设置的晶振电容值
 */
void AFE_SetXtalCap(uint8_t *AFE_XCSEL_x)
{
    u32 reg1;
    reg1 = AFE->REG1;
    reg1 &= ~(BIT8 | BIT9);
    reg1 |= ((uint32_t)AFE_XCSEL_x << 8);
    SYS->PROTECT = 0x7a83;
    AFE->REG1 = reg1;
    SYS->PROTECT = 0;
}
/*! @} */
