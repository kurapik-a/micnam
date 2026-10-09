/**
 * @file
 * @defgroup mcpwm
 * @brief        MCPWM库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_mcpwm.h"
#include "lks32mc45x.h"
#include "lks32mc45x_sys.h"
#include "string.h"

/**
 * @brief    MCPWM结构体初始化
 * @par 更新记录  V1.0  2020/08/21  YangZJ  创建
 */
void MCPWM_StructInit(MCPWM_InitTypeDef *MCPWM_InitStruct)
{

    memset(MCPWM_InitStruct, 0, sizeof(MCPWM_InitTypeDef));
}

/**
 * @brief    MCPWM初始化
 * @par 更新记录  V1.0  2020/08/26  YangZJ  Li          创建
 */
void MCPWM_Init(MCPWM_TypeDef *MCPWMx, MCPWM_InitTypeDef *MCPWM_InitStruct)
{
    if (MCPWMx == MCPWM0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_MCPWM0, ENABLE);
    }
    else
    {
        SYS_ModuleClockCmd(SYS_MODULE_MCPWM1, ENABLE);
    }

    MCPWMx->PRT = 0x0000DEAD; /*enter password to unlock write protection */
    MCPWMx->TCLK = (u32)MCPWM_InitStruct->CLK_DIV | (u32)(MCPWM_InitStruct->MCLK_EN << 2) | (u32)(MCPWM_InitStruct->TMR2_TimeBase_Sel << 4) | (u32)(MCPWM_InitStruct->TMR3_TimeBase_Sel << 5) | (u32)(MCPWM_InitStruct->MCPWM_Cnt0_EN << 6) | (u32)(MCPWM_InitStruct->MCPWM_Cnt1_EN << 7) | (u32)(MCPWM_InitStruct->TimeBase0_Trig_Enable << 8) | (u32)(MCPWM_InitStruct->TimeBase1_Trig_Enable << 9);
    MCPWMx->TH0 = MCPWM_InitStruct->TimeBase0_PERIOD;
    MCPWMx->TH1 = MCPWM_InitStruct->TimeBase1_PERIOD;

    MCPWMx->TMR0 = MCPWM_InitStruct->TriggerPoint0;
    MCPWMx->TMR1 = MCPWM_InitStruct->TriggerPoint1;
    MCPWMx->TMR2 = MCPWM_InitStruct->TriggerPoint2;
    MCPWMx->TMR3 = MCPWM_InitStruct->TriggerPoint3;

    MCPWMx->FLT = (u32)(MCPWM_InitStruct->CMP_BKIN_Filter << 8) | MCPWM_InitStruct->GPIO_BKIN_Filter;

    MCPWMx->IO01 = MCPWM_InitStruct->CH0N_Polarity_INV | (u32)(MCPWM_InitStruct->CH0P_Polarity_INV << 1) | (u32)(MCPWM_InitStruct->Switch_CH0N_CH0P << 6) | (u32)(MCPWM_InitStruct->MCPWM_WorkModeCH0 << 7) | (u32)(MCPWM_InitStruct->CH1N_Polarity_INV << 8) | (u32)(MCPWM_InitStruct->CH1P_Polarity_INV << 9) | (u32)(MCPWM_InitStruct->Switch_CH1N_CH1P << 14) | (u32)(MCPWM_InitStruct->MCPWM_WorkModeCH1 << 15);

    MCPWMx->IO23 = (u32)MCPWM_InitStruct->CH2N_Polarity_INV | (u32)(MCPWM_InitStruct->CH2P_Polarity_INV << 1) | (u32)(MCPWM_InitStruct->Switch_CH2N_CH2P << 6) | (u32)(MCPWM_InitStruct->MCPWM_WorkModeCH2 << 7) | (u32)(MCPWM_InitStruct->CH3N_Polarity_INV << 8) | (u32)(MCPWM_InitStruct->CH3P_Polarity_INV << 9) | (u32)(MCPWM_InitStruct->Switch_CH3N_CH3P << 14) | (u32)(MCPWM_InitStruct->MCPWM_WorkModeCH3 << 15);

    MCPWMx->FAIL012 = MCPWM_InitStruct->FAIL0_Signal_Sel | (u32)(MCPWM_InitStruct->FAIL0_Polarity << 2) | (u32)(MCPWM_InitStruct->FAIL0_INPUT_EN << 4) | (u32)(MCPWM_InitStruct->FAIL1_INPUT_EN << 5) | (u32)(MCPWM_InitStruct->FAIL1_Signal_Sel << 1) | (u32)(MCPWM_InitStruct->FAIL1_Polarity << 3) | (u32)(MCPWM_InitStruct->DebugMode_PWM_out << 7) | (u32)(MCPWM_InitStruct->CH0P_default_output << 8) | (u32)(MCPWM_InitStruct->CH0N_default_output << 9) | (u32)(MCPWM_InitStruct->CH1P_default_output << 10) | (u32)(MCPWM_InitStruct->CH1N_default_output << 11) | (u32)(MCPWM_InitStruct->CH2P_default_output << 12) | (u32)(MCPWM_InitStruct->CH2N_default_output << 13);

    MCPWMx->FAIL3 = (u32)MCPWM_InitStruct->FAIL2_Signal_Sel | (u32)(MCPWM_InitStruct->FAIL2_Polarity << 2) | (u32)(MCPWM_InitStruct->FAIL2_INPUT_EN << 4) | (u32)(MCPWM_InitStruct->FAIL3_INPUT_EN << 5) | (u32)(MCPWM_InitStruct->FAIL3_Signal_Sel << 1) | (u32)(MCPWM_InitStruct->FAIL3_Polarity << 3) | (u32)(MCPWM_InitStruct->DebugMode_PWM_out << 7) | (u32)(MCPWM_InitStruct->CH3P_default_output << 8) | (u32)(MCPWM_InitStruct->CH3N_default_output << 9);

    MCPWMx->SDCFG = (u32)MCPWM_InitStruct->MCPWM_UpdateInterval | (u32)(MCPWM_InitStruct->MCPWM_Base0T0_UpdateEN << 4) | (u32)(MCPWM_InitStruct->MCPWM_Base0T1_UpdateEN << 5) | (u32)(MCPWM_InitStruct->MCPWM_Auto_ERR_EN << 6) | (u32)(MCPWM_InitStruct->MCPWM_Base1T0_UpdateEN << 12) | (u32)(MCPWM_InitStruct->MCPWM_Base1T1_UpdateEN << 13) | (u32)(MCPWM_InitStruct->MCPWM_Auto_ERR_EN1 << 14) | (u32)(MCPWM_InitStruct->MCPWM_UpdateInterval1 << 8);

    MCPWMx->DTH00 = MCPWM_InitStruct->DeadTimeCH0N;
    MCPWMx->DTH01 = MCPWM_InitStruct->DeadTimeCH0P;
    MCPWMx->DTH10 = MCPWM_InitStruct->DeadTimeCH1N;
    MCPWMx->DTH11 = MCPWM_InitStruct->DeadTimeCH1P;
    MCPWMx->DTH20 = MCPWM_InitStruct->DeadTimeCH2N;
    MCPWMx->DTH21 = MCPWM_InitStruct->DeadTimeCH2P;
    MCPWMx->DTH30 = MCPWM_InitStruct->DeadTimeCH3N;
    MCPWMx->DTH31 = MCPWM_InitStruct->DeadTimeCH3P;

    MCPWMx->AUEN = MCPWM_InitStruct->AUEN;

    MCPWMx->EVT0 = MCPWM_InitStruct->TimeBase_TrigEvt0;
    MCPWMx->EVT1 = MCPWM_InitStruct->TimeBase_TrigEvt1;

    MCPWMx->CNT0 = (u32)MCPWM_InitStruct->TimeBase0Init_CNT;
    MCPWMx->CNT1 = (u32)MCPWM_InitStruct->TimeBase1Init_CNT;

    MCPWMx->EIF = 0xffff;
    MCPWMx->IE0 = MCPWM_InitStruct->T0_Update_INT_EN_BASE0 | (u32)(MCPWM_InitStruct->T1_Update_INT_EN_BASE0 << 1) | (u32)(MCPWM_InitStruct->TMR0_Match_INT_EN_BASE0 << 10) | (u32)(MCPWM_InitStruct->TMR1_Match_INT_EN_BASE0 << 11) | (u32)(MCPWM_InitStruct->TMR0_Match_INT_EN_BASE0 << 12) | (u32)(MCPWM_InitStruct->TMR1_Match_INT_EN_BASE0 << 13);
    MCPWMx->IE1 = MCPWM_InitStruct->T0_Update_INT_EN_BASE1 | (u32)(MCPWM_InitStruct->T1_Update_INT_EN_BASE1 << 1) | (u32)(MCPWM_InitStruct->TMR0_Match_INT_EN_BASE1 << 10) | (u32)(MCPWM_InitStruct->TMR1_Match_INT_EN_BASE1 << 11) | (u32)(MCPWM_InitStruct->TMR0_Match_INT_EN_BASE1 << 12) | (u32)(MCPWM_InitStruct->TMR1_Match_INT_EN_BASE1 << 13);

    MCPWMx->EIE = (u32)(MCPWM_InitStruct->FAIL0_INT_EN << 4) | (u32)(MCPWM_InitStruct->FAIL1_INT_EN << 5) | (u32)(MCPWM_InitStruct->FAIL2_INT_EN << 6) | (u32)(MCPWM_InitStruct->FAIL3_INT_EN << 7);

    MCPWMx->UPDATE = 0xffff; // write whatever value to trigger update

    //    MCPWMx->MCPWM_UPDATE = 0x00;

    MCPWMx->IF0 = 0xffff;
    MCPWMx->IF1 = 0xffff;
    MCPWMx->EIF = 0xffff;

    MCPWMx->PRT = 0x0000CAFE; // write any value other than 0xDEAD to enable write protection
}

