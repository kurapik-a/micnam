/**
 * @file
 * @brief        GPIO库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
 *  */

#ifndef __lks32mc45x_GPIO_H
#define __lks32mc45x_GPIO_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO u32 PIE;   ///< 0x00 GPIO x输入使能
    __IO u32 POE;   ///< 0x04 GPIO x输出使能
    __IO u32 PDI;   ///< 0x08 GPIO x输入数据
    __IO u32 PDO;   ///< 0x0C GPIO x输出数据
    __IO u32 PUE;   ///< 0x10 GPIO x上拉使能
    __IO u32 PDE;   ///< 0x14 GPIO x下拉使能
    __IO u32 PODE;  ///< 0x18 GPIO x开漏使能
    __IO u32 PFLT;  ///< 0x1C GPIO x滤波使能
    __IO u32 F3210; ///< 0x20 GPIO x [3:0]功能选择
    __IO u32 F7654; ///< 0x24 GPIO x [7:4]功能选择
    __IO u32 FBA98; ///< 0x28 GPIO x [11:8]功能选择
    __IO u32 FFEDC; ///< 0x2C GPIO x [15:12]功能选择
    __IO u32 BSRR;  ///< 0x30 GPIO x 位操作寄存器
    __IO u32 BRR;   ///< 0x34 GPIO x 位清零寄存器

} GPIO_TypeDef;
typedef struct
{
    __IO u32 CR0;   ///< 0x00 GPIO 0[7:0] 中断/DMA 触发类型
    __IO u32 CR1;   ///< 0x04 GPIO 0[15:8]中断/DMA 触发类型
    __IO u32 null0; ///< 0x08 GPIO 1[7:0] 中断/DMA 触发类型
    __IO u32 null1; ///< 0x0C GPIO 1[15:8]中断/DMA 触发类型
    __IO u32 null2; ///< 0x10 GPIO 2[7:0] 中断/DMA 触发类型
    __IO u32 null3; ///< 0x14 GPIO 2[15:8]中断/DMA 触发类型
    __IO u32 null4; ///< 0x18 GPIO 3[7:0] 中断/DMA 触发类型
    __IO u32 null5; ///< 0x1C GPIO 3[15:8]中断/DMA 触发类型
    __IO u32 null6; ///< 0x20 GPIO 4[7:0] 中断/DMA 触发类型
    __IO u32 null7; ///< 0x24 GPIO 4[15:8]中断/DMA 触发类型
    __IO u32 null8; ///< 0x28 GPIO 5[7:0] 中断/DMA 触发类型
    __IO u32 null9; ///< 0x2C GPIO 5[15:8]中断/DMA 触发类型
    __IO u32 IF;   ///< 0x30 GPIO 0 中断标志
} EXTI0_TypeDef;
typedef struct
{
    __IO u32 CR0;   ///< 0x00 GPIO 0[7:0] 中断/DMA 触发类型
    __IO u32 CR1;   ///< 0x04 GPIO 0[15:8]中断/DMA 触发类型
    __IO u32 null2; ///< 0x10 GPIO 2[7:0] 中断/DMA 触发类型
    __IO u32 null3; ///< 0x14 GPIO 2[15:8]中断/DMA 触发类型
    __IO u32 null4; ///< 0x18 GPIO 3[7:0] 中断/DMA 触发类型
    __IO u32 null5; ///< 0x1C GPIO 3[15:8]中断/DMA 触发类型
    __IO u32 null6; ///< 0x20 GPIO 4[7:0] 中断/DMA 触发类型
    __IO u32 null7; ///< 0x24 GPIO 4[15:8]中断/DMA 触发类型
    __IO u32 null8; ///< 0x28 GPIO 5[7:0] 中断/DMA 触发类型
    __IO u32 null9; ///< 0x2C GPIO 5[15:8]中断/DMA 触发类型
    __IO u32 nulla; ///< 0x30 GPIO 0 中断标志
    __IO u32 IF;    ///< 0x30 GPIO 0 中断标志
} EXTI1_TypeDef;
typedef struct
{
    __IO u32 CR0;   ///< 0x00 GPIO 0[7:0] 中断/DMA 触发类型
    __IO u32 CR1;   ///< 0x04 GPIO 0[15:8]中断/DMA 触发类型
    __IO u32 null4; ///< 0x18 GPIO 3[7:0] 中断/DMA 触发类型
    __IO u32 null5; ///< 0x1C GPIO 3[15:8]中断/DMA 触发类型
    __IO u32 null6; ///< 0x20 GPIO 4[7:0] 中断/DMA 触发类型
    __IO u32 null7; ///< 0x24 GPIO 4[15:8]中断/DMA 触发类型
    __IO u32 null8; ///< 0x28 GPIO 5[7:0] 中断/DMA 触发类型
    __IO u32 null9; ///< 0x2C GPIO 5[15:8]中断/DMA 触发类型
    __IO u32 nullb; ///< 0x30 GPIO 0 中断标志
    __IO u32 nullc; ///< 0x30 GPIO 0 中断标志
    __IO u32 IF;   ///< 0x30 GPIO 0 中断标志
} EXTI2_TypeDef;
typedef struct
{
    __IO u32 CR0;   ///< 0x00 GPIO 0[7:0] 中断/DMA 触发类型
    __IO u32 CR1;   ///< 0x04 GPIO 0[15:8]中断/DMA 触发类型
    __IO u32 null6; ///< 0x20 GPIO 4[7:0] 中断/DMA 触发类型
    __IO u32 null7; ///< 0x24 GPIO 4[15:8]中断/DMA 触发类型
    __IO u32 null8; ///< 0x28 GPIO 5[7:0] 中断/DMA 触发类型
    __IO u32 null9; ///< 0x2C GPIO 5[15:8]中断/DMA 触发类型
    __IO u32 nulla; ///< 0x30 GPIO 0 中断标志
    __IO u32 nullb; ///< 0x30 GPIO 0 中断标志
    __IO u32 nullc; ///< 0x30 GPIO 0 中断标志
    __IO u32 IF;   ///< 0x30 GPIO 0 中断标志
} EXTI3_TypeDef;
typedef struct
{
    __IO u32 CR0;   ///< 0x00 GPIO 0[7:0] 中断/DMA 触发类型
    __IO u32 CR1;   ///< 0x04 GPIO 0[15:8]中断/DMA 触发类型
    __IO u32 null8; ///< 0x28 GPIO 5[7:0] 中断/DMA 触发类型
    __IO u32 null9; ///< 0x2C GPIO 5[15:8]中断/DMA 触发类型
    __IO u32 nulla; ///< 0x30 GPIO 0 中断标志
    __IO u32 nullb; ///< 0x30 GPIO 0 中断标志
    __IO u32 nullc; ///< 0x30 GPIO 0 中断标志
    __IO u32 nulld; ///< 0x30 GPIO 0 中断标志
    __IO u32 IF;   ///< 0x30 GPIO 0 中断标志
} EXTI4_TypeDef;
typedef struct
{
    __IO u32 CR0;   ///< 0x00 GPIO 0[7:0] 中断/DMA 触发类型
    __IO u32 CR1;   ///< 0x04 GPIO 0[15:8]中断/DMA 触发类型
    __IO u32 nulla; ///< 0x30 GPIO 0 中断标志
    __IO u32 nullb; ///< 0x30 GPIO 0 中断标志
    __IO u32 nullc; ///< 0x30 GPIO 0 中断标志
    __IO u32 nulld; ///< 0x30 GPIO 0 中断标志
    __IO u32 nulle; ///< 0x30 GPIO 0 中断标志
    __IO u32 IF;    ///< 0x30 GPIO 0 中断标志
} EXTI5_TypeDef;
#ifndef EXTI0
#define EXTI0 ((EXTI0_TypeDef *)(EXTI_BASE + 0x00))
#endif
#ifndef EXTI1
#define EXTI1 ((EXTI1_TypeDef *)(EXTI_BASE + 0x08))
#endif
#ifndef EXTI2
#define EXTI2 ((EXTI2_TypeDef *)(EXTI_BASE + 0x10))
#endif
#ifndef EXTI3
#define EXTI3 ((EXTI3_TypeDef *)(EXTI_BASE + 0x18))
#endif
#ifndef EXTI4
#define EXTI4 ((EXTI4_TypeDef *)(EXTI_BASE + 0x20))
#endif
#ifndef EXTI5
#define EXTI5 ((EXTI5_TypeDef *)(EXTI_BASE + 0x28))
#endif
typedef enum
{
    GPIO_Mode_IN = 0x00,   ///< 数字输入模式
    GPIO_Mode_OUT = 0x01,  ///< 数字输出模式
    GPIO_Mode_ANA = 0x02,  ///< 模拟输入模式
    GPIO_Mode_INOUT = 0x03 ///< 双向模式，通常需要配置为开漏
} GPIO_Mode_TypeDef;

