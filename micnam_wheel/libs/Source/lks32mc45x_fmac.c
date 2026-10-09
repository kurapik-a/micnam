/**
 * @file
 * @defgroup fmac
 * @brief        Fmac库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_fmac.h"

/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void FMAC_InitX1(FMAC_TypeDef *FMACx, u16 data[], u8 size)
{
    u32 i;
    FMACx->PARAM &= 0x00FFFFFF; // clear start and func
    FMACx->PARAM |= 0x01000000; // x1 init
    FMACx->PARAM |= 0x80000000; // start
    for (i = 0; i < size; i++)
    {
        FMACx->WDATA = data[i];
    }
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void FMAC_InitX2(FMAC_TypeDef *FMACx, u16 data[], u8 size)
{
    u32 i;
    FMACx->PARAM &= 0x00FFFFFF; // clear start and func
    FMACx->PARAM |= 0x02000000; // x2 init
    FMACx->PARAM |= 0x80000000; // start
    for (i = 0; i < size; i++)
    {
        FMACx->WDATA = data[i];
    }
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void FMAC_InitY(FMAC_TypeDef *FMACx, u16 data[], u8 size)
{
    u32 i;
    FMACx->PARAM &= 0x00FFFFFF; // clear start and func
    FMACx->PARAM |= 0x03000000; // x3 init
    FMACx->PARAM |= 0x80000000; // start
    for (i = 0; i < size; i++)
    {
        FMACx->WDATA = data[i];
    }
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void FMAC_ClearIRQFlag(u16 FMAC_IF_x)
{
    FMAC0->SR = FMAC_IF_x;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
u8 FMAC_GetIRQFlag(u16 FMAC_IF_x)
{
    u8 val = 0;
    if (FMAC_IF_x & FMAC_IF_R)
    {
        if (FMAC0->CR & FMAC_IE_R)
        {
            if (FMAC0->SR & FMAC_IF_R)
            {
                val = 1;
            }
        }
    }
    if (FMAC_IF_x & FMAC_IF_W)
    {
        if (FMAC0->CR & FMAC_IE_W)
        {
            if (FMAC0->SR & FMAC_IF_W)
            {
                val = 1;
            }
        }
    }
    if (FMAC_IF_x & FMAC_IF_OVFL)
    {
        if (FMAC0->CR & FMAC_IE_OVFL)
        {
            if (FMAC0->SR & FMAC_IF_OVFL)
            {
                val = 1;
            }
        }
    }
    if (FMAC_IF_x & FMAC_IF_UNFL)
    {
        if (FMAC0->CR & FMAC_IE_UNFL)
        {
            if (FMAC0->SR & FMAC_IF_UNFL)
            {
                val = 1;
            }
        }
    }
    if (FMAC_IF_x & FMAC_IF_SAT)
    {
        if (FMAC0->CR & FMAC_IE_SAT)
        {
            if (FMAC0->SR & FMAC_IF_SAT)
            {
                val = 1;
            }
        }
    }
    return val;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
u8 FMAC_GetFlag(u16 FMAC_IF_x)
{
    if (FMAC0->SR & FMAC_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*! @} */
