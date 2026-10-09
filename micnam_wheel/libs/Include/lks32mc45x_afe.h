/**
 * @file
 * @brief        模拟部分驱动
 * @author       YangZJ
 * @date         2023-01-02
 *  */
#ifndef __lks32mc45x_AFE_H
#define __lks32mc45x_AFE_H
#include "basic.h"

typedef struct
{
    __IO u32 null0; ///<  0x00 填充地址
    __IO u32 INFO;  ///<  0x04 芯片版本信息寄存器
    __IO u32 DBG0;  ///<  0x08 调试寄存器
    __IO u32 null1; ///<  0x0C 填充地址

    __IO u32 REG0; ///<  0x10 模拟配置寄存器 0
    __IO u32 REG1; ///<  0x14 模拟配置寄存器 1
    __IO u32 REG2; ///<  0x18 模拟配置寄存器 2
    __IO u32 REG3; ///<  0x1C 模拟配置寄存器 3

    __IO u32 REG4;  ///<  0x20 模拟配置寄存器 4
    __IO u32 REG5;  ///<  0x24 模拟配置寄存器 5
    __IO u32 REG6;  ///<  0x28 模拟配置寄存器 6
    __IO u32 null2; ///<  0x2c 填充地址

    __IO u32 null3; ///<  0x30 填充地址
    __IO u32 REG9;  ///<  0x34 模拟配置寄存器 9
    __IO u32 REGA;  ///<  0x38 模拟配置寄存器 A
    __IO u32 REGB;  ///<  0x3C 模拟配置寄存器 B

    __IO u32 REGC; ///<  0x40 模拟配置寄存器 C
    __IO u32 REGD; ///<  0x44 模拟配置寄存器 D
    __IO u32 REGE; ///<  0x48 模拟配置寄存器 E
    __IO u32 REGF; ///<  0x4C 模拟配置寄存器 F

    __IO u32 REG10; ///<  0x50 模拟配置寄存器 10
    __IO u32 null4; ///<  0x54 填充地址
    __IO u32 TMP_A; ///<  0x58 温度传感器系数 A
    __IO u32 TMP_B; ///<  0x5C 温度传感器系数 B

    __IO u32 DAC0;     ///<  0x60 DAC0 数字量寄存器
    __IO u32 DAC0_AMC; ///<  0x64 DAC0 增益校正寄存器
    __IO u32 DAC0_DC;  ///<  0x68 DAC0 直流偏置寄存器
    __IO u32 null5;    ///<  0x6c 填充地址

    __IO u32 DAC1;     ///<  0x70 DAC1 数字量寄存器
    __IO u32 DAC1_AMC; ///<  0x74 DAC1 增益校正寄存器
    __IO u32 DAC1_DC;  ///<  0x78 DAC1 直流偏置寄存器
} AFE_TypeDef;

#ifndef AFE
#define AFE ((AFE_TypeDef *)(MISC_BASE))
#endif

typedef struct
{
    u8 PLLPDN;    ///<  PLL使能0: 关闭1: 使能
    u8 XDIV;      ///<  晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
    u8 PLLSR_SEL; ///<  PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
    u8 XTALPDN;   ///<  晶体起振电路使能0: 关闭1: 使能
    u8 XTRSEL;    ///<  晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
} AFE_ClkInitTypeDef;

typedef struct
{
    u16 PDT_PD;    ///<  掉电检测电路关闭控制信号0: 开启1: 关闭
    u16 WK_POL;    ///<  Wakeup IO唤醒信号极性选择0: 低电平有效，选中的wakeup IO上加100k上拉电阻1: 高电平有效，选中的wakeup IO上加100k下拉电阻
    u16 PVDSEL;    ///<  电源掉电监测阈值选择00: 3.0V01: 2.710: 2.4V11: 2.1V
    u16 VSR_PDT;   ///<  掉电检测基准源选择 0:选择低功耗基准源1:选择DAC0的输出
    u16 PORFIL_EN; ///<  POR滤波使能0: 关闭 1: 使能
    u16 LDOOUT_EN; ///<  LDO 输出到 IO 使能  1:使能  0:关闭
    u16 LDO2_CTL;  ///<  可掉电LDO电源关闭控制信号0: 开启1: 关闭
    u16 RCHPD;     ///<  RCH模块关闭信号0: 开启1: 关闭
    u16 BGPPD;     ///<  BGP模块关闭信号0: 开启1: 关闭
} AFE_DebugInitTypeDef;

