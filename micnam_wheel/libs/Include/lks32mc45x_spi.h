/**
 * @file
 * @brief        SPI库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __LKS32MC45X_SPI_H
#define __LKS32MC45X_SPI_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"
typedef struct
{

    __IO u32 CFG;     ///< 0x00  SPI配置寄存器
    __IO u32 IE;      ///< 0x04  SPI中断寄存器
    __IO u32 BAUD;    ///< 0x08  SPI波特率寄存器
    __IO u32 TX_DATA; ///< 0x0C  SPI发送数据寄存器
    __IO u32 RX_DATA; ///< 0x10  SPI接收数据寄存器
    __IO u32 SIZE;    ///< 0x14  SPI传输数据字节长度寄存器
} SPI_TypeDef;
#ifndef SPI0
#define SPI0 ((SPI_TypeDef *)SPI0_BASE)
#endif
#ifndef SPI1
#define SPI1 ((SPI_TypeDef *)SPI1_BASE)
#endif

typedef enum
{
    SPI_Master = 0x01, ///< SPI主模式
    SPI_Slave = 0x00   ///< SPI主模式
} SPI_Mode;

typedef enum
{
    SPI_Full = 0x0,    ///< SPI全双工
    SPI_Half_Tx = 0x2, ///< SPI全双工
    SPI_Half_Rx = 0x3  ///< SPI全双工
} SPI_Duplex;

typedef struct
{
    u8 EN;          ///<  SPI模块使能：0，关闭；1，开启
    u8 CPHA;        ///<  相位选择：0对应0；1对应1
    u8 CPOL;        ///<  极性选择：0对应0；1对应1
    u8 ByteLength;  ///<  SPI传输数据长度 8 - 16 有效
    u16 BaudRate;   ///<  波特率设置
    u16 DataOrder;  ///<  传输顺序：0，高位先传；1，低位先传
    u16 IRQEna;     ///<  SPI中断使能：0，关闭；1，开启
    u16 Mode;       ///<  主从模式选择：0，从模式；1，主模式
    u16 Duplex;     ///<  全双工、半双工工作模式选择：0，全双工；2，半双工仅发送；3，半双工仅接收
    u16 CS;         ///<  从设备下片选信号来源    0，恒有效    1，来源于主设备
    u16 Trig;       ///<  传输触发选择：0，内部自动执行（仅主模式有效）；1，外部触发
    u16 TRANS_MODE; ///<  SPI数据搬运方式：0，DMA搬运；1，MCU搬运
} SPI_InitTypeDef;

#define SPI_FIRSTSEND_LSB 1 ///< 低位先传
#define SPI_FIRSTSEND_MSB 0 ///< 低位先传

#define SPI_DMA_ENABLE 0  ///< 使能DMA搬运
#define SPI_DMA_DISABLE 1 ///< 使能DMA搬运

/*中断使能定义*/
#define SPI_IE_Enable BIT7   ///< SPI中断使能开关。默认值为0。
#define SPI_IE_TranDone BIT6 ///< SPI传输，完成事件中断使能信号
#define SPI_IE_SSErr BIT5    ///< SPI传输，异常事件中断使能信号。
#define SPI_IE_DataOver BIT4 ///< SPI传输，溢出事件中断使能信号。默认值为0

/*中断标志定义*/
#define SPI_IF_TranDone BIT2 ///< SPI传输，完成事件。高电平有效，写1清除。
#define SPI_IF_SSErr BIT1    ///< SPI传输，异常事件。Slave模式下，传输未完成，发生片选信号无效事件。高电平有效，写1清除。
#define SPI_IF_DataOver BIT0 ///< SPI传输，溢出事件。前次接收的旧数据没有被取走，本次接收的新数据已经到达。高电平有效，写1清除。

void SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct);
void SPI_StructInit(SPI_InitTypeDef *SPI_InitStruct);
void SPI_ClearIRQFlag(SPI_TypeDef *SPIx, u8 SPI_IF_x);
u8 SPI_GetIRQFlag(SPI_TypeDef *SPIx, u8 SPI_IF_x);
u8 SPI_GetFlag(SPI_TypeDef *SPIx, u8 SPI_IF_x);

void SPI_SendData(SPI_TypeDef *SPIx, u8 n);
u8 SPI_ReadData(SPI_TypeDef *SPIx);

void SPI_Enable(SPI_TypeDef *SPIx);
void SPI_Disable(SPI_TypeDef *SPIx);
void SPI_SetBaudRate(SPI_TypeDef *SPIx, u32 BaudRate);

#endif /*__LKS32MC45X_SPI_H */
