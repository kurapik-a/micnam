/**
 * @file
 * @defgroup adc
 * @brief        ADC库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */

#include "lks32mc45x_adc.h"
#include "lks32mc45x_sys.h"
#include "lks32mc45x_afe.h"
#include "lks32mc45x.h"
#include "string.h"
#include "lks32mc45x_nvr.h"
#include "lks32mc45x_lib.h"

/**
 * @brief    ADC初始化
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *this)
{
    u16 t_reg;

    if (ADCx == ADC0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC0, ENABLE);
    }
    else if (ADCx == ADC1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC1, ENABLE);
    }
    else if (ADCx == ADC2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC2, ENABLE);
    }
    AFE_ModuleClockCmd(AFE_MODULE_ADC, ENABLE);

    ADCx->IE = this->IE | this->RE;

    t_reg = (u16)(this->OVSR) | (u16)(this->TROVS << 3) | (u16)(this->TCNT << 4) | (u16)(this->CSMP << 8) | (u16)(this->Align << 10) | (u16)(this->NSMP << 12);
    ADCx->CFG = t_reg;
    ADCx->GAIN = this->GAIN;

    ADCx->CHNT = this->S1 | (u16)(this->S2 << 4);

    ADCx->TH0 = this->LTH0 | (u16)(this->HTH0 << 16);
    ADCx->GEN0 = this->GEN0;
    ADCx->TH1 = this->LTH1 | (u16)(this->HTH1 << 16);
    ADCx->GEN1 = this->GEN1;
		
	if (this->Align == ADC_ALIGN_LEFT)
	{
		if(ADCx ==ADC0)
		{
			ADCx->DC0  = TRIM_Read(TRIM_VAL_ADC0_DC0)  ;
			ADCx->DC1  = TRIM_Read(TRIM_VAL_ADC0_DC1)  ;
			ADCx->AMC0 = TRIM_Read(TRIM_VAL_ADC0_AMC0) ;
			ADCx->AMC1 = TRIM_Read(TRIM_VAL_ADC0_AMC1) ;
		}              
		if(ADCx ==ADC1)
		{              
			ADCx->DC0  = TRIM_Read(TRIM_VAL_ADC1_DC0)  ;
			ADCx->DC1  = TRIM_Read(TRIM_VAL_ADC1_DC1)  ;
			ADCx->AMC0 = TRIM_Read(TRIM_VAL_ADC1_AMC0) ;
			ADCx->AMC1 = TRIM_Read(TRIM_VAL_ADC1_AMC1) ;
		}
		if(ADCx ==ADC2)
		{
			ADCx->DC0  = TRIM_Read(TRIM_VAL_ADC2_DC0)  ;
			ADCx->DC1  = TRIM_Read(TRIM_VAL_ADC2_DC1)  ;
			ADCx->AMC0 = TRIM_Read(TRIM_VAL_ADC2_AMC0) ;
			ADCx->AMC1 = TRIM_Read(TRIM_VAL_ADC2_AMC1) ;
		}
	}                
	else             
	{ 
		if(ADCx ==ADC0)
		{
			ADCx->DC0  = TRIM_Read(TRIM_VAL_ADC0_DC0)  ;
			ADCx->DC1  = TRIM_Read(TRIM_VAL_ADC0_DC1)  ;
			ADCx->AMC0 = TRIM_Read(TRIM_VAL_ADC0_AMC0) ;
			ADCx->AMC1 = TRIM_Read(TRIM_VAL_ADC0_AMC1) ;
		}              
		if(ADCx ==ADC1)
		{              
			ADCx->DC0  = TRIM_Read(TRIM_VAL_ADC1_DC0)  ;
			ADCx->DC1  = TRIM_Read(TRIM_VAL_ADC1_DC1)  ;
			ADCx->AMC0 = TRIM_Read(TRIM_VAL_ADC1_AMC0) ;
			ADCx->AMC1 = TRIM_Read(TRIM_VAL_ADC1_AMC1) ;
		}
		if(ADCx ==ADC2)
		{
			ADCx->DC0  = TRIM_Read(TRIM_VAL_ADC2_DC0)  ;
			ADCx->DC1  = TRIM_Read(TRIM_VAL_ADC2_DC1)  ;
			ADCx->AMC0 = TRIM_Read(TRIM_VAL_ADC2_AMC0) ;
			ADCx->AMC1 = TRIM_Read(TRIM_VAL_ADC2_AMC1) ;
		}
	}

    ADCx->TRIG = (this->TRIG);	
}
/**
 * @brief    ADC配置结构体初始化
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void ADC_StructInit(ADC_InitTypeDef *this)
{
    memset(this, 0, sizeof(ADC_InitTypeDef));
}

/**
 * @brief    读ADC采样值
 * @return ADC采样值
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
s16 ADC_GetData(ADC_TypeDef *ADCx, u8 datax)
{
    u32 *p;
    p = (u32 *)ADCx;
    return (s16)p[datax];
}
/**
 * @brief    获取ADC量程，当使用外部
 * @return 电压
 * @note 当使用外部电源作为ADC_REF的时候，默认供电为3.3V
 * @par 更新记录  V1.0  2022/07/28  YangZJ  创建
 * @par 更新记录  V1.1  2022/11/21  YangZJ  ADC使用内部基准的时候，量程由3.6改为3.3
 */
