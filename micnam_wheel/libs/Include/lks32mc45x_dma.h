/**
 * @file
 * @brief        DMA库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */

#ifndef __LKS32MC45X_DMA_H
#define __LKS32MC45X_DMA_H

#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    u8 DMA_Channel_EN; ///< DMA 通道使能
    u8 DMA_IRQ_EN;     ///< DMA 通道使能
    u8 DMA_RMODE;      ///< DMA 通道使能
    u8 DMA_CIRC;       ///< DMA 通道使能
    u8 DMA_SINC;       ///< DMA 通道使能
    u8 DMA_DINC;       ///< DMA 通道使能
    u8 DMA_SBTW;       ///< DMA 通道使能
    u8 DMA_DBTW;       ///< DMA 通道使能
    u32 DMA_REQ_EN;    ///< DMA 通道使能
    u32 DMA_TIMES;     ///< DMA 通道使能
    u32 DMA_SADR;      ///< DMA 通道使能
    u32 DMA_DADR;      ///< DMA 通道使能
} DMA_InitTypeDef;

typedef struct
{
    __IO u32 CCR;
    __IO u32 REN;
    __IO u32 CTMS;
    __IO u32 SADR;
    __IO u32 DADR;
} DMA_RegTypeDef;

typedef struct
{
    __IO u32 CTRL;
    __IO u32 IE;
    __IO u32 IF;
} DMA_MODULERegTypeDef;

#define DMA_CH0 ((DMA_RegTypeDef *)(DMA_BASE))
#define DMA_CH1 ((DMA_RegTypeDef *)(DMA_BASE + 0x18))
#define DMA_CH2 ((DMA_RegTypeDef *)(DMA_BASE + 0x30))
#define DMA_CH3 ((DMA_RegTypeDef *)(DMA_BASE + 0x48))
#define DMA_CH4 ((DMA_RegTypeDef *)(DMA_BASE + 0x60))
#define DMA_CH5 ((DMA_RegTypeDef *)(DMA_BASE + 0x78))
#define DMA_CH6 ((DMA_RegTypeDef *)(DMA_BASE + 0x90))
#define DMA_CH7 ((DMA_RegTypeDef *)(DMA_BASE + 0xA8))
#define DMA_MODULE ((DMA_MODULERegTypeDef *)(DMA_BASE + 0xc0))

#define DMA ((DMA_MODULERegTypeDef *)(DMA_BASE + 0xc0))

#define DMA_TCIE BIT0 ///< 传输完成中断使能，高有效

#define DMA_CH3_FIF BIT3 ///< 通道 3 完成中断标志，高有效，写 1 清零
#define DMA_CH2_FIF BIT2 ///< 通道 3 完成中断标志，高有效，写 1 清零
#define DMA_CH1_FIF BIT1 ///< 通道 3 完成中断标志，高有效，写 1 清零
#define DMA_CH0_FIF BIT0 ///< 通道 3 完成中断标志，高有效，写 1 清零

#define DMA_BYTE_TRANS 0     ///< 访问位宽， 0:byte
#define DMA_HALFWORD_TRANS 1 ///< 访问位宽， 0:byte
#define DMA_WORD_TRANS 2     ///< 访问位宽， 0:byte

#define DMA_REQ_SW BIT31       ///<  SW        触发
#define DMA_REQ_GPIO BIT29     ///<  GPIO      触发
#define DMA_REQ_FMAC_Y BIT28   ///<  FMAC_Y    触发
#define DMA_REQ_FMAC_X1 BIT27  ///<  FMAC_X1   触发
#define DMA_REQ_CMP BIT26      ///<  CMP       触发
#define DMA_REQ_HALL1 BIT25    ///<  HALL1     触发
#define DMA_REQ_HALL0 BIT24    ///<  HALL0     触发
#define DMA_REQ_I2C1_TX BIT23  ///<  I2C1_TX   触发
#define DMA_REQ_I2C1_RX BIT22  ///<  I2C1_RX   触发
#define DMA_REQ_UART2_TX BIT21 ///<  UART2_TX  触发
#define DMA_REQ_UART2_RX BIT20 ///<  UART2_RX  触发
#define DMA_REQ_UART1_TX BIT19 ///<  UART1_TX  触发
#define DMA_REQ_UART1_RX BIT18 ///<  UART1_RX  触发
#define DMA_REQ_UART0_TX BIT17 ///<  UART0_TX  触发
#define DMA_REQ_UART0_RX BIT16 ///<  UART0_RX  触发
#define DMA_REQ_I2C0_TX BIT15  ///<  I2C0_TX   触发
#define DMA_REQ_I2C0_RX BIT14  ///<  I2C0_RX   触发
#define DMA_REQ_MCPWM1 BIT13   ///<  MCPWM1    触发
#define DMA_REQ_MCPWM0 BIT12   ///<  MCPWM0    触发
#define DMA_REQ_SPI1_TX BIT11  ///<  SPI1_TX   触发
#define DMA_REQ_SPI1_RX BIT10  ///<  SPI1_RX   触发
#define DMA_REQ_SPI0_TX BIT9   ///<  SPI0_TX   触发
#define DMA_REQ_SPI0_RX BIT8   ///<  SPI0_RX   触发
#define DMA_REQ_TIMER4 BIT7    ///<  TIMER4    触发
#define DMA_REQ_TIMER3 BIT6    ///<  TIMER3    触发
#define DMA_REQ_TIMER2 BIT5    ///<  TIMER2    触发
#define DMA_REQ_TIMER1 BIT4    ///<  TIMER1    触发
#define DMA_REQ_TIMER0 BIT3    ///<  TIMER0    触发
#define DMA_REQ_ADC2 BIT2      ///<  ADC2      触发
#define DMA_REQ_ADC1 BIT1      ///<  ADC1      触发
#define DMA_REQ_ADC0 BIT0      ///<  ADC0      触发

#define DMA_IF_CH0 BIT0
#define DMA_IF_CH1 BIT1
#define DMA_IF_CH2 BIT2
#define DMA_IF_CH3 BIT3
#define DMA_IF_CH4 BIT4
#define DMA_IF_CH5 BIT5
#define DMA_IF_CH6 BIT6
#define DMA_IF_CH7 BIT7

#define DMA_IE_CH0 BIT0
#define DMA_IE_CH1 BIT1
#define DMA_IE_CH2 BIT2
#define DMA_IE_CH3 BIT3
#define DMA_IE_CH4 BIT4
#define DMA_IE_CH5 BIT5
#define DMA_IE_CH6 BIT6
#define DMA_IE_CH7 BIT7

void DMA_Init(DMA_RegTypeDef *DMA_CHx, DMA_InitTypeDef *DMAInitStruct);
void DMA_StructInit(DMA_InitTypeDef *DMAInitStruct);
u32 DMA_GetIRQFlag(u32 DMA_IF_x);
void DMA_ClearIRQFlag(u32 DMA_IF_x);
u32 DMA_GetFlag(u32 DMA_IF_x);

void DMA_Enable(void);
void DMA_Disable(void);
void DMA_SetAddr(DMA_RegTypeDef *DMA_CHx, u32 addr1, u32 addr2);
void DMA_SetLength(DMA_RegTypeDef *DMA_CHx, u32 len);
void DMA_Trigger(DMA_RegTypeDef *DMA_CHx);
void DMA_SetRequest(DMA_RegTypeDef *DMA_CHx, u32 DMA_REQ_x);

#endif
