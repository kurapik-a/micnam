/**
 * @file
 * @brief        ADC库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
 */
#ifndef _LKS32MC45X_ADC_H_
#define _LKS32MC45X_ADC_H_

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO s16 DAT0;
    __IO u16 null1;
    __IO s16 DAT1;
    __IO u16 null2;
    __IO s16 DAT2;
    __IO u16 null3;
    __IO s16 DAT3;
    __IO u16 null4;
    __IO s16 DAT4;
    __IO u16 null5;
    __IO s16 DAT5;
    __IO u16 null6;
    __IO s16 DAT6;
    __IO u16 null7;
    __IO s16 DAT7;
    __IO u16 null8;
    __IO s16 DAT8;
    __IO u16 null9;
    __IO s16 DAT9;
    __IO u16 null10;
    __IO s16 DAT10;
    __IO u16 null11;
    __IO s16 DAT11;
    __IO u16 null12;
    __IO s16 DAT12;
    __IO u16 null13;
    __IO s16 DAT13;
    __IO u16 null14;
    __IO s16 DAT14;
    __IO u16 null15;
    __IO s16 DAT15;
    __IO u16 null16;

    __IO u32 PCHN0;
    __IO u32 PCHN1;
    __IO u32 PCHN2;
    __IO u32 PCHN3;
    __IO u32 NCHN0;
    __IO u32 NCHN1;
    __IO u32 NCHN2;
    __IO u32 NCHN3;

    __IO u32 CHNT; ///< ADCx 各种触发模式下采样次数
    __IO u32 null;
    __IO u32 null17;
    __IO u32 null18;
    __IO u32 GAIN;
    __IO u32 CFG;
    __IO u32 TRIG; ///< ADCx 采样触发配置
    __IO u32 SWT;  ///< ADCx 软件触发

    __IO u32 DC0;
    __IO u32 AMC0;
    __IO u32 DC1;
    __IO u32 AMC1;

    __IO u32 IE;
    __IO u32 IF;

    __IO u32 TH0;  ///< ADCx 阈值 0
    __IO u32 GEN0; ///< ADCx 阈值 0 监测使能
    __IO u32 TH1;
    __IO u32 GEN1;
} ADC_TypeDef;

typedef struct
{
    u16 IE;    ///< ADC中断使能
    u16 RE;    ///< ADC中断使能
    u16 Align; ///< 采样数据对齐方式
    u16 GAIN;  ///< 通道增益
    u16 TROVS; ///< 过采样
    u16 OVSR;  ///< 过采样率
    u16 CSMP;  ///< 连续采样模式
    u16 TCNT;  ///< 触发一次采样所需的触发事件数
    u16 S1;    ///< 第一段采样的总通道数
    u16 S2;    ///< 第二段采样的总通道数
    u16 NSMP;  ///< ADC 触发模式 单段或双段
    u32 TRIG;  ///< MCPWM触发采样使能
    u16 HTH0;  ///< 看门狗 0 上阈值
    u16 LTH0;  ///< 看门狗 0 下阈值
    u16 GEN0;  ///< 看门狗 0 通道使能
    u16 HTH1;  ///< 看门狗 1 上阈值
    u16 LTH1;  ///< 看门狗 1 下阈值
    u16 GEN1;  ///< 看门狗 1 通道使能
} ADC_InitTypeDef;

#define ADC_IF_HERR BIT6 ///< 硬件触发发生在非空闲状态中断标志
#define ADC_IF_SERR BIT5 ///< 软件触发发生在非空闲状态中断标志
#define ADC_IF_AWD1 BIT3 ///< 阈值监测1超限中断标志
#define ADC_IF_AWD0 BIT2 ///< 阈值监测0超限中断标志
#define ADC_IF_SF2 BIT1  ///< 第二段采样完成中断标志
#define ADC_IF_SF1 BIT0  ///< 第一段采样完成中断标志

#define ADC_RE_HERR BIT14 ///< 硬件触发发生在非空闲状态DMA请求使能
#define ADC_RE_SERR BIT13 ///< 软件触发发生在非空闲状态DMA请求使能
#define ADC_RE_AWD1 BIT11 ///< 阈值监测1超限DMA请求使能
#define ADC_RE_AWD0 BIT10 ///< 阈值监测0超限DMA请求使能
#define ADC_RE_SF2 BIT9   ///< 第二段采样完成DMA请求使能
#define ADC_RE_SF1 BIT8   ///< 第一段采样完成DMA请求使能

