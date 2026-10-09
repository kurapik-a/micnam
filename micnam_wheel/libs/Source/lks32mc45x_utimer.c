/**
 * @file
 * @defgroup utimer
 * @brief        Utimer库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_utimer.h"
#include "lks32mc45x.h"
#include "lks32mc45x_sys.h"
#include "string.h"

/**
 * @brief    Timer初始化
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
void UTIMER_Init(UTIMER_TypeDef *TIMERx, UTIMER_InitTypeDef *this)
{
    if (TIMERx == UTIMER0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER0, this->EN);
    }
    else if (TIMERx == UTIMER1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER1, this->EN);
    }
    else if (TIMERx == UTIMER2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER2, this->EN);
    }
    else if (TIMERx == UTIMER3)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER3, this->EN);
    }
    else if (TIMERx == UTIMER4)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER4, this->EN);
    }

    TIMERx->CFG = (u32)(this->EN << 31) |
                  (u32)(this->CAP1_CLR_EN << 27) |
                  (u32)(this->CAP0_CLR_EN << 26) |
                  (u32)(this->ONE_TRIG << 25) |
                  (u32)(this->ETON << 24) |
                  (u32)(this->CLK_DIV << 20) |
                  (u32)(this->CLK_SRC << 16) |
                  (u32)(this->SRC1 << 12) |
                  (u32)(this->CH1_POL << 11) |
                  (u32)(this->CH1_MODE << 10) |
                  (u32)(this->CH1_FE_CAP_EN << 9) |
                  (u32)(this->CH1_RE_CAP_EN << 8) |
                  (u32)(this->SRC0 << 4) |
                  (u32)(this->CH0_POL << 3) |
                  (u32)(this->CH0_MODE << 2) |
                  (u32)(this->CH0_FE_CAP_EN << 1) |
                  (u32)(this->CH0_RE_CAP_EN << 0);
    TIMERx->TH = this->TH;
    TIMERx->CMP0 = this->CMP0;
    TIMERx->CMP1 = this->CMP1;
    TIMERx->EVT = this->EVT_SRC;
    TIMERx->FLT = this->FLT;
    TIMERx->IE = this->IE |
                 (u32)(this->RE << 8);

    TIMERx->CNT = 0;
    TIMERx->IF = 7;
}

/**
 * @brief    Timer结构体初始化
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
void UTIMER_StructInit(UTIMER_InitTypeDef *TIM_TimerInitStruct)
{
    memset(TIM_TimerInitStruct, 0, sizeof(UTIMER_InitTypeDef));
}

/**
 * @brief    获取UTIMER中断位
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
u16 UTIMER_GetIRQFlag(UTIMER_TypeDef *UTIMERx, u32 TIMER_IF_x)
{
    if (UTIMERx->IF & UTIMERx->IE & TIMER_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    获取UTIMER中断位
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
u16 UTIMER_GetFlag(UTIMER_TypeDef *UTIMERx, u32 TIMER_IF_x)
{
    if (UTIMERx->IF & TIMER_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    清楚UTIMER中断标志
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
void UTIMER_ClearIRQFlag(UTIMER_TypeDef *UTIMERx, u32 TIMER_IF_x)
{
    UTIMERx->IF = TIMER_IF_x;
}
/**
 * @brief    读取UTIMERx当前计数值
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
u32 UTIMER_GetCount(UTIMER_TypeDef *UTIMERx)
{
    return UTIMERx->CNT;
}
/**
 * @brief    读取UTIMERx通道0捕获值
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
u32 UTIMER_GetCMPT0(UTIMER_TypeDef *UTIMERx)
{
    return UTIMERx->CMP0;
}
/**
 * @brief    读取UTIMERx通道1捕获值
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
u32 UTIMER_GetCMPT1(UTIMER_TypeDef *UTIMERx)
{
    return UTIMERx->CMP1;
}
/**
 * @brief    使用timer进行指定时间的延迟
 * @param UTIMERx
              xUs
 * @note lks32mc45x芯片中，192Mhz时钟频率下，Utimer0、Utimer1和Utimer4最大43.69mS
                                                 Utimer2和Utimer3最大 2863.26S
              其他频率按照比例放大，例如96Mhz时钟下Utimer0最大87.38ms
 * @par 更新记录  V1.0  2022/05/31  YangZJ  创建
*/
void UTIMER_DelayUs(UTIMER_TypeDef *UTIMERx, u32 xUs)
{
    UTIMER_InitTypeDef tim_config;
    u64 mclk;
    u64 cnt;
    u8 div;
    mclk = SYS_ReadMcuClk();
    if (xUs == 0)
        xUs = 1;
    cnt = xUs * ((mclk + 500000) / 1000000);

    if ((UTIMERx == UTIMER0) || (UTIMERx == UTIMER1) || (UTIMERx == UTIMER4))
    {
        if (cnt > 0xffff)
        {
            div = 7;
            cnt = cnt >> 7;
            cnt = (cnt > 0xffff) ? 0xffff : cnt;
        }
        else
        {
            div = 0;
        }
    }
    else
    {
        if (cnt > 0xffffffff)
        {
            div = 7;
            cnt = cnt >> 7;
            cnt = (cnt > 0xffffffff) ? 0xffffffff : cnt;
        }
        else
        {
            div = 0;
        }
    }
    UTIMER_StructInit(&tim_config);
    tim_config.EN = ENABLE;                   // Timer 模块整体使能，高有效
    tim_config.CAP1_CLR_EN = DISABLE;         // 当发生CAP1捕获事件时，清零Timer计数器，高有效
    tim_config.CAP0_CLR_EN = DISABLE;         // 当发生CAP0捕获事件时，清零Timer计数器，高有效
    tim_config.ETON = 0;                      // Timer 计数器计数使能配置 0:自动运行 1:等待外部事件触发计数
    tim_config.CLK_DIV = div;                 // Timer 计数器分频设置
    tim_config.CLK_SRC = UTIMER_CLK_SRC_MCLK; // Timer 时钟源
    tim_config.TH = (u32)cnt;                 // Timer 计数器计数门限。
    tim_config.IE = 0;                        // 中断配置
    tim_config.RE = 0;                        // DMA请求
    UTIMER_Init(UTIMERx, &tim_config);
    while ((UTIMERx->IF & UTIMER_IF_ZERO) == 0)
        ;
    UTIMER_ClearIRQFlag(UTIMERx, UTIMER_IF_ZERO);
}

/**
 * @brief    MCPWM使能
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void UTIMER_Enable(UTIMER_TypeDef *UTIMERx)
{
    if (UTIMERx == UTIMER0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER0, ENABLE);
    }
    else if (UTIMERx == UTIMER1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER1, ENABLE);
    }
    else if (UTIMERx == UTIMER2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER2, ENABLE);
    }
    else if (UTIMERx == UTIMER3)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER3, ENABLE);
    }
    else if (UTIMERx == UTIMER4)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER4, ENABLE);
    }
}
/**
 * @brief    MCPWM使能
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void UTIMER_Disable(UTIMER_TypeDef *UTIMERx)
{
    if (UTIMERx == UTIMER0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER0, DISABLE);
    }
    else if (UTIMERx == UTIMER1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER1, DISABLE);
    }
    else if (UTIMERx == UTIMER2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER2, DISABLE);
    }
    else if (UTIMERx == UTIMER3)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER3, DISABLE);
    }
    else if (UTIMERx == UTIMER4)
    {
        SYS_ModuleClockCmd(SYS_MODULE_TIMER4, DISABLE);
    }
}

/*! @} */
