/**
 * @file
 * @defgroup spi
 * @brief        SPI库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_spi.h"
#include "lks32mc45x_sys.h"
/**
 * @brief    SPI初始化
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *this)
{
    if (SPIx == SPI0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_SPI0, this->EN);
        SYS_SoftResetMODULE(SYS_MODULE_SPI0);
    }
    else
    {
        SYS_ModuleClockCmd(SYS_MODULE_SPI1, this->EN);
        SYS_SoftResetMODULE(SYS_MODULE_SPI1);
    }

    SPIx->IE = (u32)(this->Trig << 3) | (u32)(this->IRQEna);
    SPIx->BAUD = (u32)this->BaudRate | (u32)(this->TRANS_MODE << 15);
    SPIx->SIZE = this->ByteLength;
    SPIx->CFG = this->EN | (u32)(this->DataOrder << 1) | (u32)(this->CPOL << 2) | (u32)(this->CPHA << 3) | (u32)(this->Mode << 4) | (u32)(this->CS << 5) | (u32)(this->Duplex << 6);
}

/**
 * @brief    SPI结构体初始化
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
void SPI_StructInit(SPI_InitTypeDef *SPI_InitStruct)
{

    SPI_InitStruct->Duplex = SPI_Full;
    SPI_InitStruct->CS = 0;
    SPI_InitStruct->Mode = SPI_Slave;
    SPI_InitStruct->CPOL = 0;
    SPI_InitStruct->CPHA = 0;
    SPI_InitStruct->DataOrder = DISABLE;
    SPI_InitStruct->EN = 0;

    SPI_InitStruct->IRQEna = DISABLE;
    SPI_InitStruct->Trig = 0;

    SPI_InitStruct->TRANS_MODE = 0;
    SPI_InitStruct->BaudRate = 3;

    SPI_InitStruct->ByteLength = 0;
}

/**
 * @brief    SPI发送数据
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
void SPI_SendData(SPI_TypeDef *SPIx, u8 n)
{
    SPIx->TX_DATA = n;
}

/**
 * @brief    SPI读缓冲区数据
 * @param SPI_TypeDef *SPIx
 * @return 缓冲区数据
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
u8 SPI_ReadData(SPI_TypeDef *SPIx)
{
    return (u8)SPIx->RX_DATA;
}

/**
 * @brief    取得SPI中断标志
 * @param SPI_InitTypeDef *SPIx:要操作的SPI对象
 * @return SPI中断标志
 * @par 更新记录  V1.0  2022/04/20  YangZJ  创建
 */
u8 SPI_GetIRQFlag(SPI_TypeDef *SPIx, u8 SPI_IF_x)
{
    u8 SPI_IRQFlag = 0;
    u8 ie;
    ie = (u8)(SPIx->IE >> 4);
    if (SPIx->IE & BIT7)
    {
        if (ie & SPI_IF_x)
        {
            SPI_IRQFlag = 1;
        }
        else
        {
            SPI_IRQFlag = 0;
        }
    }
    else
    {
        SPI_IRQFlag = 0;
    }
    return SPI_IRQFlag;
}

/**
 * @brief    SPI中断标志位清零
 * @note 写1清零
 * @par 更新记录  V1.0  2022/04/20  YangZJ  创建
 */
void SPI_ClearIRQFlag(SPI_TypeDef *SPIx, u8 SPI_IF_x)
{
    SPIx->IE = (SPIx->IE & 0xf8) | (SPI_IF_x & 0x7);
}
/**
 * @brief    SPI中断标志位清零
 * @note 写1清零
 * @par 更新记录  V1.0  2022/04/20  YangZJ  创建
 */
u8 SPI_GetFlag(SPI_TypeDef *SPIx, u8 SPI_IF_x)
{
    u8 SPI_IRQFlag = 0;
    if ((SPIx->IE & SPI_IF_x) & 0x7)
    {
        SPI_IRQFlag = 1;
    }
    else
    {
        SPI_IRQFlag = 0;
    }
    return SPI_IRQFlag;
}

/**
 * @brief    SPI模块使能
 * @param SPI_TypeDef *SPIx
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
void SPI_Enable(SPI_TypeDef *SPIx)
{
    if (SPIx == SPI0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_SPI0, ENABLE);
    }
    else if (SPIx == SPI1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_SPI1, ENABLE);
    }
}
/**
 * @brief    SPI模块关闭
 * @param SPI_TypeDef *SPIx
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
void SPI_Disable(SPI_TypeDef *SPIx)
{
    if (SPIx == SPI0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_SPI0, DISABLE);
    }
    else if (SPIx == SPI1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_SPI1, DISABLE);
    }
}

/**
 * @brief    设置SPI模块的波特率
 * @param BaudRate 波特率
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
void SPI_SetBaudRate(SPI_TypeDef *SPIx, u32 BaudRate)
{
    u32 mclk;
    u32 div;
    mclk = SYS_ReadMcuClk();
    div = mclk / (2 * BaudRate) - 1;
    if (div < 3)
    {
        div = 3;
    }
    if (div > 0xfff)
    {
        div = 0xfff;
    }
    SPIx->BAUD = (SPIx->BAUD & BIT15) | div;
}

/*! @} */
