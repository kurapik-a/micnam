/**
 * @file
 * @brief        MCPWM库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
 *  */
#ifndef __lks45x_PWM_H
#define __lks45x_PWM_H

///< Includes ------------------------------------------------------------------
#include "basic.h"
#include "lks32mc45x.h"

typedef struct
{
    __IO s16 TH00;    ///< 0x00 MCPWM CH0_P比较门限值寄存器
    __IO u16 null1;   ///< 0x02 无意义 地址对齐
    __IO s16 TH01;    ///< 0x04 MCPWM CH0_N比较门限值寄存器
    __IO u16 null2;   ///< 0x02 无意义 地址对齐
    __IO s16 TH10;    ///< 0x08 MCPWM CH1_P比较门限值寄存器
    __IO u16 null3;   ///< 0x02 无意义 地址对齐
    __IO s16 TH11;    ///< 0x0C MCPWM CH1_N比较门限值寄存器
    __IO u16 null4;   ///< 0x02 无意义 地址对齐
    __IO s16 TH20;    ///< 0x10 MCPWM CH2_P比较门限值寄存器
    __IO u16 null5;   ///< 0x02 无意义 地址对齐
    __IO s16 TH21;    ///< 0x14 MCPWM CH2_N比较门限值寄存器
    __IO u16 null6;   ///< 0x02 无意义 地址对齐
    __IO s16 TH30;    ///< 0x18 MCPWM CH3_P比较门限值寄存器
    __IO u16 null7;   ///< 0x02 无意义 地址对齐
    __IO s16 TH31;    ///< 0x1C MCPWM CH3_N比较门限值寄存器
    __IO u16 null8;   ///< 0x02 无意义 地址对齐
    __IO s16 TMR0;    ///< 0x20 ADC采样定时器比较门限0寄存器
    __IO u16 null9;   ///< 0x02 无意义 地址对齐
    __IO s16 TMR1;    ///< 0x24 ADC采样定时器比较门限1寄存器
    __IO u16 null10;  ///< 0x02 无意义 地址对齐
    __IO s16 TMR2;    ///< 0x28 ADC采样定时器比较门限2寄存器
    __IO u16 null11;  ///< 0x02 无意义 地址对齐
    __IO s16 TMR3;    ///< 0x2C ADC采样定时器比较门限3寄存器
    __IO u16 null12;  ///< 0x02 无意义 地址对齐
    __IO u32 TH0;     ///< 0x30 MCPWM时基0门限值寄存器
    __IO u32 TH1;     ///< 0x34 MCPWM时基1门限值寄存器
    __IO u32 CNT0;    ///< 0x38 MCPWM时基0计数器寄存器
    __IO u32 CNT1;    ///< 0x3C MCPWM时基1计数器寄存器
    __IO u32 UPDATE;  ///< 0x40 MCPWM加载控制寄存器
    __IO u32 FCNT;    ///< 0x44 MCPWM FAIL时刻CNT值
    __IO u32 EVT0;    ///< 0x48 MCPWM时基0外部触发
    __IO u32 EVT1;    ///< 0x4C MCPWM时基1外部触发
    __IO u32 DTH00;   ///< 0x50 MCPWM CH0 N通道死区宽度控制寄存器
    __IO u32 DTH01;   ///< 0x54 MCPWM CH0 P通道死区宽度控制寄存器
    __IO u32 DTH10;   ///< 0x58 MCPWM CH1 N通道死区宽度控制寄存器
    __IO u32 DTH11;   ///< 0x5C MCPWM CH1 P通道死区宽度控制寄存器
    __IO u32 DTH20;   ///< 0x60 MCPWM CH2 N通道死区宽度控制寄存器
    __IO u32 DTH21;   ///< 0x64 MCPWM CH2 P通道死区宽度控制寄存器
    __IO u32 DTH30;   ///< 0x68 MCPWM CH3 N通道死区宽度控制寄存器
    __IO u32 DTH31;   ///< 0x6C MCPWM CH3 P通道死区宽度控制寄存器
    __IO u32 FLT;     ///< 0x70 MCPWM滤波时钟分频寄存器
    __IO u32 SDCFG;   ///< 0x74 MCPWM加载配置寄存器
    __IO u32 AUEN;    ///< 0x78 MCPWM自动更新使能寄存器
    __IO u32 TCLK;    ///< 0x7C MCPWM时钟分频控制寄存器
    __IO u32 IE0;     ///< 0x80 MCPWM时基0中断控制寄存器
    __IO u32 IF0;     ///< 0x84 MCPWM时基0中断标志位寄存器
    __IO u32 IE1;     ///< 0x88 MCPWM中断控制寄存器
    __IO u32 IF1;     ///< 0x8C MCPWM中断标志位寄存器
    __IO u32 EIE;     ///< 0x90 MCPWM异常中断控制寄存器
    __IO u32 EIF;     ///< 0x94 MCPWM异常中断标志位寄存器
    __IO u32 RE;      ///< 0x98 MCPWM DMA请求控制寄存器
    __IO u32 PP;      ///< 0x9C MCPWM推挽模式使能寄存器
    __IO u32 IO01;    ///< 0xA0 MCPWM CH0 CH1 IO控制寄存器
    __IO u32 IO23;    ///< 0xA4 MCPWM CH2 CH3 IO控制寄存器
    __IO u32 FAIL012; ///< 0xA8 MCPWM CH0 CH1 CH2短路控制寄存器
    __IO u32 FAIL3;   ///< 0xAC MCPWM CH3短路控制寄存器
    __IO u32 PRT;     ///< 0xB0 MCPWM保护寄存器
    __IO u32 null13;  ///< 0xB4 无意义 地址对齐
    __IO u32 CHMSK;   ///< 0xB8 MCPWM通道屏蔽位寄存器
} MCPWM_TypeDef;
#define MCPWM_REG_TypeDef MCPWM_TypeDef
typedef struct
{
    u16 TimeBase0_PERIOD; ///< 时期0周期设置
    u16 TimeBase1_PERIOD; ///< 时期0周期设置
    u8 CLK_DIV;           ///< 时期0周期设置
    u8 MCLK_EN;           ///< 时期0周期设置

    u8 TMR2_TimeBase_Sel; ///< TMR2 比较门限寄存器 时基选择 0:时基0 | 1:时基1
    u8 TMR3_TimeBase_Sel; ///< TMR2 比较门限寄存器 时基选择 0:时基0 | 1:时基1

    u8 MCPWM_Cnt0_EN;         ///< MCPWM 时基0主计数器使能开关
    u8 TimeBase0_Trig_Enable; ///< MCPWM 时基0主计数器使能开关
    s16 TimeBase0Init_CNT;    ///< MCPWM 时基0主计数器使能开关
    u16 TimeBase_TrigEvt0;    ///< MCPWM 时基0主计数器使能开关

    u8 MCPWM_Cnt1_EN;         ///< MCPWM 时基1主计数器使能开关
    u8 TimeBase1_Trig_Enable; ///< MCPWM 时基1主计数器使能开关
    s16 TimeBase1Init_CNT;    ///< MCPWM 时基1主计数器使能开关
    u16 TimeBase_TrigEvt1;    ///< MCPWM 时基1主计数器使能开关

    u16 MCPWM_WorkModeCH0; ///< MCPWM CH0工作模式：边沿对齐/中心对齐
    u16 MCPWM_WorkModeCH1; ///< MCPWM CH0工作模式：边沿对齐/中心对齐
    u16 MCPWM_WorkModeCH2; ///< MCPWM CH0工作模式：边沿对齐/中心对齐
    u16 MCPWM_WorkModeCH3; ///< MCPWM CH0工作模式：边沿对齐/中心对齐

    s16 TriggerPoint0; ///< PWM触发ADC事件0，时间点设置
    s16 TriggerPoint1; ///< PWM触发ADC事件0，时间点设置
    s16 TriggerPoint2; ///< PWM触发ADC事件0，时间点设置
    s16 TriggerPoint3; ///< PWM触发ADC事件0，时间点设置

    u16 DeadTimeCH0N; ///< CH0N死区时间设置　
    u16 DeadTimeCH0P; ///< CH0N死区时间设置　
    u16 DeadTimeCH1N; ///< CH0N死区时间设置　
    u16 DeadTimeCH1P; ///< CH0N死区时间设置　
    u16 DeadTimeCH2N; ///< CH0N死区时间设置　
    u16 DeadTimeCH2P; ///< CH0N死区时间设置　
    u16 DeadTimeCH3N; ///< CH0N死区时间设置　
    u16 DeadTimeCH3P; ///< CH0N死区时间设置　

    u8 CH0N_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出
    u8 CH0P_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出
    u8 CH1N_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出
    u8 CH1P_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出
    u8 CH2N_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出
    u8 CH2P_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出
    u8 CH3N_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出
    u8 CH3P_Polarity_INV; ///< CH0N输出极性取反，0:正常输出；1:取反输出

    u8 Switch_CH0N_CH0P; ///< 交换CH0N, CH0P信号输出使能开关
    u8 Switch_CH1N_CH1P; ///< 交换CH0N, CH0P信号输出使能开关
    u8 Switch_CH2N_CH2P; ///< 交换CH0N, CH0P信号输出使能开关
    u8 Switch_CH3N_CH3P; ///< 交换CH0N, CH0P信号输出使能开关

    u8 MCPWM_UpdateInterval;   ///< 时基 0 更新间隔。
    u8 MCPWM_Base0T0_UpdateEN; ///< 时基 0 t0（起点）事件更新使能。
    u8 MCPWM_Base0T1_UpdateEN; ///< 时基 0 t1（过零）事件更新使能。
    u8 MCPWM_Base1T0_UpdateEN; ///< 时基 1 t0（起点）事件更新使能。
    u8 MCPWM_Base1T1_UpdateEN; ///< 时基 1 t1（过零）事件更新使能。
    u8 MCPWM_Auto_ERR_EN;      ///< 更新事件自动清除MCPWM_EIF[5:4]并置位MCPWM_FAIL012.MOE，恢复MCPWM通道0/1/2信号输出。
    u8 MCPWM_UpdateInterval1;  ///< 时基 1 更新间隔。
    u8 MCPWM_Auto_ERR_EN1;     ///< 更新事件是否自动清除MCPWM_EIF[7:6]并置位MCPWM_FAIL3.MOE，恢复MCPWM通道3信号输出。

    u8 DebugMode_PWM_out; ///< Debug时，MCU进入Halt, MCPWM信号是否正常输出

    u8 GPIO_BKIN_Filter; ///< GPIO输入滤波时钟设置1-16
    u8 CMP_BKIN_Filter;  ///< GPIO输入滤波时钟设置1-16

    u8 FAIL0_INPUT_EN;   ///< FAIL0 输入功能使能
    u8 FAIL0_INT_EN;     ///< FAIL0 输入功能使能
    u8 FAIL0_Signal_Sel; ///< FAIL0 输入功能使能
    u8 FAIL0_Polarity;   ///< FAIL0 输入功能使能

    u8 FAIL1_INPUT_EN;   ///< FAIL1 输入功能使能
    u8 FAIL1_INT_EN;     ///< FAIL1 输入功能使能
    u8 FAIL1_Signal_Sel; ///< FAIL1 输入功能使能
    u8 FAIL1_Polarity;   ///< FAIL1 输入功能使能

    u8 FAIL2_INPUT_EN;   ///< FAIL2 输入功能使能
    u8 FAIL2_INT_EN;     ///< FAIL2 输入功能使能
    u8 FAIL2_Signal_Sel; ///< FAIL2 输入功能使能
    u8 FAIL2_Polarity;   ///< FAIL2 输入功能使能

    u8 FAIL3_INPUT_EN;   ///< FAIL3 输入功能使能
    u8 FAIL3_INT_EN;     ///< FAIL3 输入功能使能
    u8 FAIL3_Signal_Sel; ///< FAIL3 输入功能使能
    u8 FAIL3_Polarity;   ///< FAIL3 输入功能使能

    u8 CH0P_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出
    u8 CH0N_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出
    u8 CH1P_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出
    u8 CH1N_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出
    u8 CH2P_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出
    u8 CH2N_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出
    u8 CH3P_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出
    u8 CH3N_default_output; ///< CH0P MOE为0时或发生FAIL事件时，默认电平输出

    u8 T0_Update_INT_EN_BASE0;  ///< T0更新事件中断使能
    u8 T1_Update_INT_EN_BASE0;  ///< T0更新事件中断使能
    u8 TMR0_Match_INT_EN_BASE0; ///< T0更新事件中断使能
    u8 TMR1_Match_INT_EN_BASE0; ///< T0更新事件中断使能
    u8 TMR2_Match_INT_EN_BASE0; ///< T0更新事件中断使能
    u8 TMR3_Match_INT_EN_BASE0; ///< T0更新事件中断使能

    u8 T0_Update_INT_EN_BASE1;  ///< T1更新事件中断使能
    u8 T1_Update_INT_EN_BASE1;  ///< T1更新事件中断使能
    u8 TMR0_Match_INT_EN_BASE1; ///< T1更新事件中断使能
    u8 TMR1_Match_INT_EN_BASE1; ///< T1更新事件中断使能
    u8 TMR2_Match_INT_EN_BASE1; ///< T1更新事件中断使能
    u8 TMR3_Match_INT_EN_BASE1; ///< T1更新事件中断使能

    u8 null;  ///< 地址对齐
    u16 AUEN; ///< 自动更新使能寄存器

} MCPWM_InitTypeDef;