/*模拟模块位定义*/
#define AFE_MODULE_PLL BIT23  ///<  [7  ] PLLPDN
#define AFE_MODULE_XTAL BIT22 ///<  [6  ] XTALPDN
#define AFE_MODULE_TMP BIT21  ///<  [5  ] TMPPDN
#define AFE_MODULE_LDO2 BIT20 ///<  [4  ] LDO2_CTL
#define AFE_MODULE_DAC1 BIT19 ///<  [3  ] DAC1PDN
#define AFE_MODULE_DAC0 BIT18 ///<  [2  ] DAC0PDN
#define AFE_MODULE_RCH BIT17  ///<  [1  ] RCHPD
#define AFE_MODULE_BGP BIT16  ///<  [0  ] BGPPD
#define AFE_MODULE_ADC BIT15  ///<  [15 ] ADC
#define AFE_MODULE_CMP5 BIT13 ///<  [13 ] CMP5
#define AFE_MODULE_CMP4 BIT12 ///<  [12 ] CMP4
#define AFE_MODULE_CMP3 BIT11 ///<  [11 ] CMP3
#define AFE_MODULE_CMP2 BIT10 ///<  [10 ] CMP2
#define AFE_MODULE_CMP1 BIT9  ///<  [9  ] CMP1
#define AFE_MODULE_CMP0 BIT8  ///<  [8  ] CMP0
#define AFE_MODULE_OPA5 BIT5  ///<  [5  ] OPA5
#define AFE_MODULE_OPA4 BIT4  ///<  [4  ] OPA4
#define AFE_MODULE_OPA3 BIT3  ///<  [3  ] OPA3
#define AFE_MODULE_OPA2 BIT2  ///<  [2  ] OPA2
#define AFE_MODULE_OPA1 BIT1  ///<  [1  ] OPA1
#define AFE_MODULE_OPA0 BIT0  ///<  [0  ] OPA0

// 运放反馈电阻定义
#define AFE_OPA_RES_20_10 0
#define AFE_OPA_RES_40_10 1
#define AFE_OPA_RES_80_10 2
#define AFE_OPA_RES_160_10 3
#define AFE_OPA_RES_320_10 4
#define AFE_OPA_RES_320_5 5

// 可以输出到P4.6的信号定义
#define AFE_OUT_P4_6_NULL 0x00
#define AFE_OUT_P4_6_OPA3 0x10
#define AFE_OUT_P4_6_OPA4 0x20
#define AFE_OUT_P4_6_OPA5 0x30

// 可以输出到P3.11的信号定义
#define AFE_OUT_P3_11_NULL 0x00
#define AFE_OUT_P3_11_OPA0 0x01
#define AFE_OUT_P3_11_OPA1 0x02
#define AFE_OUT_P3_11_OPA2 0x03

// 比较器比较时间定义
#define AFE_CMP_TIME_30nS 0x01  ///< 比较器比较时间30nS
#define AFE_CMP_TIME_150nS 0x00 ///< 比较器比较时间150nS
#define AFE_CMP_TIME_600nS 0x02 ///< 比较器比较时间600nS

// 比较器回差电压定义
#define AFE_CMP_HYS_20mV 0 ///< 比较器回差20mV
#define AFE_CMP_HYS_40mV 1 ///< 比较器回差40mV

// 比较器0负端信号选择
#define AFE_CMP0_SELN_IN 0  ///< 比较器0-5
#define AFE_CMP0_SELN_REF 1 ///< 比较器0-5
#define AFE_CMP0_SELN_DAC 2 ///< 比较器0 DAC0 比较器1 DAC1
#define AFE_CMP0_SELN_MID 3 ///< 比较器0

// 比较器1负端信号选择
#define AFE_CMP1_SELN_IN 0  ///< 比较器0-5
#define AFE_CMP1_SELN_REF 1 ///< 比较器0-5
#define AFE_CMP1_SELN_DAC 2 ///< 比较器0 DAC0 比较器1 DAC1
#define AFE_CMP1_SELN_MID 3 ///< 比较器0

// 比较器2负端信号选择
#define AFE_CMP2_SELN_IN 0   ///< 比较器0-5
#define AFE_CMP2_SELN_REF 1  ///< 比较器0-5
#define AFE_CMP2_SELN_DAC0 2 ///< 比较器2-5
#define AFE_CMP2_SELN_DAC1 3 ///< 比较器2-5

// 比较器3负端信号选择
#define AFE_CMP3_SELN_IN 0   ///< 比较器0-5
#define AFE_CMP3_SELN_REF 1  ///< 比较器0-5
#define AFE_CMP3_SELN_DAC0 2 ///< 比较器2-5
#define AFE_CMP3_SELN_DAC1 3 ///< 比较器2-5

// 比较器4负端信号选择
#define AFE_CMP4_SELN_IN 0   ///< 比较器0-5
#define AFE_CMP4_SELN_REF 1  ///< 比较器0-5
#define AFE_CMP4_SELN_DAC0 2 ///< 比较器2-5
#define AFE_CMP4_SELN_DAC1 3 ///< 比较器2-5

// 比较器5负端信号选择
#define AFE_CMP5_SELN_IN 0   ///< 比较器0-5
#define AFE_CMP5_SELN_REF 1  ///< 比较器0-5
#define AFE_CMP5_SELN_DAC0 2 ///< 比较器2-5
#define AFE_CMP5_SELN_DAC1 3 ///< 比较器2-5

// 比较器0正端信号选择
#define AFE_CMP0_SELP_IP0 0
#define AFE_CMP0_SELP_OPA0_IP 1
#define AFE_CMP0_SELP_OPA0_OUT_HF 2
#define AFE_CMP0_SELP_OPA1_OUT_HF 3
#define AFE_CMP0_SELP_IP1 4
#define AFE_CMP0_SELP_IP2 5
#define AFE_CMP0_SELP_IP3 6
#define AFE_CMP0_SELP_AVSS 7

