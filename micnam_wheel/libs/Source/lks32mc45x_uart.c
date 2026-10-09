/**
 * @file
 * @defgroup uart
 * @brief        串口库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_uart.h"
#include "lks32mc45x_sys.h"
#include "string.h"

/**
 * @brief    UART初始化
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *this)
{
    u32 DIVHDATA;
    u32 DIVLDATA;
    u64 mclk;
    uint32_t div2;
    // 如果设置了div2，则dma在接收数据时会错位，例如实际数据为{1,2,3,4}，dma实际收到的数据是{0,1,2,3}
    // 如果没有特殊需求，建议保持默认（默认值为0）
    div2 = SYS_CLK_DIV2 + 1;
    mclk = SYS_ReadMcuClk();
    DIVHDATA = (u32)(((mclk / div2) / this->BAUDRATE) / 256);
    DIVLDATA = (u32)((mclk / div2) / this->BAUDRATE) - (DIVHDATA * 256) - 1;

    if (UARTx == UART0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART0, ENABLE);
    }
    else if (UARTx == UART1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART1, ENABLE);
    }
    else if (UARTx == UART2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART2, ENABLE);
    }
    UARTx->CTRL = (u32)(this->DUPLEX << 7) |
                  (u32)(this->MD_EN << 5) | (u32)(this->CK_EN << 4) |
                  (u32)(this->CK_TYPE << 3) | (u32)(this->BIT_ORDER << 2) |
                  (u32)(this->STOP_LEN << 1) | (u32)(this->BYTE_LEN);                              // 0x00 UART 控制寄存器
    UARTx->DIVH = DIVHDATA;                                                                        // 0x04 UART 波特率设置高字节寄存器
    UARTx->DIVL = DIVLDATA;                                                                        // 0x08 UART 波特率设置低字节寄存器
    UARTx->ADR = this->ADR;                                                                        // 0x10 485 通信地址匹配寄存器
    UARTx->RE = (u32)(this->TX_BUF_EMPTY << 2) | (u32)(this->RX_DONE << 1) | (u32)(this->TX_DONE); // 0x18 UART DMA 请求使能寄存器
    UARTx->IE = this->IE;                                                                          // 0x1C UART 中断使能寄存器
    UARTx->IOC = (u32)(this->TXD_INV << 1) | (u32)(this->RXD_INV);                                 // 0x24 UART IO 控制
    UARTx->IF = 0x1ff;                                                                             // 0x20 UART 中断标志寄存器
}

/**
 * @brief    UART结构体初始化
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
void UART_StructInit(UART_InitTypeDef *this)
{
    this->DUPLEX = DISABLE;       // 半双工模式使能，tx_data
    this->MD_EN = DISABLE;        // Multi-drop 使能
    this->CK_EN = DISABLE;        // 数据校验使能
    this->CK_TYPE = 0;            // 奇偶校验配置       0:偶校验（EVEN）;1: 奇校验（ODD）
    this->BIT_ORDER = 0;          // 数据发送顺序配置   0:LSB;1:MSB
    this->STOP_LEN = 0;           // 停止位长度配置     0:1-Bit;1:2-Bit
    this->BYTE_LEN = 0;           // 数据长度配置       0:8-Bit;1:9-Bit
    this->BAUDRATE = 115200;      // 波特率
    this->ADR = 0;                // 多机通讯时的从机地址
    this->TX_BUF_EMPTY = DISABLE; // 发送缓冲区空 DMA 请求使能
    this->RX_DONE = DISABLE;      // 接收完成 DMA 请求使能
    this->TX_DONE = DISABLE;      // 发送完成 DMA 请求使能
    this->TXD_INV = DISABLE;      // TXD 输出极性取反
    this->RXD_INV = DISABLE;      // RXD 输入极性取反
    this->IE = DISABLE;           // 中断配置
}

/**
 * @brief    UART发送数据
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
void UART_SendData(UART_TypeDef *UARTx, u32 n)
{
    UARTx->BUFF = n;
}

/**
 * @brief    UART读缓冲区数据
 * @param UART_TypeDef *UARTx
 * @return 缓冲区数据
 * @par 更新记录  V1.0  2016/05/21  YangZJ  创建
 */
u32 UART_ReadData(UART_TypeDef *UARTx)
{
    return UARTx->BUFF;
}

/**
 * @brief    等待串口发送完成
 * @param UART_TypeDef *UARTx:要操作的UART对象
 * @par 更新记录  V1.0  2022/05/31  YangZJ  创建
 */
void UART_DelaySendOver(UART_TypeDef *UARTx)
{
    u32 n = 960000;
    while ((UARTx->IF & UART_IF_SendOver) == 0)
    {
        n--;
        if (n == 0)
        {
            break;
        }
    }
    UARTx->IF = UART_IF_SendOver;
}
/**
 * @brief    取得UART中断标志
 * @param UART_TypeDef *UARTx:要操作的UART对象
 * @return UART中断标志
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
u8 UART_GetFlag(UART_TypeDef *UARTx, u16 UART_IF_x)
{
    if (UARTx->IF & UART_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    取得UART中断标志
 * @param UART_TypeDef *UARTx:要操作的UART对象
 * @return UART中断标志
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
u8 UART_GetIRQFlag(UART_TypeDef *UARTx, u16 UART_IF_x)
{
    if (UARTx->IF & UART_IF_x & UARTx->IE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    清除UART中断标志位
 * @param UART_TypeDef *UARTx:要操作的UART对象, u32 nFlag当前中断标志
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
void UART_ClearIRQFlag(UART_TypeDef *UARTx, u16 UART_IF_x)
{
    UARTx->IF = UART_IF_x;
}
/**
 * @brief    串口使能
 * @param UART_TypeDef *UARTx:要操作的UART对象, u32 nFlag当前中断标志
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
void UART_Enable(UART_TypeDef *UARTx)
{
    if (UARTx == UART0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART0, ENABLE);
    }
    else if (UARTx == UART1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART1, ENABLE);
    }
    else if (UARTx == UART2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART2, ENABLE);
    }
}
/**
 * @brief    清除UART中断标志位
 * @param UART_TypeDef *UARTx:要操作的UART对象, u32 nFlag当前中断标志
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
void UART_Disable(UART_TypeDef *UARTx)
{
    if (UARTx == UART0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART0, DISABLE);
    }
    else if (UARTx == UART1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART1, DISABLE);
    }
    else if (UARTx == UART2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_UART2, DISABLE);
    }
}
/**
 * @brief    清除UART中断标志位
 * @param UART_TypeDef *UARTx:要操作的UART对象, u32 nFlag当前中断标志
 * @par 更新记录  V1.0  2022/08/30  YangZJ  创建
 */
void UART_SetBaudRate(UART_TypeDef *UARTx, u32 BaudRate)
{
    u32 mclk;
    s32 div;
    mclk = SYS_ReadMcuClk();
    div = (s32)((u32)((mclk + BaudRate) / BaudRate) - 1);
    if (div > 0xffff)
    {
        div = 0xffff;
    }
    if (div < 0)
    {
        div = 0;
    }
    UARTx->DIVH = (u32)(div >> 8) & 0xff;
    UARTx->DIVH = (u32)(div & 0xff);
}

/*! @} */