#define MCPWM_IE_FAIL3 BIT19 ///< FAIL3 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIF
#define MCPWM_IE_FAIL2 BIT18 ///< FAIL2 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIF
#define MCPWM_IE_FAIL1 BIT17 ///< FAIL1 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIE
#define MCPWM_IE_FAIL0 BIT16 ///< FAIL0 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIE
#define MCPWM_IE_UP BIT14    ///< MCPWM_TH/MCPWM_TH00~MCPWM_TH31/MCPWM_TMR0~MCPWM_TMR3 等寄存器更新到 MCPWM 实际运行系统的中断源使能
#define MCPWM_IE_TMR3 BIT13  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR3 中断源使能。
#define MCPWM_IE_TMR2 BIT12  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR2 中断源使能。
#define MCPWM_IE_TMR1 BIT11  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR1 中断源使能。
#define MCPWM_IE_TMR0 BIT10  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR0 中断源使能。
#define MCPWM_IE_TH31 BIT9   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH31 中断源使能。
#define MCPWM_IE_TH30 BIT8   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH30 中断源使能。
#define MCPWM_IE_TH21 BIT7   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH21 中断源使能。
#define MCPWM_IE_TH20 BIT6   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH20 中断源使能。
#define MCPWM_IE_TH11 BIT5   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH11 中断源使能。
#define MCPWM_IE_TH10 BIT4   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH10 中断源使能。
#define MCPWM_IE_TH01 BIT3   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH01 中断源使能。
#define MCPWM_IE_TH00 BIT2   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH00 中断源使能。
#define MCPWM_IE_T1 BIT1     ///< t1 事件，计数器的计数值到达 0 中断源使能。
#define MCPWM_IE_T0 BIT0     ///< t0 事件，计数器的计数值回到 MCPWM_TH 中断源使能。

