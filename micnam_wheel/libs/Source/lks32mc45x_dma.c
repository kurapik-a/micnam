/**
 * @file
 * @defgroup dma
 * @brief        DMA库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_dma.h"
#include "lks32mc45x_sys.h"
#include "string.h"

/**
 * @brief    DMA结构体初始化
 * @par 更新记录  V1.0  2021.10.14  YangZJ  创建
 */
void DMA_StructInit(DMA_InitTypeDef *DMAInitStruct)
{
    memset(DMAInitStruct, 0, sizeof(DMA_InitTypeDef));
}

/**
 * @brief    DMA初始化
 * @par 更新记录  V1.0  2021.10.14  YangZJ  创建
 */
void DMA_Init(DMA_RegTypeDef *DMAx, DMA_InitTypeDef *DMAInitStruct)
{
    DMA_CTRL = 0x0001;
    // 通道配置寄存器 斌值
    DMAx->CCR = 0;
    DMAx->CTMS = DMAInitStruct->DMA_TIMES; // 传输次数寄存器
    DMAx->SADR = DMAInitStruct->DMA_SADR;  // 传输次数寄存器
    DMAx->DADR = DMAInitStruct->DMA_DADR;  // 传输次数寄存器
    DMAx->REN = DMAInitStruct->DMA_REQ_EN;
    DMAx->CCR = (u32)(DMAInitStruct->DMA_SBTW << 10) | (u32)(DMAInitStruct->DMA_DBTW << 8) |
                (u32)(DMAInitStruct->DMA_SINC << 6) | (u32)(DMAInitStruct->DMA_DINC << 4) |
                (u32)(DMAInitStruct->DMA_CIRC << 3) | (u32)(DMAInitStruct->DMA_RMODE << 1) |
                (u32)(DMAInitStruct->DMA_Channel_EN);
    if (DMAInitStruct->DMA_IRQ_EN)
    {
        if(DMAx == DMA_CH0)
        {
            DMA_IE |= BIT0;
        }
        if(DMAx == DMA_CH1)
        {
            DMA_IE |= BIT1;
        }
        if(DMAx == DMA_CH2)
        {
            DMA_IE |= BIT2;
        }
        if(DMAx == DMA_CH3)
        {
            DMA_IE |= BIT3;
        }
    }
    else
    {
        if(DMAx == DMA_CH0)
        {
            DMA_IE &= ~BIT0;
        }
        if(DMAx == DMA_CH1)
        {
            DMA_IE &= ~BIT1;
        }
        if(DMAx == DMA_CH2)
        {
            DMA_IE &= ~BIT2;
        }
        if(DMAx == DMA_CH3)
        {
            DMA_IE &= ~BIT3;
        }
    }
    DMA_CTRL = 0x0001; /*enable dma, mcu has higher priorit */
}

/**
 * @brief    DMA中断标志
 * @return DMA中断标志
 * @par 更新记录  V1.0  2022/4/13  YangZJ  创建
 */
u32 DMA_GetIRQFlag(u32 DMA_IF_x)
{
    if (DMA_IF & DMA_IF_x & DMA_IE)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief    清除DMA中断标志
 * @param DMA中断标志
 * @par 更新记录  V1.0  2022/4/13  YangZJ  创建
 */
void DMA_ClearIRQFlag(u32 DMA_IF_x)
{
    DMA_IF = DMA_IF_x;
}

/**
 * @brief
 * @return DMA中断标志
 * @par 更新记录  V1.0  2022/9/21  YangZJ  创建
 */
u32 DMA_GetFlag(u32 DMA_IF_x)
{
    if (DMA_IF & DMA_IF_x)
    {
        return 1;
    }
    return 0;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void DMA_Enable(void)
{
    DMA->CTRL = 1;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void DMA_Disable(void)
{
    DMA->CTRL = 0;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void DMA_SetAddr(DMA_RegTypeDef *DMA_CHx, u32 addr1, u32 addr2)
{
    DMA_CHx->CCR = DMA_CHx->CCR & ~BIT0;
    DMA_CHx->SADR = addr1;
    DMA_CHx->DADR = addr2;
    DMA_CHx->CCR = DMA_CHx->CCR | BIT0;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void DMA_SetLength(DMA_RegTypeDef *DMA_CHx, u32 len)
{
    DMA_CHx->CCR = DMA_CHx->CCR & ~BIT0;
    DMA_CHx->CTMS = len;
    DMA_CHx->CCR = DMA_CHx->CCR | BIT0;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void DMA_Trigger(DMA_RegTypeDef *DMA_CHx)
{
    DMA_CHx->REN |= BIT31;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void DMA_SetRequest(DMA_RegTypeDef *DMA_CHx, u32 DMA_REQ_x)
{
    DMA_CHx->REN = DMA_REQ_x;
}

/*! @} */