/**
 * @brief    清除指定的霍尔中断
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
void MCPWM_ClearIRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x, u8 MCPWM_CNT_x)
{
    if (MCPWM_CNT_x == MCPWM_CNT_0)
    {
        MCPWMx->IF0 = MCPWM_IF_x;
        if (MCPWM_IF_x & MCPWM_IF_FAIL0)
        {
            MCPWMx->EIF = MCPWM_EIF_FAIL0;
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL1)
        {
            MCPWMx->EIF = MCPWM_EIF_FAIL1;
        }
    }
    else if (MCPWM_CNT_x == MCPWM_CNT_1)
    {
        MCPWMx->IF1 = MCPWM_IF_x;
        if (MCPWM_IF_x & MCPWM_IF_FAIL2)
        {
            MCPWMx->EIF = MCPWM_EIF_FAIL2;
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL3)
        {
            MCPWMx->EIF = MCPWM_EIF_FAIL3;
        }
    }
}
/**
 * @brief    获取HALL中断状态
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
u8 MCPWM_GetIRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x, u8 MCPWM_CNT_x)
{
    u8 IRQFlag;
    IRQFlag = 0;
    if (MCPWM_CNT_x == MCPWM_CNT_0)
    {
        if (MCPWMx->IF0 & MCPWMx->IE0 & MCPWM_IF_x)
        {
            IRQFlag = 1;
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL0)
        {
            if (MCPWMx->EIF & MCPWMx->EIE & MCPWM_EIF_FAIL0)
            {
                IRQFlag = 1;
            }
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL1)
        {
            if (MCPWMx->EIF & MCPWMx->EIE & MCPWM_EIF_FAIL1)
            {
                IRQFlag = 1;
            }
        }
    }
    else if (MCPWM_CNT_x == MCPWM_CNT_1)
    {
        if (MCPWMx->IF1 & MCPWMx->IE1 & MCPWM_IF_x)
        {
            IRQFlag = 1;
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL2)
        {
            if (MCPWMx->EIF & MCPWMx->EIE & MCPWM_EIF_FAIL2)
            {
                IRQFlag = 1;
            }
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL3)
        {
            if (MCPWMx->EIF & MCPWMx->EIE & MCPWM_EIF_FAIL3)
            {
                IRQFlag = 1;
            }
        }
    }
    return IRQFlag;
}

/**
 * @brief    获取HALL标志位
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
u8 MCPWM_GetFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x, u8 MCPWM_CNT_x)
{
    u8 IRQFlag;
    IRQFlag = 0;
    if (MCPWM_CNT_x == MCPWM_CNT_0)
    {
        if (MCPWMx->IF0 & MCPWM_IF_x)
        {
            IRQFlag = 1;
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL0)
        {
            if (MCPWM0_EIF & MCPWM_EIF_FAIL0)
            {
                IRQFlag = 1;
            }
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL1)
        {
            if (MCPWM0_EIF & MCPWM_EIF_FAIL1)
            {
                IRQFlag = 1;
            }
        }
    }
    else if (MCPWM_CNT_x == MCPWM_CNT_1)
    {
        if (MCPWMx->IF1 & MCPWM_IF_x)
        {
            IRQFlag = 1;
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL2)
        {
            if (MCPWM0_EIF & MCPWM_EIF_FAIL2)
            {
                IRQFlag = 1;
            }
        }
        if (MCPWM_IF_x & MCPWM_IF_FAIL3)
        {
            if (MCPWM0_EIF & MCPWM_EIF_FAIL3)
            {
                IRQFlag = 1;
            }
        }
    }
    return IRQFlag;
}
/**
 * @brief    获取HALL标志位
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
u8 MCPWM_GetCnt0Flag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x)
{
    return MCPWM_GetFlag(MCPWMx, MCPWM_IF_x, MCPWM_CNT_0);
}
/**
 * @brief    获取HALL标志位
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
u8 MCPWM_GetCnt1Flag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x)
{
    return MCPWM_GetFlag(MCPWMx, MCPWM_IF_x, MCPWM_CNT_1);
}

/**
 * @brief    获取HALL中断状态
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
u8 MCPWM_GetCnt0IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x)
{
    return MCPWM_GetIRQFlag(MCPWMx, MCPWM_IF_x, MCPWM_CNT_0);
}
/**
 * @brief    获取中断状态
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
u8 MCPWM_GetCnt1IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x)
{
    return MCPWM_GetIRQFlag(MCPWMx, MCPWM_IF_x, MCPWM_CNT_1);
}
/**
 * @brief    清除指定的中断
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
void MCPWM_ClearCnt0IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x)
{
    MCPWM_ClearIRQFlag(MCPWMx, MCPWM_IF_x, MCPWM_CNT_0);
}
/**
 * @brief    清除指定的霍尔中断
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void MCPWM_ClearCnt1IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x)
{
    MCPWM_ClearIRQFlag(MCPWMx, MCPWM_IF_x, MCPWM_CNT_1);
}

/**
 * @brief    MCPWM使能
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void MCPWM_Enable(MCPWM_TypeDef *MCPWMx)
{
    if (MCPWMx == MCPWM0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_MCPWM0, ENABLE);
    }
    else if (MCPWMx == MCPWM1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_MCPWM1, ENABLE);
    }
}
/**
 * @brief    MCPWM使能
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void MCPWM_Disable(MCPWM_TypeDef *MCPWMx)
{
    if (MCPWMx == MCPWM0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_MCPWM0, DISABLE);
    }
    else if (MCPWMx == MCPWM1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_MCPWM1, DISABLE);
    }
}
/**
 * @brief    MCPWM使能
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void MCPWM_OutputMode(MCPWM_TypeDef *MCPWMx, u8 MCPWM_OUT_CHN_x, u8 MCPWM_OUT_MODE_x)
{
    if (MCPWM_OUT_CHN_x & MCPWM_OUT_CHN_012)
    {
        MCPWMx->PRT = 0x0000DEAD;

        if (MCPWM_OUT_MODE_x == ENABLE)
        {
            MCPWMx->FAIL012 |= MCPWM_MOE_ENABLE_MASK;
        }
        else
        {
            MCPWMx->FAIL012 &= MCPWM_MOE_DISABLE_MASK;
        }
        MCPWMx->PRT = 0x0000CAFE;
    }
    if (MCPWM_OUT_CHN_x & MCPWM_OUT_CHN_3)
    {
        MCPWMx->PRT = 0x0000DEAD;

        if (MCPWM_OUT_MODE_x == ENABLE)
        {
            MCPWMx->FAIL3 |= MCPWM_MOE_ENABLE_MASK;
        }
        else
        {
            MCPWMx->FAIL3 &= MCPWM_MOE_DISABLE_MASK;
        }
        MCPWMx->PRT = 0x0000CAFE;
    }
}

/*! @} */