// 比较器1正端信号选择
#define AFE_CMP1_SELP_IP0 0
#define AFE_CMP1_SELP_OPA1_IP 1
#define AFE_CMP1_SELP_OPA1_OUT_HF 2
#define AFE_CMP1_SELP_OPA2_OUT_HF 3
#define AFE_CMP1_SELP_IP1 4
#define AFE_CMP1_SELP_IP2 5
#define AFE_CMP1_SELP_IP3 6
#define AFE_CMP1_SELP_AVSS 7

// 比较器2正端信号选择
#define AFE_CMP2_SELP_IP0 0
#define AFE_CMP2_SELP_OPA2_IP 1
#define AFE_CMP2_SELP_OPA2_OUT_HF 2
#define AFE_CMP2_SELP_OPA3_OUT_HF 3
#define AFE_CMP2_SELP_IP1 4
#define AFE_CMP2_SELP_IP2 5
#define AFE_CMP2_SELP_IP3 6
#define AFE_CMP2_SELP_AVSS 7

// 比较器3正端信号选择
#define AFE_CMP3_SELP_IP0 0
#define AFE_CMP3_SELP_OPA3_IP 1
#define AFE_CMP3_SELP_OPA3_OUT_HF 2
#define AFE_CMP3_SELP_OPA4_OUT_HF 3
#define AFE_CMP3_SELP_IP1 4
#define AFE_CMP3_SELP_IP2 5
#define AFE_CMP3_SELP_IP3 6
#define AFE_CMP3_SELP_AVSS 7

// 比较器4正端信号选择
#define AFE_CMP4_SELP_IP0 0
#define AFE_CMP4_SELP_OPA4_IP 1
#define AFE_CMP4_SELP_OPA4_OUT_HF 2
#define AFE_CMP4_SELP_OPA5_OUT_HF 3
#define AFE_CMP4_SELP_IP1 4
#define AFE_CMP4_SELP_IP2 5
#define AFE_CMP4_SELP_IP3 6
#define AFE_CMP4_SELP_AVSS 7

// 比较器5正端信号选择
#define AFE_CMP5_SELP_IP0 0
#define AFE_CMP5_SELP_OPA5_IP 1
#define AFE_CMP5_SELP_OPA5_OUT_HF 2
#define AFE_CMP5_SELP_OPA0_OUT_HF 3
#define AFE_CMP5_SELP_IP1 4
#define AFE_CMP5_SELP_IP2 5
#define AFE_CMP5_SELP_IP3 6
#define AFE_CMP5_SELP_AVSS 7
// ADC工作频率
#define AFE_ADC_CLK_96Mhz 0
#define AFE_ADC_CLK_48Mhz 3
#define AFE_ADC_CLK_24Mhz 2
// ADC基准信号
#define AFE_ADC_REF_AVDD 1
#define AFE_ADC_REF_BGP 0
// DAC量程（固定基准为BGP）
#define AFE_DAC_RANGE_1_2V 0x01
#define AFE_DAC_RANGE_3_0V 0x00
// 晶体电容
#define AFE_XCSEL_9_19PF 0  // 晶体电容调整为9.19pF（默认值）
#define AFE_XCSEL_10_77PF 1 // 晶体电容调整为10.77pF
#define AFE_XCSEL_6_02PF 2  // 晶体电容调整为6.02pF
#define AFE_XCSEL_7_61PF 3  // 晶体电容调整为7.61pF
///< DAC相关
void AFE_Dac0OutGpio3Pin4(u8 state);
void AFE_Dac1OutGpio4Pin7(u8 state);
void AFE_DacSetVal(u32 AFE_MODULE_DACx, u16 val);
void AFE_DacSetVoltage(u32 AFE_MODULE_DACx, float Voltage);
void AFE_DacSetRange(u32 AFE_MODULE_DACx, u16 AFE_DAC_RANGE_x);

///< 运放相关
void AFE_OpaOutGpio3Pin11(u8 AFE_OUT_P3_11_x);
void AFE_OpaOutGpio4Pin6(u8 AFE_OUT_P4_6_x);
void AFE_OpaGainConfig(u32 AFE_OPAx, u32 AFE_OPA_RES_x);
// 比较器相关
void AFE_CmpTimeConfig(u32 AFE_CMP_TIME_x);
void AFE_CmpHysteresisConfig(u32 AFE_CMP_HYS_x);
void AFE_CmpPassagewayConfig(u32 AFE_MODULE_CMPx, u32 AFE_CMPx_SELP_x, u32 AFE_CMPx_SELN_x);
// ADC相关
void AFE_AdcConfig(u32 AFE_ADC_CLK_x, u32 AFE_ADC_REF_x);
// 其它
void AFE_ClkInit(AFE_ClkInitTypeDef *);
void AFE_ModuleClockCmd(u32 AFE_MODULE_x, u8 state);
void AFE_DebugInit(AFE_DebugInitTypeDef *);
void AFE_SetXtalCap(uint8_t *AFE_XCSEL_x);
#endif