#define MCPWM_IF_FAIL3 BIT19 ///< FAIL3 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIF
#define MCPWM_IF_FAIL2 BIT18 ///< FAIL2 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIF
#define MCPWM_IF_FAIL1 BIT17 ///< FAIL1 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIF
#define MCPWM_IF_FAIL0 BIT16 ///< FAIL0 中断源事件。 警告，虚拟位，实际指向 MCPWM_EIF
#define MCPWM_IF_UP BIT14    ///< MCPWM_TH/MCPWM_TH00~MCPWM_TH31/MCPWM_TMR0~MCPWM_TMR3 等寄存器更新到 MCPWM 实际运行系统的中断源事件。
#define MCPWM_IF_TMR3 BIT13  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR3 中断源事件。
#define MCPWM_IF_TMR2 BIT12  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR2 中断源事件。
#define MCPWM_IF_TMR1 BIT11  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR1 中断源事件。
#define MCPWM_IF_TMR0 BIT10  ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TMR0 中断源事件。
#define MCPWM_IF_TH31 BIT9   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH31 中断源事件。
#define MCPWM_IF_TH30 BIT8   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH30 中断源事件。
#define MCPWM_IF_TH21 BIT7   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH21 中断源事件。
#define MCPWM_IF_TH20 BIT6   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH20 中断源事件。
#define MCPWM_IF_TH11 BIT5   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH11 中断源事件。
#define MCPWM_IF_TH10 BIT4   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH10 中断源事件。
#define MCPWM_IF_TH01 BIT3   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH01 中断源事件。
#define MCPWM_IF_TH00 BIT2   ///< MCPWM 实际运行系统中计数器的计数值等于 MCPWM_TH00 中断源事件。
#define MCPWM_IF_T1 BIT1     ///< t1 事件，计数器的计数值到达 0 中断源事件。
#define MCPWM_IF_T0 BIT0     ///< t0 事件，计数器的计数值回到 MCPWM_TH 中断源事件。