#define ADC_IE_HERR BIT6 ///< 硬件触发发生在非空闲状态中断标志
#define ADC_IE_SERR BIT5 ///< 软件触发发生在非空闲状态中断标志
#define ADC_IE_AWD1 BIT3 ///< 阈值监测1超限中断标志
#define ADC_IE_AWD0 BIT2 ///< 阈值监测0超限中断标志
#define ADC_IE_SF2 BIT1  ///< 第二段采样完成中断标志
#define ADC_IE_SF1 BIT0  ///< 第一段采样完成中断标志

#define ADC_TRIG_UTIMER4_CMP1_EN BIT25 ///< UTIMER4比较事件1触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER4_CMP0_EN BIT24 ///< UTIMER4比较事件0触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER3_CMP1_EN BIT23 ///< UTIMER3比较事件1触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER3_CMP0_EN BIT22 ///< UTIMER3比较事件0触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER2_CMP1_EN BIT21 ///< UTIMER2比较事件1触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER2_CMP0_EN BIT20 ///< UTIMER2比较事件0触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER1_CMP1_EN BIT19 ///< UTIMER1比较事件1触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER1_CMP0_EN BIT18 ///< UTIMER1比较事件0触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER0_CMP1_EN BIT17 ///< UTIMER0比较事件1触发ADC采样使能，高有效
#define ADC_TRIG_UTIMER0_CMP0_EN BIT16 ///< UTIMER0比较事件0触发ADC采样使能，高有效
#define ADC_TRIG_MCPWM1_T3_EN BIT7     ///< MCPWM1 TMR3事件触发ADC采样使能，高有效
#define ADC_TRIG_MCPWM1_T2_EN BIT6     ///< MCPWM1 TMR2事件触发ADC采样使能，高有效
#define ADC_TRIG_MCPWM1_T1_EN BIT5     ///< MCPWM1 TMR1事件触发ADC采样使能，高有效
#define ADC_TRIG_MCPWM1_T0_EN BIT4     ///< MCPWM1 TMR0事件触发ADC采样使能，高有效

#define ADC_TRIG_MCPWM0_T3_EN BIT3 ///< MCPWM0 T3事件触发ADC采样使能，高有效
#define ADC_TRIG_MCPWM0_T2_EN BIT2 ///< MCPWM0 T2事件触发ADC采样使能，高有效
#define ADC_TRIG_MCPWM0_T1_EN BIT1 ///< MCPWM0 T1事件触发ADC采样使能，高有效
#define ADC_TRIG_MCPWM0_T0_EN BIT0 ///< MCPWM0 T0事件触发ADC采样使能，高有效
#define ADC_TRIG_NONE 0            ///< ADC不使用硬件触发

#define ADC_ALIGN_LEFT ((u16)0x0000)  ///< ADC数据输出左对齐
#define ADC_ALIGN_RIGHT ((u16)0x0001) ///< ADC数据输出右对齐

#define ADC_CHANNEL_0 ((u16)0x00)  ///< ADC通道0
#define ADC_CHANNEL_1 ((u16)0x01)  ///< ADC通道1
#define ADC_CHANNEL_2 ((u16)0x02)  ///< ADC通道2
#define ADC_CHANNEL_3 ((u16)0x03)  ///< ADC通道3
#define ADC_CHANNEL_4 ((u16)0x04)  ///< ADC通道4
#define ADC_CHANNEL_5 ((u16)0x05)  ///< ADC通道5
#define ADC_CHANNEL_6 ((u16)0x06)  ///< ADC通道6
#define ADC_CHANNEL_7 ((u16)0x07)  ///< ADC通道7
#define ADC_CHANNEL_8 ((u16)0x08)  ///< ADC通道8
#define ADC_CHANNEL_9 ((u16)0x09)  ///< ADC通道9
#define ADC_CHANNEL_10 ((u16)0x0a) ///< ADC通道10
#define ADC_CHANNEL_11 ((u16)0x0b) ///< ADC通道11
#define ADC_CHANNEL_12 ((u16)0x0c) ///< ADC通道12
#define ADC_CHANNEL_13 ((u16)0x0d) ///< ADC通道13
#define ADC_CHANNEL_14 ((u16)0x0e) ///< ADC通道14
#define ADC_CHANNEL_15 ((u16)0x0f) ///< ADC通道15

#define ADC_GAIN_HIGH_DAT0 BIT0   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT1 BIT1   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT2 BIT2   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT3 BIT3   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT4 BIT4   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT5 BIT5   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT6 BIT6   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT7 BIT7   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT8 BIT8   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT9 BIT9   ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT10 BIT10 ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT11 BIT11 ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT12 BIT12 ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT13 BIT13 ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT14 BIT14 ///< ADC高增益使能
#define ADC_GAIN_HIGH_DAT15 BIT15 ///< ADC高增益使能