float ADC_GetRange(ADC_TypeDef *ADCx, u8 datax)
{
    float range = 0;
    if (SYS_AFE_REG2 & BIT12)
    {
        range = (ADCx->GAIN & (1 << datax)) ? (float)3.3 : (float)4.95;
    }
    else
    {
        range = (ADCx->GAIN & (1 << datax)) ? (float)2.4 : (float)3.3;
    }
    return range;
}
/**
 * @brief    读ADC采样电压
 * @return 电压
 * @par 更新记录  V1.0  2022/07/28  YangZJ  创建
 */
float ADC_GetVoltage(ADC_TypeDef *ADCx, u8 datax)
{
    s16 adc_val;
    float adcx_range;
    adc_val = ADC_GetData(ADCx, datax);
    adcx_range = ADC_GetRange(ADCx, datax);
    if (ADCx->CFG & BIT10)
    {
        return adc_val * adcx_range / 0x1fff;
    }
    else
    {
        return adc_val * adcx_range / 0x7ffe;
    }
}
/**
 * @brief    设置ADCx通道x正端信号来源
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 * @par 更新记录  V1.1  2022/11/21  YangZJ  修复BUG，ADC通道配置错误的问题
 */
void ADC_SetPChanne(ADC_TypeDef *ADCx, u8 datax, u8 chnx)
{
    volatile u32 *p;
    u32 reg;
    p = &ADCx->PCHN0;
    reg = p[datax >> 2];
    reg &= (u32)(~(0xf << ((datax & 3) * 4)));
    reg |= (u32)(chnx << (u8)((datax & 3) * 4));
    p[datax >> 2] = reg;
    if (ADCx == ADC2)
    {
        if (chnx == 14)
        {
            ADCx->GAIN |= (1 << datax);
        }
    }
}
/**
 * @brief    设置ADCx通道x负端信号来源
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void ADC_SetNChanne(ADC_TypeDef *ADCx, u8 datax, u8 chnx)
{
    volatile u32 *p;
    u32 reg;
    p = &ADCx->NCHN0;
    reg = p[datax >> 2];
    reg &= (u32)(~(0xf << ((datax & 3) * 4)));
    reg |= (u32)(chnx << ((datax & 3) * 4));
    p[datax >> 2] = reg;
}
/**
 * @brief    软件触发ADC采样
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void ADC_Trigger(ADC_TypeDef *ADCx)
{
    ADCx->SWT = 0x5aa5;
}
/**
 * @brief    ADC清除中断
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void ADC_ClearIRQFlag(ADC_TypeDef *ADCx, u16 ifx)
{
    ADCx->IF = ifx;
}
/**
 * @brief    判断ADC中断状态
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
u8 ADC_GetIRQFlag(ADC_TypeDef *ADCx, u16 ADC_IF_x)
{
    if (ADCx->IF & ADCx->IE & ADC_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    判断ADC中断状态
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
u8 ADC_GetFlag(ADC_TypeDef *ADCx, u16 ADC_IF_x)
{
    if (ADCx->IF & ADC_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    获取ADC工作状态
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
u8 ADC_GetState(ADC_TypeDef *ADCx)
{
    if (ADCx->CFG & BIT11)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/05/09  YangZJ  创建
 */
void ADC_Enable(ADC_TypeDef *ADCx)
{
    if (ADCx == ADC0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC0, ENABLE);
    }
    else if (ADCx == ADC1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC1, ENABLE);
    }
    else if (ADCx == ADC2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC2, ENABLE);
    }
}
/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/08/13  YangZJ  创建
 */
void ADC_Disable(ADC_TypeDef *ADCx)
{
    if (ADCx == ADC0)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC0, DISABLE);
    }
    else if (ADCx == ADC1)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC1, DISABLE);
    }
    else if (ADCx == ADC2)
    {
        SYS_ModuleClockCmd(SYS_MODULE_ADC2, DISABLE);
    }
}

/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/08/13  YangZJ  创建
 */
void ADC_AnalogDisable(void)
{
    AFE_ModuleClockCmd(AFE_MODULE_ADC, DISABLE);
}
/**
 * @brief    ADC使能
 * @par 更新记录  V1.0  2022/08/13  YangZJ  创建
 */
void ADC_AnalogEnable(void)
{
    AFE_ModuleClockCmd(AFE_MODULE_ADC, ENABLE);
}

/*! @} */