#define MCPWM_EIF_FAIL3 BIT7 ///< FAIL3 中断源事件。
#define MCPWM_EIF_FAIL2 BIT6 ///< FAIL2 中断源事件。
#define MCPWM_EIF_FAIL1 BIT5 ///< FAIL1 中断源事件。
#define MCPWM_EIF_FAIL0 BIT4 ///< FAIL0 中断源事件。

#define MCPWM_EIE_FAIL3 BIT7 ///< FAIL3 中断源事件。
#define MCPWM_EIE_FAIL2 BIT6 ///< FAIL2 中断源事件。
#define MCPWM_EIE_FAIL1 BIT5 ///< FAIL1 中断源事件。
#define MCPWM_EIE_FAIL0 BIT4 ///< FAIL0 中断源事件。

#define MCPWM_MOE_ENABLE_MASK ((u16)0x0040)   ///< 打开MOE位MASK位
#define MCPWM_MOE_DISABLE_MASK ((u16)~0x0040) ///< 打开MOE位MASK位

#define MCPWM_F_SELECT_CMP0 BIT0 ///< 比较器信号选择
#define MCPWM_F_SELECT_CMP1 BIT1
#define MCPWM_F_LFAIL_CH0 BIT2
#define MCPWM_F_LFAIL_CH1 BIT3
#define MCPWM_F_ENABLE_CH0 BIT4
#define MCPWM_F_ENABLE_CH1 BIT5