#define ADC_TROVS_ONES 0 ///< 过采样触发模式，一次触发，触发一次采样OVSR次
#define ADC_TROVS_MANY 1 ///< 过采样触发模式，多次触发，触发一次采样一次，需要触发OVSR次

#define ADC_OVSR_1 0   ///< 过采样率，实际结果为 1   次采样后的平均值
#define ADC_OVSR_2 1   ///< 过采样率，实际结果为 2   次采样后的平均值
#define ADC_OVSR_4 2   ///< 过采样率，实际结果为 4   次采样后的平均值
#define ADC_OVSR_8 3   ///< 过采样率，实际结果为 8   次采样后的平均值
#define ADC_OVSR_16 4  ///< 过采样率，实际结果为 16  次采样后的平均值
#define ADC_OVSR_32 5  ///< 过采样率，实际结果为 32  次采样后的平均值
#define ADC_OVSR_64 6  ///< 过采样率，实际结果为 64  次采样后的平均值
#define ADC_OVSR_128 7 ///< 过采样率，实际结果为 128 次采样后的平均值

#define ADC_CSMP_DISABLE 0 ///< 关闭连续采样模式
#define ADC_CSMP_ENABLE 1  ///< 打开连续采样模式，ADC采样完成之后自动开启下一轮次采样

#define ADC_TCNT_1 0   ///< 触发 1  次采样1次
#define ADC_TCNT_2 1   ///< 触发 2  次采样1次
#define ADC_TCNT_3 2   ///< 触发 3  次采样1次
#define ADC_TCNT_4 3   ///< 触发 4  次采样1次
#define ADC_TCNT_5 4   ///< 触发 5  次采样1次
#define ADC_TCNT_6 5   ///< 触发 6  次采样1次
#define ADC_TCNT_7 6   ///< 触发 7  次采样1次
#define ADC_TCNT_8 7   ///< 触发 8  次采样1次
#define ADC_TCNT_9 8   ///< 触发 9  次采样1次
#define ADC_TCNT_10 9  ///< 触发 10 次采样1次
#define ADC_TCNT_11 10 ///< 触发 11 次采样1次
#define ADC_TCNT_12 11 ///< 触发 12 次采样1次
#define ADC_TCNT_13 12 ///< 触发 13 次采样1次
#define ADC_TCNT_14 13 ///< 触发 14 次采样1次
#define ADC_TCNT_15 14 ///< 触发 15 次采样1次
#define ADC_TCNT_16 15 ///< 触发 16 次采样1次

#define ADC_S1_1 1   ///< ADC第 1 段采样 1  个通道
#define ADC_S1_2 2   ///< ADC第 1 段采样 2  个通道
#define ADC_S1_3 3   ///< ADC第 1 段采样 3  个通道
#define ADC_S1_4 4   ///< ADC第 1 段采样 4  个通道
#define ADC_S1_5 5   ///< ADC第 1 段采样 5  个通道
#define ADC_S1_6 6   ///< ADC第 1 段采样 6  个通道
#define ADC_S1_7 7   ///< ADC第 1 段采样 7  个通道
#define ADC_S1_8 8   ///< ADC第 1 段采样 8  个通道
#define ADC_S1_9 9   ///< ADC第 1 段采样 9  个通道
#define ADC_S1_10 10 ///< ADC第 1 段采样 10 个通道
#define ADC_S1_11 11 ///< ADC第 1 段采样 11 个通道
#define ADC_S1_12 12 ///< ADC第 1 段采样 12 个通道
#define ADC_S1_13 13 ///< ADC第 1 段采样 13 个通道
#define ADC_S1_14 14 ///< ADC第 1 段采样 14 个通道
#define ADC_S1_15 15 ///< ADC第 1 段采样 15 个通道

#define ADC_S2_1 1   ///< ADC第 2 段采样 1  个通道
#define ADC_S2_2 2   ///< ADC第 2 段采样 2  个通道
#define ADC_S2_3 3   ///< ADC第 2 段采样 3  个通道
#define ADC_S2_4 4   ///< ADC第 2 段采样 4  个通道
#define ADC_S2_5 5   ///< ADC第 2 段采样 5  个通道
#define ADC_S2_6 6   ///< ADC第 2 段采样 6  个通道
#define ADC_S2_7 7   ///< ADC第 2 段采样 7  个通道
#define ADC_S2_8 8   ///< ADC第 2 段采样 8  个通道
#define ADC_S2_9 9   ///< ADC第 2 段采样 9  个通道
#define ADC_S2_10 10 ///< ADC第 2 段采样 10 个通道
#define ADC_S2_11 11 ///< ADC第 2 段采样 11 个通道
#define ADC_S2_12 12 ///< ADC第 2 段采样 12 个通道
#define ADC_S2_13 13 ///< ADC第 2 段采样 13 个通道
#define ADC_S2_14 14 ///< ADC第 2 段采样 14 个通道
#define ADC_S2_15 15 ///< ADC第 2 段采样 15 个通道

