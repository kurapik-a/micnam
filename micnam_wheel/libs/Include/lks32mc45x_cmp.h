/**
 * @file
 * @brief        比较器库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __LKS32MC45x_CMP_H
#define __LKS32MC45x_CMP_H
///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO u32 IE;      ///< 比较器中断使能寄存器
    __IO u32 IF;      ///< 比较器中断标志寄存器
    __IO u32 TCLK;    ///< 比较器分频时钟控制寄存器
    __IO u32 CFG;     ///< 比较器控制寄存器
    __IO u32 BLCWIN0; ///< 比较器开窗控制寄存器
    __IO u32 BLCWIN1; ///< 比较器开窗控制寄存器
    __IO u32 DATA;    ///< 比较器输出数值寄存器
} CMP_TypeDef;

#ifndef CMP
#define CMP ((CMP_TypeDef *)CMP_BASE)
#endif

#define CMP_CHN_0 0
#define CMP_CHN_1 1
#define CMP_CHN_2 2
#define CMP_CHN_3 3
#define CMP_CHN_4 4
#define CMP_CHN_5 5

// 比较器回差选择
#define CMP_HYS_20mV 0
#define CMP_HYS_0mV 1

// 比较器信号正端选择
#define CMP_SELP_IP0 0
#define CMP_SELP_OPA_IP 1
#define CMP_SELP_OPAX_OUT 2
#define CMP_SELP_OPAX_ADD1_OUT 3 ///< 比较器0对应运放1，比较器1对应运放2，以此类推，比较器5对应运放0
#define CMP_SELP_IP1 4
#define CMP_SELP_IP2 5
#define CMP_SELP_IP3 6
#define CMP_SELP_AVSS 7

// 比较器信号负端选择
#define CMP_SELN_IN 0
#define CMP_SELN_REF 1
#define CMP_SELN_DAC 2
#define CMP_SELN_HALL_MID 3

typedef struct
{
    ///< 比较器0
    u16 SELP;             ///< 比较器 0 信号正端选择
    u16 SELN;             ///< 比较器 0 信号负端选择
    FuncState EN;         ///< 比较器 0 使能
    FuncState IE;         ///< 比较器 0 中断使能
    FuncState RE;         ///< 比较器 0 DMA 请求使能
    FuncState POL;        ///< 比较器 0 极性选择
    FuncState IRQ_TRIG;   ///< 比较器 0 边沿触发使能
    FuncState IN_EN;      ///< 比较器 0 信号输入使能
    FuncState W_PWM_POL;  ///< 比较器 0 开窗 PWM 信号极性选择
    FuncState CHN3P_PWM0; ///< MCPWM0 模块 CHN3_P 通道使能比较器 0 开窗
    FuncState CHN2P_PWM0; ///< MCPWM0 模块 CHN2_P 通道使能比较器 0 开窗
    FuncState CHN1P_PWM0; ///< MCPWM0 模块 CHN1_P 通道使能比较器 0 开窗
    FuncState CHN0P_PWM0; ///< MCPWM0 模块 CHN0_P 通道使能比较器 0 开窗
    FuncState CHN3P_PWM1; ///< MCPWM1 模块 CHN3_P 通道使能比较器 0 开窗
    FuncState CHN2P_PWM1; ///< MCPWM1 模块 CHN2_P 通道使能比较器 0 开窗
    FuncState CHN1P_PWM1; ///< MCPWM1 模块 CHN1_P 通道使能比较器 0 开窗
    FuncState CHN0P_PWM1; ///< MCPWM1 模块 CHN0_P 通道使能比较器 0 开窗
    u8 null;              ///< 地址对齐
} CMP_SingleTypeDef;
typedef struct
{
    ///< 比较器IO配置
    FuncState FT;           ///< 比较器快速比较使能
    u8 HYS;                 ///< 比较器回差选择
    FuncState IT_CMP;       ///< 比较速度选择 0:比较速度 150ns    1:比较速度 600ns
    u8 null;                ///< 地址对齐
    u32 CLK01_DIV;          ///< 比较器01滤波系数，1-2048
    u32 CLK23_DIV;          ///< 比较器01滤波系数，1-2048
    u32 CLK45_DIV;          ///< 比较器01滤波系数，1-2048
    CMP_SingleTypeDef cmp0; ///< CMP0配置
    CMP_SingleTypeDef cmp1; ///< CMP1配置
    CMP_SingleTypeDef cmp2; ///< CMP2配置
    CMP_SingleTypeDef cmp3; ///< CMP3配置
    CMP_SingleTypeDef cmp4; ///< CMP4配置
    CMP_SingleTypeDef cmp5; ///< CMP5配置
} CMP_InitTypeDef;

void CMP_Init(CMP_InitTypeDef *);       ///< 比较器初始化
void CMP_StructInit(CMP_InitTypeDef *); ///< 比较器配置结构体初始化
void CMP_ClearIRQFlag(u8 CMP_CHN_x);    ///< 清CMP中断
u8 CMP_GetIRQFlag(u8 CMP_CHN_x);        ///< 读CMP中断

u8 CMP_GetFlag(u8 CMP_CHN_x);
void CMP_Enable(void);
void CMP_Disable(void);
void CMP_AnalogDisable(u8 CMP_CHN_x);
void CMP_AnalogEnable(u8 CMP_CHN_x);
#endif /*__lks32mc45x_cmp_H */