#define MCPWM_ENABLE_BK0IF BIT4
#define MCPWM_ENABLE_BK1IF BIT5
#define MCPWM_ENABLE_CMP0IF BIT6
#define MCPWM_ENABLE_CMP1IF BIT7

#define MCPWM_TMR0_IF BIT10 ///< 计数值等于 MCPWM_TMR0 中断源事件

#define MCPWM_AUEN_TH00 BIT0 ///< MCPWM_TH00 自动加载使能
#define MCPWM_AUEN_TH01 BIT1
#define MCPWM_AUEN_TH10 BIT2
#define MCPWM_AUEN_TH11 BIT3
#define MCPWM_AUEN_TH20 BIT4
#define MCPWM_AUEN_TH21 BIT5
#define MCPWM_AUEN_TH30 BIT6
#define MCPWM_AUEN_TH31 BIT7
#define MCPWM_AUEN_TMR0 BIT8
#define MCPWM_AUEN_TMR1 BIT9
#define MCPWM_AUEN_TMR2 BIT10
#define MCPWM_AUEN_TMR3 BIT11
#define MCPWM_AUEN_TH0 BIT12  ///< MCPWM_0TH 自动加载使能
#define MCPWM_AUEN_TH1 BIT13  ///< MCPWM_0TH 自动加载使能
#define MCPWM_AUEN_CNT0 BIT14 ///< MCPWM_0TH 自动加载使能
#define MCPWM_AUEN_CNT1 BIT15 ///< MCPWM_0TH 自动加载使能

