/**
 * @file
 * @brief   系统寄存器库函数头文件
 * @author  YangZJ
 * @date    2023-01-02
*  */
#ifndef __lks32mc45x_SYS_H
#define __lks32mc45x_SYS_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

/*PLL信号输入源选择*/
#define SYS_PLLSRSEL_RCH        		0x00          /*使用12MHz RC时钟*/
#define SYS_PLLSRSEL_CRYSTAL_12M    0x01          /*使用12MHz 晶体时钟*/
#define SYS_PLLSRSEL_CRYSTAL_24M    0x02          /*使用24MHz 晶体时钟*/

/* 主时钟选择 */
#define CLK_SEL_HS							0x00
#define CLK_SEL_PLL							0x01
#define CLK_SEL_XTAL						0x03

typedef struct
{
	uint32_t PLL_SrcSel;                      /*PLL信号输入源选择，RC时钟或外部晶体*/
	uint32_t PLL_DivSel;                      /*选择8个时钟周期中，哪个周期输出时钟*/
	uint32_t Clk_Sel;						  						/**选择12MHz HS时钟、 XTAL或者PLL作为主时钟*/										
	uint32_t WDT_Ena;                         /*使能看门狗*/
}SYS_InitTypeDef;

typedef struct
{
    __IO u32 CLK_CFG;   ///< 0x80 时钟控制寄存器
    __IO u32 IO_CFG;    ///< 0x84 IO控制寄存器
    __IO u32 DBG_CFG;   ///< 0x88 Debug控制寄存器
    __IO u32 null0;     ///< 0x8C 地址对齐，无意义
    __IO u32 CLK_DIV0;  ///< 0x90 外设时钟分频寄存器0
    __IO u32 CLK_DIV1;  ///< 0x94 外设时钟分频寄存器1
    __IO u32 CLK_DIV2;  ///< 0x98 外设时钟分频寄存器2
    __IO u32 CLK_FEN;   ///< 0x9C 外设时钟门控寄存器
    __IO u32 null1;     ///< 0xA0 地址对齐，无意义
    __IO u32 SFT_RST;   ///< 0xA4 软复位寄存器
    __IO u32 PROTECT;   ///< 0xA8 写保护寄存器
    __IO u32 CACHE_CFG; ///< 0xAC Cache控制寄存器
    __IO u32 null2;     ///< 0xB0 地址对齐，无意义
    __IO u32 null3;     ///< 0xB4 地址对齐，无意义
    __IO u32 null4;     ///< 0xB8 地址对齐，无意义
    __IO u32 null5;     ///< 0xBC 地址对齐，无意义
    __IO u32 null6;     ///< 0xC0 地址对齐，无意义
    __IO u32 MEM_CFG;   ///< 0xC4 Memory控制寄存器
    __IO u32 null7;     ///< 0xC8 地址对齐，无意义
    __IO u32 null8;     ///< 0xCC 地址对齐，无意义
    __IO u32 FLSE;      ///< 0xD0 On-Chip FLASH擦除控制寄存器
    __IO u32 FLSP;      ///< 0xD4 On-Chip FLASH编程控制寄存器
} SYS_TypeDef;
#ifndef SYS
#define SYS ((SYS_TypeDef *)(MISC_BASE + 0x80))
#endif
/*数字模块位定义*/
#define SYS_MODULE_I2C0 BIT0
#define SYS_MODULE_I2C1 BIT1
#define SYS_MODULE_HALL0 BIT2
#define SYS_MODULE_HALL1 BIT3
#define SYS_MODULE_UART0 BIT4
#define SYS_MODULE_UART1 BIT5
#define SYS_MODULE_UART2 BIT6
#define SYS_MODULE_CMP BIT8
#define SYS_MODULE_MCPWM0 BIT9
#define SYS_MODULE_MCPWM1 BIT10
#define SYS_MODULE_TIMER0 BIT12
#define SYS_MODULE_TIMER1 BIT13
#define SYS_MODULE_TIMER2 BIT14
#define SYS_MODULE_TIMER3 BIT15
#define SYS_MODULE_TIMER4 BIT16
#define SYS_MODULE_QEP0 BIT17
#define SYS_MODULE_QEP1 BIT18
#define SYS_MODULE_QEP2 BIT19
#define SYS_MODULE_QEP3 BIT20
#define SYS_MODULE_GPIO BIT22
#define SYS_MODULE_CRC BIT23
#define SYS_MODULE_CORDIC BIT24
#define SYS_MODULE_ADC0 BIT26
#define SYS_MODULE_ADC1 BIT27
#define SYS_MODULE_ADC2 BIT28
#define SYS_MODULE_CAN BIT29
#define SYS_MODULE_SPI0 BIT30
#define SYS_MODULE_SPI1 BIT31

/*主时钟配置*/
#define SYS_MCLK_192M_RC 0x00
#define SYS_MCLK_96M_RC 0x01
#define SYS_MCLK_48M_RC 0x02
#define SYS_MCLK_24M_RC 0x03
#define SYS_MCLK_12M_RC 0x04
#define SYS_MCLK_32K_RC 0x05

#define SYS_MCLK_192M_XTAL12 0x10
#define SYS_MCLK_96M_XTAL12 0x11
#define SYS_MCLK_48M_XTAL12 0x12
#define SYS_MCLK_24M_XTAL12 0x13
#define SYS_MCLK_12M_XTAL12 0x14

#define SYS_MCLK_192M_XTAL24 0x20
#define SYS_MCLK_96M_XTAL24 0x21
#define SYS_MCLK_48M_XTAL24 0x22
#define SYS_MCLK_24M_XTAL24 0x23
#define SYS_MCLK_12M_XTAL24 0x24

/*模块使能*/
void SYS_ModuleClockCmd(u32 nMODULE, u8 state);

/*模块软复位*/
void SYS_SoftResetMODULE(u32 nMODULE);

///< 系统时钟配置
void SYS_MclkChoice(u8 SYS_MCLK_x);
u32 SYS_ReadMcuClk(void);
void SYS_UartClkDiv(u16 div);
void SYS_ClearRst(void);

/*SYS时钟初始化*/ 
void SYS_Init(SYS_InitTypeDef* SYS_InitStruct);

/*SYS时钟结构体初始化*/ 
void SYS_StructInit(SYS_InitTypeDef* SYS_InitStruct);

/**
 * @brief 复位所有外设到默认状态（芯片上电后的状态）
 * @warning 不复位的寄存器有：
 *              复位事件记录寄存器
 *              后备存储器 
 *              ram  
 *              校准寄存器（仅限于上电会自动加载的校准寄存器）
 *              所有内核的寄存器
 */
void SYS_ResetPeripheral(void);
/**
 * @brief 复位所有内部外设为默认值(除io的所有模块)
 * @warning 不复位的寄存器有：
 *              复位事件记录寄存器
 *              后备存储器
 *              ram
 *              校准寄存器（仅限于上电会自动加载的校准寄存器）
 *              所有内核的寄存器
 */
void SYS_ResetPeripheralInternal(void);
/**
 * @brief 复位所有IO为默认值（gpio以及部分和io有关的sys寄存器）
 */
void SYS_ResetIO(void);
#endif /*__lks32mc45x_SYS_H */
