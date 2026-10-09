/**
 * @file
 * @defgroup qep
 * @brief        编码器库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_qep.h"
#include "lks32mc45x.h"
#include "string.h"
/**
 * @brief    QEP初始化
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
void QEP_Init(QEP_TypeDef *QEPx, QEP_InitTypeDef *this)
{
    switch ((u32)QEPx)
    {
    case DMA_BASE + 0x00:
        SYS_ModuleClockCmd(SYS_MODULE_QEP0, this->EN);
        break;
    case DMA_BASE + 0x18:
        SYS_ModuleClockCmd(SYS_MODULE_QEP1, this->EN);
        break;
    case DMA_BASE + 0x30:
        SYS_ModuleClockCmd(SYS_MODULE_QEP2, this->EN);
        break;
    case DMA_BASE + 0x48:
        SYS_ModuleClockCmd(SYS_MODULE_QEP3, this->EN);
        break;
    }
    QEPx->CFG = (u32)(this->EN << 15) | (u32)(this->MODE << 8) | (u32)(this->ZC << 0);
    QEPx->TH = this->TH;
    QEPx->IE = this->IE;
    QEPx->CNT = 0;
    QEPx->IF = 3;
}
/**
 * @brief    QEP初始化
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
void QEP_StructInit(QEP_InitTypeDef *this)
{
    memset(this, 0, sizeof(QEP_InitTypeDef));
}
/**
 * @brief    QEP初始化
 * @par 更新记录  V1.0  2016/06/06  YangZJ  创建
 */
u32 QEP_GetCount(QEP_TypeDef *QEPx)
{
    return QEPx->CNT;
}
/**
 * @brief    QEP模块使能
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void QEP_Enable(QEP_TypeDef *QEPx)
{
    if (QEPx == QEP0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP0, ENABLE);
    }
    else if (QEPx == QEP1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP1, ENABLE);
    }
    else if (QEPx == QEP2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP2, ENABLE);
    }
    else if (QEPx == QEP3)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP3, ENABLE);
    }
}
/**
 * @brief    MCPWM使能
 * @par 更新记录  V1.0  2022/08/18  YangZJ  创建
 */
void QEP_Disable(QEP_TypeDef *QEPx)
{
    if (QEPx == QEP0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP0, DISABLE);
    }
    else if (QEPx == QEP1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP1, DISABLE);
    }
    else if (QEPx == QEP2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP2, DISABLE);
    }
    else if (QEPx == QEP3)
    {
        SYS_ModuleClockCmd(SYS_MODULE_QEP3, DISABLE);
    }
}

/**
 * @brief    ADC清除中断
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void QEP_ClearIRQFlag(QEP_TypeDef *QEPx, u16 QEP_IF_x)
{
    QEPx->IF = QEP_IF_x;
}
/**
 * @brief    判断ADC中断状态
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
u8 QEP_GetIRQFlag(QEP_TypeDef *QEPx, u16 QEP_IF_x)
{
    if (QEPx->IF & QEPx->IE & QEP_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    判断ADC中断状态
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
u8 QEP_GetFlag(QEP_TypeDef *QEPx, u16 QEP_IF_x)
{
    if (QEPx->IF & QEP_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*! @} */