#define ADC_NSMP_1 0 ///< ADC单段触发
#define ADC_NSMP_2 1 ///< ADC两段触发

#define ADC_GEN0_NONE 0      ///< ADC模拟看门狗 0 不监控任何数据
#define ADC_GEN0_DAT0 BIT0   ///< ADC模拟看门狗 0 监控 DAT0  的数据
#define ADC_GEN0_DAT1 BIT1   ///< ADC模拟看门狗 0 监控 DAT1  的数据
#define ADC_GEN0_DAT2 BIT2   ///< ADC模拟看门狗 0 监控 DAT2  的数据
#define ADC_GEN0_DAT3 BIT3   ///< ADC模拟看门狗 0 监控 DAT3  的数据
#define ADC_GEN0_DAT4 BIT4   ///< ADC模拟看门狗 0 监控 DAT4  的数据
#define ADC_GEN0_DAT5 BIT5   ///< ADC模拟看门狗 0 监控 DAT5  的数据
#define ADC_GEN0_DAT6 BIT6   ///< ADC模拟看门狗 0 监控 DAT6  的数据
#define ADC_GEN0_DAT7 BIT7   ///< ADC模拟看门狗 0 监控 DAT7  的数据
#define ADC_GEN0_DAT8 BIT8   ///< ADC模拟看门狗 0 监控 DAT8  的数据
#define ADC_GEN0_DAT9 BIT9   ///< ADC模拟看门狗 0 监控 DAT9  的数据
#define ADC_GEN0_DAT10 BIT10 ///< ADC模拟看门狗 0 监控 DAT10 的数据
#define ADC_GEN0_DAT11 BIT11 ///< ADC模拟看门狗 0 监控 DAT11 的数据
#define ADC_GEN0_DAT12 BIT12 ///< ADC模拟看门狗 0 监控 DAT12 的数据
#define ADC_GEN0_DAT13 BIT13 ///< ADC模拟看门狗 0 监控 DAT13 的数据
#define ADC_GEN0_DAT14 BIT14 ///< ADC模拟看门狗 0 监控 DAT14 的数据
#define ADC_GEN0_DAT15 BIT15 ///< ADC模拟看门狗 0 监控 DAT15 的数据

#define ADC_GEN1_NONE 0      ///< ADC模拟看门狗 1 不监控任何数据
#define ADC_GEN1_DAT0 BIT0   ///< ADC模拟看门狗 1 监控 DAT0  的数据
#define ADC_GEN1_DAT1 BIT1   ///< ADC模拟看门狗 1 监控 DAT1  的数据
#define ADC_GEN1_DAT2 BIT2   ///< ADC模拟看门狗 1 监控 DAT2  的数据
#define ADC_GEN1_DAT3 BIT3   ///< ADC模拟看门狗 1 监控 DAT3  的数据
#define ADC_GEN1_DAT4 BIT4   ///< ADC模拟看门狗 1 监控 DAT4  的数据
#define ADC_GEN1_DAT5 BIT5   ///< ADC模拟看门狗 1 监控 DAT5  的数据
#define ADC_GEN1_DAT6 BIT6   ///< ADC模拟看门狗 1 监控 DAT6  的数据
#define ADC_GEN1_DAT7 BIT7   ///< ADC模拟看门狗 1 监控 DAT7  的数据
#define ADC_GEN1_DAT8 BIT8   ///< ADC模拟看门狗 1 监控 DAT8  的数据
#define ADC_GEN1_DAT9 BIT9   ///< ADC模拟看门狗 1 监控 DAT9  的数据
#define ADC_GEN1_DAT10 BIT10 ///< ADC模拟看门狗 1 监控 DAT10 的数据
#define ADC_GEN1_DAT11 BIT11 ///< ADC模拟看门狗 1 监控 DAT11 的数据
#define ADC_GEN1_DAT12 BIT12 ///< ADC模拟看门狗 1 监控 DAT12 的数据
#define ADC_GEN1_DAT13 BIT13 ///< ADC模拟看门狗 1 监控 DAT13 的数据
#define ADC_GEN1_DAT14 BIT14 ///< ADC模拟看门狗 1 监控 DAT14 的数据
#define ADC_GEN1_DAT15 BIT15 ///< ADC模拟看门狗 1 监控 DAT15 的数据