typedef enum
{
    GPIO_PuPd_NOPULL = 0x00,
    GPIO_PuPd_UP = 0x01,   /*上拉*/
    GPIO_PuPd_DOWN = 0x02, /*下拉*/
} GPIO_PuPd_TypeDef;

typedef enum
{
    Bit_RESET = 0,
    Bit_SET
} GPIO_BitAction;

typedef struct
{
    u32 GPIO_Pin;                ///< 要配置的PIN
    u8 GPIO_PODEna;              ///< 开漏使能
    u8 GPIO_PFLT;                ///< 滤波使能
    GPIO_Mode_TypeDef GPIO_Mode; ///< GPIO模式:输入、输出、模拟
    GPIO_PuPd_TypeDef GPIO_PuPd; ///< 上拉/下拉
} GPIO_InitTypeDef;

#define GPIO_AF_GPIO 0x0   ///< 双向  GPIO
#define GPIO_AF_ANA 0x0    ///< 输入  模拟输入
#define GPIO_AF_CLK 0x1    ///< 输出  时钟
#define GPIO_AF_CMP 0x1    ///< 输出  数字比较器
#define GPIO_AF_HALL 0x2   ///< 输入  霍尔
#define GPIO_AF_MCPWM 0x3  ///< 输出  MCPWM
#define GPIO_AF_UART 0x4   ///< 双向  UART
#define GPIO_AF_SPI 0x5    ///< 双向  SPI
#define GPIO_AF_IIC 0x6    ///< 双向  I2C
#define GPIO_AF_TIMER0 0x7 ///< 双向  TIMER0
#define GPIO_AF_TIMER1 0x7 ///< 双向  TIMER1
#define GPIO_AF_TIMER2 0x8 ///< 双向  TIMER2
#define GPIO_AF_TIMER3 0x8 ///< 双向  TIMER3
#define GPIO_AF_TIMER4 0x7 ///< 双向  TIMER4
#define GPIO_AF_ADC 0x9    ///< 输出  ADC
#define GPIO_AF_CAN 0xA    ///< 双向  CAN
#define GPIO_AF_FLASH 0xB  ///< 双向  片外FLASH

