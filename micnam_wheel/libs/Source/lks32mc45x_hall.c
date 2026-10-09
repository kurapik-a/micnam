/**
 * @file
 * @defgroup hall
 * @brief        霍尔库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_hall.h"
#include "lks32mc45x_sys.h"
#include "lks32mc45x.h"

/**
 * @brief    HALL初始化
 * @par 更新记录  V1.0  2016/04/19  YangZJ  创建
 */
void HALL_Init(HALL_TypeDef *HALLx, HALL_InitTypeDef *HALL_InitStruct)
{
    if (HALLx == HALL0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_HALL0, ENABLE);
    }
    else
    {
        SYS_ModuleClockCmd(SYS_MODULE_HALL1, ENABLE);
    }

    HALLx->CFG = (u32)(HALL_InitStruct->FilterLen) | (u32)(HALL_InitStruct->ClockDivision << 16) | (u32)(HALL_InitStruct->Filter75_Ena << 20) | (u32)(HALL_InitStruct->HALL_Ena << 24) | (u32)(HALL_InitStruct->Capture_IRQ_Ena << 28) | (u32)(HALL_InitStruct->OverFlow_IRQ_Ena << 29) | (u32)(HALL_InitStruct->softIE << 30);
    HALLx->TH = HALL_InitStruct->CountTH;
    HALLx->INFO = 0;
}

/**
 * @brief    HALL结构体初始化
 * @par 更新记录  V1.0  2016/04/19  YangZJ  创建
 */
void HALL_StructInit(HALL_InitTypeDef *HALL_InitStruct)
{
    HALL_InitStruct->FilterLen = 1023;
    HALL_InitStruct->ClockDivision = HALL_CLK_DIV1;
    HALL_InitStruct->Filter75_Ena = ENABLE;
    HALL_InitStruct->HALL_Ena = ENABLE;
    HALL_InitStruct->Capture_IRQ_Ena = ENABLE;
    HALL_InitStruct->OverFlow_IRQ_Ena = DISABLE;
    HALL_InitStruct->CountTH = 1000;
    HALL_InitStruct->softIE = DISABLE;
}

/**
 * @brief    取得HALL值，滤波结果
 * @par 更新记录  V1.0  2016/04/19  YangZJ  创建
 */
u32 HALL_GetFilterValue(HALL_TypeDef *HALL)
{
    return (HALL->INFO & 0x07);
}

/**
 * @brief    取得HALL值，发生HALL事件时的锁存结果
 * @par 更新记录  V1.0  2016/04/19  YangZJ  创建
 */
u32 HALL_GetCaptureValue(HALL_TypeDef *HALL)
{
    return (HALL->INFO >> 8) & 0x07;
}

/**
 * @brief    取得HALL计数器值
 * @par 更新记录  V1.0  2016/04/19  YangZJ  创建
 */
u32 HALL_GetCount(HALL_TypeDef *HALL)
{
    return HALL->CNT;
}

/**
 * @brief    是否发生HALL信号变化捕获事件
 * @par 更新记录  V1.0  2016/04/19  YangZJ  创建
 */
u32 HALL_IsCaptureEvent(HALL_TypeDef *HALL)
{
    return (HALL->INFO & HALL_CAPTURE_EVENT);
}

/**
 * @brief    是否发生HALL计数器溢出事件
 * @par 更新记录  V1.0  2016/04/19  YangZJ  创建
 */
u32 HALL_IsOverFlowEvent(HALL_TypeDef *HALL)
{
    return (HALL->INFO & HALL_OVERFLOW_EVENT);
}
/**
 * @brief    清除指定的霍尔中断
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
void HALL_ClearIRQFlag(HALL_TypeDef *HALLx, u16 HALL_IF_x)
{
    HALLx->INFO = HALL_IF_x;
}
/**
 * @brief    获取HALL中断状态
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
u8 HALL_GetIRQFlag(HALL_TypeDef *HALLx, u16 HALL_IF_x)
{
    u8 IRQFlag;
    IRQFlag = 0;
    if (HALLx->CFG & HALL_IE_OV)
    {
        if (HALLx->INFO & HALL_IF_OV & HALL_IF_x)
        {
            IRQFlag = 1;
        }
    }
    if (HALLx->CFG & HALL_IE_CHG)
    {
        if (HALLx->INFO & HALL_IF_CHG & HALL_IF_x)
        {
            IRQFlag = 1;
        }
    }
    return IRQFlag;
}
/**
 * @brief    获取HALL标志位
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
u8 HALL_GetFlag(HALL_TypeDef *HALLx, u16 HALL_IF_x)
{
    if (HALLx->INFO & HALL_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    HALL使能
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
void HALL_Enable(HALL_TypeDef *HALLx)
{
    if (HALLx == HALL0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_HALL0, ENABLE);
    }
    else if (HALLx == HALL1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_HALL1, ENABLE);
    }
}
/**
 * @brief    HALL使能
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
void HALL_Disable(HALL_TypeDef *HALLx)
{
    if (HALLx == HALL0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_HALL0, DISABLE);
    }
    else if (HALLx == HALL1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_HALL1, DISABLE);
    }
}
/**
 * @brief    触发HALL变化中断
 * @par 更新记录  V1.0  2022/08/16  YangZJ  创建
 */
void HALL_Trigger(HALL_TypeDef *HALLx)
{
    HALLx->INFO = BIT18;
}

/*! @} */