#define ADC_DAT_0 0   ///< ADC模拟看门狗 1 监控 DAT0  的数据
#define ADC_DAT_1 1   ///< ADC模拟看门狗 1 监控 DAT1  的数据
#define ADC_DAT_2 2   ///< ADC模拟看门狗 1 监控 DAT2  的数据
#define ADC_DAT_3 3   ///< ADC模拟看门狗 1 监控 DAT3  的数据
#define ADC_DAT_4 4   ///< ADC模拟看门狗 1 监控 DAT4  的数据
#define ADC_DAT_5 5   ///< ADC模拟看门狗 1 监控 DAT5  的数据
#define ADC_DAT_6 6   ///< ADC模拟看门狗 1 监控 DAT6  的数据
#define ADC_DAT_7 7   ///< ADC模拟看门狗 1 监控 DAT7  的数据
#define ADC_DAT_8 8   ///< ADC模拟看门狗 1 监控 DAT8  的数据
#define ADC_DAT_9 9   ///< ADC模拟看门狗 1 监控 DAT9  的数据
#define ADC_DAT_10 10 ///< ADC模拟看门狗 1 监控 DAT10 的数据
#define ADC_DAT_11 11 ///< ADC模拟看门狗 1 监控 DAT11 的数据
#define ADC_DAT_12 12 ///< ADC模拟看门狗 1 监控 DAT12 的数据
#define ADC_DAT_13 13 ///< ADC模拟看门狗 1 监控 DAT13 的数据
#define ADC_DAT_14 14 ///< ADC模拟看门狗 1 监控 DAT14 的数据
#define ADC_DAT_15 15 ///< ADC模拟看门狗 1 监控 DAT15 的数据

#define ADC_CHN_0 0   ///< ADC通道0
#define ADC_CHN_1 1   ///< ADC通道1
#define ADC_CHN_2 2   ///< ADC通道2
#define ADC_CHN_3 3   ///< ADC通道3
#define ADC_CHN_4 4   ///< ADC通道4
#define ADC_CHN_5 5   ///< ADC通道5
#define ADC_CHN_6 6   ///< ADC通道6
#define ADC_CHN_7 7   ///< ADC通道7
#define ADC_CHN_8 8   ///< ADC通道8
#define ADC_CHN_9 9   ///< ADC通道9
#define ADC_CHN_10 10 ///< ADC通道10
#define ADC_CHN_11 11 ///< ADC通道11
#define ADC_CHN_12 12 ///< ADC通道12
#define ADC_CHN_13 13 ///< ADC通道13
#define ADC_CHN_14 14 ///< ADC通道14
#define ADC_CHN_15 15 ///< ADC通道15

void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *this);
void ADC_StructInit(ADC_InitTypeDef *this);

s16 ADC_GetData(ADC_TypeDef *ADCx, u8 ADC_DAT_x);                   ///< 读ADC采样值
void ADC_SetPChanne(ADC_TypeDef *ADCx, u8 ADC_DAT_x, u8 ADC_CHN_x); ///< 设置ADCx通道x正端信号来源
void ADC_SetNChanne(ADC_TypeDef *ADCx, u8 ADC_DAT_x, u8 ADC_CHN_x); ///< 设置ADCx通道x负端信号来源
void ADC_Trigger(ADC_TypeDef *ADCx);                                ///< 软件触发ADC采样
void ADC_ClearIRQFlag(ADC_TypeDef *ADCx, u16 ADC_IF_x);             ///< ADC清除指定中断
u8 ADC_GetFlag(ADC_TypeDef *ADCx, u16 ADC_IF_x);
u8 ADC_GetIRQFlag(ADC_TypeDef *ADCx, u16 ADC_IF_x); ///< 判断ADC中断状态
u8 ADC_GetState(ADC_TypeDef *ADCx);
float ADC_GetVoltage(ADC_TypeDef *ADCx, u8 datax);
float ADC_GetRange(ADC_TypeDef *ADCx, u8 datax);
void ADC_Enable(ADC_TypeDef *ADCx);
void ADC_Disable(ADC_TypeDef *ADCx);
void ADC_AnalogDisable(void);
void ADC_AnalogEnable(void);
#endif /*_LKS32MC45X_ADC_H_ */