#define MCPWM_EVT_TIMER3_CMP1 BIT15 ///< [15] TIMER3 CMP1事件触发时基0开始计数
#define MCPWM_EVT_TIMER3_CMP0 BIT14 ///< [14] TIMER3 CMP0事件触发时基0开始计数
#define MCPWM_EVT_TIMER2_CMP1 BIT13 ///< [13] TIMER2 CMP1事件触发时基0开始计数
#define MCPWM_EVT_TIMER2_CMP0 BIT12 ///< [12] TIMER2 CMP0事件触发时基0开始计数
#define MCPWM_EVT_TIMER1_CMP1 BIT11 ///< [11] TIMER1 CMP1事件触发时基0开始计数
#define MCPWM_EVT_TIMER1_CMP0 BIT10 ///< [10] TIMER1 CMP0事件触发时基0开始计数
#define MCPWM_EVT_TIMER0_CMP1 BIT9  ///< [9]  TIMER0 CMP1事件触发时基0开始计数
#define MCPWM_EVT_TIMER0_CMP0 BIT8  ///< [8]  TIMER0 CMP0事件触发时基0开始计数
#define MCPWM_EVT_MCPWM1_TMR3 BIT7  ///< [7]  MCPWM1 TMR3事件触发时基0开始计数
#define MCPWM_EVT_MCPWM1_TMR2 BIT6  ///< [6]  MCPWM1 TMR2事件触发时基0开始计数
#define MCPWM_EVT_MCPWM1_TMR1 BIT5  ///< [5]  MCPWM1 TMR1事件触发时基0开始计数
#define MCPWM_EVT_MCPWM1_TMR0 BIT4  ///< [4]  MCPWM1 TMR0事件触发时基0开始计数
#define MCPWM_EVT_MCPWM0_TMR3 BIT3  ///< [3]  MCPWM0 TMR3事件触发时基0开始计数
#define MCPWM_EVT_MCPWM0_TMR2 BIT2  ///< [2]  MCPWM0 TMR2事件触发时基0开始计数
#define MCPWM_EVT_MCPWM0_TMR1 BIT1  ///< [1]  MCPWM0 TMR1事件触发时基0开始计数
#define MCPWM_EVT_MCPWM0_TMR0 BIT0  ///< [0]  MCPWM0 TMR0事件触发时基0开始计数

// 保留，45系列不支持该操作
// #define MCPWM_OUT_CHN_0         BIT0
// #define MCPWM_OUT_CHN_1         BIT1
// #define MCPWM_OUT_CHN_2         BIT2
#define MCPWM_OUT_CHN_012 7
#define MCPWM_OUT_CHN_3 BIT3

#define MCPWM_CNT_0 0 ///< MCPWM CNT0
#define MCPWM_CNT_1 1 ///< MCPWM CNT0

#define MCPWM_CENTRAL_PWM_MODE 0 ///< 中心对齐PWM模式
#define MCPWM_EDGE_PWM_MODE 1    ///< 中心对齐PWM模式

#define MCPWM_HIGH_LEVEL_VALID 0 ///< 高电平有效
#define MCPWM_LOW_LEVEL_VALID 1  ///< 高电平有效

#define MCPWM_FAIL_SEL_CMP 1 ///< Fail事件来源比较器
#define MCPWM_FAIL_SEL_IO 0  ///< Fail事件来源比较器