/*GPIO_Pad define*/
#define GPIO_Pin_NONE 0
#define GPIO_Pin_0 BIT0
#define GPIO_Pin_1 BIT1
#define GPIO_Pin_2 BIT2
#define GPIO_Pin_3 BIT3
#define GPIO_Pin_4 BIT4
#define GPIO_Pin_5 BIT5
#define GPIO_Pin_6 BIT6
#define GPIO_Pin_7 BIT7
#define GPIO_Pin_8 BIT8
#define GPIO_Pin_9 BIT9
#define GPIO_Pin_10 BIT10
#define GPIO_Pin_11 BIT11
#define GPIO_Pin_12 BIT12
#define GPIO_Pin_13 BIT13
#define GPIO_Pin_14 BIT14
#define GPIO_Pin_15 BIT15
#define GPIO_Pin_ALL (u32)0xFFFF

/*GPIO_Pad_sources define*/
#define GPIO_PinSource_0 0
#define GPIO_PinSource_1 1
#define GPIO_PinSource_2 2
#define GPIO_PinSource_3 3
#define GPIO_PinSource_4 4
#define GPIO_PinSource_5 5
#define GPIO_PinSource_6 6
#define GPIO_PinSource_7 7
#define GPIO_PinSource_8 8
#define GPIO_PinSource_9 9
#define GPIO_PinSource_10 10
#define GPIO_PinSource_11 11
#define GPIO_PinSource_12 12
#define GPIO_PinSource_13 13
#define GPIO_PinSource_14 14
#define GPIO_PinSource_15 15

#define GPIO_EXTI_MODE_NONE 0    ///< 不触发
#define GPIO_EXTI_MODE_NEGEDGE 1 ///< 下降沿触发
#define GPIO_EXTI_MODE_POSEDGE 2 ///< 上升沿触发
#define GPIO_EXTI_MODE_EDGE 3    ///< 上升沿、下降沿均触发

///< 初始化、配置函数
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct);

void GPIO_LockRegister(GPIO_TypeDef *GPIOx);
void GPIO_UnlockRegister(GPIO_TypeDef *GPIOx);

///< GPIO读写函数
u8 GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin);
u32 GPIO_ReadInputData(GPIO_TypeDef *GPIOx);
u8 GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin);
u32 GPIO_ReadOutputData(GPIO_TypeDef *GPIOx);

void GPIO_SetBits(GPIO_TypeDef *GPIOx, u16 GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, u16 GPIO_Pin);
void GPIO_Write(GPIO_TypeDef *GPIOx, u32 Val);
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin, GPIO_BitAction BitVal);

///< GPIO功能选择函数
void GPIO_PinAFConfig(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource, u32 GPIO_AF);

///<
void GPIO_Config(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource, GPIO_Mode_TypeDef GPIO_Mode, u32 GPIO_AF); ///< GPIO配置
void GPIO_Enable(void);                                                                              ///< GPIO使能
void GPIO_Disable(void);                                                                             ///< GPIO失能
u8 GPIO_ReadPinBit(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource);
void GPIO_WritePinBit(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource, u8 vol);
void GPIO_SetPinBits(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource);
void GPIO_ResetPinBits(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource);
u8 GPIO_ReadPinOutBit(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource);

void GPIO_Config(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource, GPIO_Mode_TypeDef GPIO_Mode, u32 GPIO_AF); ///< GPIO配置

void GPIO_ExtiConfig(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource, u8 GPIO_EXTI_MODE_x); ///< GPIO外部中断配置
void GPIO_ClearIRQFlag(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource);
u8 GPIO_GetIRQFlag(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource);
u8 GPIO_GetFlag(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource);
#endif /*__lks32mc45x_GPIO_H */
