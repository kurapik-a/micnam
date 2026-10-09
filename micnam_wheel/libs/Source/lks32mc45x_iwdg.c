/**
 * @file
 * @defgroup iwdg
 * @brief        独立看门狗库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_iwdg.h"
#include "lks32mc45x.h"
#include "basic.h"

/**
 * @brief    看门狗初始化
 * @param this 看门狗配置结构体
 * @note 休眠唤醒时间需要小于复位时间，否则休眠唤醒不生效
 * @par 更新记录  V1.0  2021/11/10  YangZJ  创建
 */
void IWDG_Init(IWDG_InitTypeDef *this)
{
    this->RTH = (this->RTH > BIT21) ? BIT21 - 1 : this->RTH;
    this->WTH = (this->WTH > BIT21) ? BIT21 - 1 : this->WTH;
    IWDG->CFG = (this->DWK_EN << 4) | (this->WDG_EN);
    IWDG->PSW = PSW_IWDG_PRE;
    IWDG->RTH = this->RTH;
    if (this->RTH > this->WTH)
    {
        IWDG->WTH = this->RTH - this->WTH;
    }
    else
    {
        IWDG->WTH = 0x001000;
    }
    IWDG->PSW = PSW_IWDG_PRE;
    IWDG->CLR = PSW_IWDG_CLR;
}
/**
 * @brief    看门狗配置结构体初始化
 * @param this看门狗配置结构体指针
 * @par 更新记录  V1.0  2021/11/09  YangZJ  创建
 */
void IWDG_StrutInit(IWDG_InitTypeDef *this)
{
    this->DWK_EN = DISABLE; // 深度休眠定时唤醒使能
    this->WDG_EN = DISABLE; // 独立看门狗使能
    this->WTH = 0x10000;    // 看门狗定时唤醒时间,大于复位时间后无效
    this->RTH = 0x10000;    // 看门狗超时复位时间（21位计数器，但低12恒位0）
                            // SECOND2IWDGCNT输入参数，秒
}
/**
 * @brief    关闭看门狗
 * @par 更新记录  V1.0  2021/11/09  YangZJ  创建
 */
void IWDG_Enable(void)
{
    IWDG->CFG = 0x1;
}
/**
 * @brief    关闭看门狗
 * @par 更新记录  V1.0  2021/11/09  YangZJ  创建
 */
void IWDG_Disable(void)
{
    IWDG->CFG = 0x3c00;
}
/**
 * @brief    看门狗喂狗
 * @par 更新记录  V1.0  2021/11/09  YangZJ  创建
 */
void IWDG_FeedDog(void)
{
    IWDG->PSW = PSW_IWDG_PRE;
    IWDG->CLR = PSW_IWDG_CLR;
}

/**
 * @brief
 * @par 更新记录  V1.0  2022/09/21  Yangzj  创建
 */
void IWDG_SetResetCnt(u32 cnt)
{
    cnt = (cnt > 0x1ffff) ? 0x1ffff : cnt;
    cnt = (cnt < 0x1000) ? 0x1000 : cnt;
    IWDG->RTH = cnt;
}

/*! @} */
