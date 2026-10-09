/**
 * @file
 * @defgroup wwdg
 * @brief        窗看门狗库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_wwdg.h"
#include "lks32mc45x_sys.h"
#include "string.h"

/**
 * @brief    WWDG初始化
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
void WWDG_Init(WWDG_TypeDef *WWDGx, WWDG_InitTypeDef *this)
{
    WWDGx->CFG = 0x3c00 | (this->WDGTB << 8) | (this->EWI << 7) | (this->W << 0);
    WWDGx->CR = (this->WDG_EN << 7) | (this->T << 0);
    WWDGx->IF = 1;
}
/**
 * @brief    QEP初始化
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
void WWDG_StructInit(WWDG_InitTypeDef *this)
{
    memset(this, 0, sizeof(WWDG_InitTypeDef));
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void WWDG_ClearIRQFlag(u16 WWDG_IF_x)
{
    WWDG0->IF = WWDG_IF_x;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
u8 WWDG_GetIRQFlag(u16 WWDG_IF_x)
{
    u8 val = 0;
    if (WWDG_IF_x & WWDG_IF_EW)
    {
        if (WWDG0->CFG & WWDG_IE_EW)
        {
            if (WWDG0->IF & WWDG_IF_EW)
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
u8 WWDG_GetFlag(u16 WWDG_IF_x)
{
    if (WWDG0->IF & WWDG_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void WWDG_Enable(void)
{
    WWDG0->CR |= BIT7;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void WWDG_Disable(void)
{
    WWDG0->CR = (WWDG0->CR & 0x7f) | 0x3c00;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void WWDG_FeedDog(u16 val)
{
    WWDG0->CR = val;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  YangZJ  创建
 */
void WWDG_SetResetCnt(u16 val)
{
    WWDG0->CR = val;
}

/*! @} */
