/**
 * @file
 * @brief        串口库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __lks32mc45x_UART_H
#define __lks32mc45x_UART_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO u32 CTRL; ///< 0x00    UART控制寄存器
    __IO u32 DIVH; ///< 0x04    UART波特率设置高字节寄存器
    __IO u32 DIVL; ///< 0x08    UART波特率设置低字节寄存器
    __IO u32 BUFF; ///< 0x0C    UART收发缓冲寄存器
    __IO u32 ADR;  ///< 0x10    485通信地址匹配寄存器
    __IO u32 STT;  ///< 0x14    UART状态寄存器
    __IO u32 RE;   ///< 0x18    UART DMA请求使能寄存器
    __IO u32 IE;   ///< 0x1C    UART中断使能寄存器
    __IO u32 IF;   ///< 0x20    UART中断标志寄存器
    __IO u32 IOC;  ///< 0x24    UART IO控制
} UART_TypeDef;

#ifndef UART0
#define UART0 ((SPI_TypeDef *)UART0_BASE)
#endif
#ifndef UART1
#define UART1 ((SPI_TypeDef *)UART1_BASE)
#endif
#ifndef UART2
#define UART2 ((SPI_TypeDef *)UART2_BASE)
#endif

typedef struct
{
    u32 BAUDRATE;     ///< 波特率
    u16 DUPLEX;       ///< 半双工模式使能，tx_data
    u16 MD_EN;        ///< Multi-drop 使能
    u16 CK_EN;        ///< 数据校验使能
    u16 CK_TYPE;      ///< 奇偶校验配置       0:偶校验（EVEN）;1: 奇校验（ODD）
    u16 BIT_ORDER;    ///< 数据发送顺序配置   0:LSB;1:MSB
    u16 STOP_LEN;     ///< 停止位长度配置     0:1-Bit;1:2-Bit
    u16 BYTE_LEN;     ///< 数据长度配置       0:8-Bit;1:9-Bit
    u16 ADR;          ///< 多机通讯时的从机地址
    u16 TX_BUF_EMPTY; ///< 发送缓冲区空 DMA 请求使能
    u16 RX_DONE;      ///< 接收完成 DMA 请求使能
    u16 TX_DONE;      ///< 发送完成 DMA 请求使能
    u16 TXD_INV;      ///< TXD 输出极性使能使能
    u16 RXD_INV;      ///< RXD 输入极性使能使能
    u16 IE;           ///< 中断配置
} UART_InitTypeDef;

#define UART_WORDLENGTH_8b 0
#define UART_WORDLENGTH_7b 1

#define UART_STOPBITS_1b 0
#define UART_STOPBITS_2b 1

#define UART_FIRSTSEND_LSB 0
#define UART_FIRSTSEND_MSB 1
typedef enum
{
    UART_Parity_NO = 0x00,
    UART_Parity_EVEN = 0x01,
    UART_Parity_ODD = 0x02
} UART_ParityMode;

#define UART_WORDLENGTH_8b 0
#define UART_WORDLENGTH_7b 1

#define UART_STOPBITS_1b 0
#define UART_STOPBITS_2b 1

#define UART_FIRSTSEND_LSB 0
#define UART_FIRSTSEND_MSB 1

/*中断使能定义*/
#define UART_IRQEna_SendOver BIT0      /*使能发送完成中断*/
#define UART_IRQEna_RcvOver BIT1       /*使能接收完成中断*/
#define UART_IRQEna_SendBuffEmpty BIT2 /*使能发送缓冲区空中断*/
#define UART_IRQEna_StopError BIT3     /*使能停止位错误*/
#define UART_IRQEna_CheckError BIT4    /*使能校验错误*/
#define UART_IRQEna_TX_DMA_RE BIT5     /*发送完成DMA请求使能*/
#define UART_IRQEna_RX_DMA_RE BIT6     /*接收完成DMA请求使能*/
#define UART_IRQEna_TX_BUF_DMA_RE BIT7 /*发送缓冲区空DMA请求使能*/

/*中断标志定义*/
#define UART_IF_SendOver BIT0     /*发送完成中断*/
#define UART_IF_RcvOver BIT1      /*接收完成中断*/
#define UART_IF_SendBufEmpty BIT2 /*发送缓冲区空中断*/
#define UART_IF_StopError BIT3    /*停止位错误*/
#define UART_IF_CheckError BIT4   /*校验错误 */

void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct);
void UART_StructInit(UART_InitTypeDef *UART_InitStruct);

void UART_SendData(UART_TypeDef *UARTx, u32 n);
u32 UART_ReadData(UART_TypeDef *UARTx);

void UART_SendnData(UART_TypeDef *UARTx, char *sendData, u32 len);
void UART_RecvnData(UART_TypeDef *UARTx, char *recvData, u32 maxlen);

u8 UART_GetIRQFlag(UART_TypeDef *UARTx, u16 UART_IF_x);
u8 UART_GetFlag(UART_TypeDef *UARTx, u16 UART_IF_x);
void UART_ClearIRQFlag(UART_TypeDef *UARTx, u16 UART_IF_x);

void UART_Enable(UART_TypeDef *UARTx);
void UART_Disable(UART_TypeDef *UARTx);
void UART_SetBaudRate(UART_TypeDef *UARTx, u32 BaudRate);

void UART_DelaySendOver(UART_TypeDef *UARTx);

#endif /*__lks32mc45x_UART_H */