#define MCPWM_SWITCH_ENABLE 0  ///< 通道交换打开
#define MCPWM_SWITCH_DISABLE 1 ///< 通道交换打开

#define MCPWM0_UPDATE_REG()      \
    {                            \
        MCPWM0_PRT = 0x0000DEAD; \
        MCPWM0_UPDATE = 0x00ff;  \
        MCPWM0_PRT = 0x0000CAFE; \
    }
#define MCPWM1_UPDATE_REG()      \
    {                            \
        MCPWM1_PRT = 0x0000DEAD; \
        MCPWM1_UPDATE = 0x00ff;  \
        MCPWM1_PRT = 0x0000CAFE; \
    }

// 解决旧版程序的兼容性问题
#define MCPWM_HIGH_LEVEL 1  ///< 高电平
#define MCPWM_LOW_LEVEL 0   ///< 高电平
#define MCPWM_FAIL0_IF BIT4 ///< 高电平
#define MCPWM_FAIL1_IF BIT5 ///< 高电平
#define MCPWM_FAIL2_IF BIT6 ///< 高电平
#define MCPWM_FAIL3_IF BIT7 ///< 高电平
#define MCPWM_TH00_AUEN MCPWM_AUEN_TH00
#define MCPWM_TH01_AUEN MCPWM_AUEN_TH01
#define MCPWM_TH10_AUEN MCPWM_AUEN_TH10
#define MCPWM_TH11_AUEN MCPWM_AUEN_TH11
#define MCPWM_TH20_AUEN MCPWM_AUEN_TH20
#define MCPWM_TH21_AUEN MCPWM_AUEN_TH21
#define MCPWM_TH30_AUEN MCPWM_AUEN_TH30
#define MCPWM_TH31_AUEN MCPWM_AUEN_TH31
#define MCPWM_TMR0_AUEN MCPWM_AUEN_TMR0
#define MCPWM_TMR1_AUEN MCPWM_AUEN_TMR1
#define MCPWM_TMR2_AUEN MCPWM_AUEN_TMR2
#define MCPWM_TMR3_AUEN MCPWM_AUEN_TMR3
#define MCPWM_TH0_AUEN MCPWM_AUEN_TH0
#define MCPWM_TH1_AUEN MCPWM_AUEN_TH1
#define MCPWM_CNT0_AUEN MCPWM_AUEN_CNT0
#define MCPWM_CNT1_AUEN MCPWM_AUEN_CNT1

#define MCPWM_OUT_MODE_DEFAULT 0 ///< mcpwm输出默认值
#define MCPWM_OUT_MODE_PWM 1     ///< mcpwm输出PWM

void MCPWM_Init(MCPWM_TypeDef *MCPWMx, MCPWM_InitTypeDef *MCPWM_InitStruct);
void MCPWM_StructInit(MCPWM_InitTypeDef *MCPWM_InitStruct);

void MCPWM_ClearIRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x, u8 MCPWM_CNT_x);
u8 MCPWM_GetIRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x, u8 MCPWM_CNT_x);
u8 MCPWM_GetFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x, u8 MCPWM_CNT_x);

u8 MCPWM_GetCnt0Flag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x);
u8 MCPWM_GetCnt1Flag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x);
u8 MCPWM_GetCnt0IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x);
u8 MCPWM_GetCnt1IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x);
void MCPWM_ClearCnt0IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x);
void MCPWM_ClearCnt1IRQFlag(MCPWM_TypeDef *MCPWMx, u32 MCPWM_IF_x);

void MCPWM_Enable(MCPWM_TypeDef *MCPWMx);
void MCPWM_Disable(MCPWM_TypeDef *MCPWMx);

void MCPWM_OutputMode(MCPWM_TypeDef *MCPWMx, u8 MCPWM_OUT_CHN_x, u8 MCPWM_OUT_MODE_x);

#endif /*__CHANHOM_PWM_H */
