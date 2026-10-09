/**
 * @file
 * @brief        校准参数库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef _LKS32MC45X_TRIM_
#define _LKS32MC45X_TRIM_

#include "lks32mc45x.h"

#define TRIM_VAL_SYS_AFE_REGC 0x00000800  ///< SYS_AFE_REGC      校准值
#define TRIM_VAL_SYS_AFE_REGD 0x00000804  ///< SYS_AFE_REGD      校准值
#define TRIM_VAL_SYS_AFE_REGE 0x00000808  ///< SYS_AFE_REGE      校准值
#define TRIM_VAL_SYS_AFE_REGF 0x0000080C  ///< SYS_AFE_REGF      校准值
#define TRIM_VAL_SYS_AFE_REG10 0x00000810 ///< SYS_AFE_REG10     校准值

#define TRIM_VAL_DAC0_AMC_30 0x00000860 ///< SYS_AFE_DAC_AMC   校准值（扩大 512 倍结果）  DAC0选择3.0V 档位
#define TRIM_VAL_DAC0_AMC_12 0x00000864 ///< SYS_AFE_DAC_AMC   校准值（扩大 512 倍结果）  DAC0选择1.2V 档位
#define TRIM_VAL_DAC0_DC_30 0x00000868  ///< SYS_AFE_DAC_DC    校准值                     DAC0选择3.0V 档位
#define TRIM_VAL_DAC0_DC_12 0x0000086C  ///< SYS_AFE_DAC_DC    校准值                     DAC0选择1.2V 档位
#define TRIM_VAL_DAC1_AMC_30 0x00000870 ///< SYS_AFE_DAC_AMC   校准值（扩大 512 倍结果）  DAC1选择3.0V 档位
#define TRIM_VAL_DAC1_AMC_12 0x00000874 ///< SYS_AFE_DAC_AMC   校准值（扩大 512 倍结果）  DAC1选择1.2V 档位
#define TRIM_VAL_DAC1_DC_30 0x00000878  ///< SYS_AFE_DAC_DC    校准值                    DAC1选择3.0V 档位
#define TRIM_VAL_DAC1_DC_12 0x0000087C  ///< SYS_AFE_DAC_DC    校准值                    DAC1选择1.2V 档位

#define TRIM_VAL_ADC0_DC0 0x00000820  ///< ADC0_DC0          校准值     2/3倍增益
#define TRIM_VAL_ADC0_AMC0 0x00000828 ///< ADC0_AMC0         校准值     2/3倍增益
#define TRIM_VAL_ADC1_DC0 0x00000830  ///< ADC1_DC0          校准值     2/3倍增益
#define TRIM_VAL_ADC1_AMC0 0x00000838 ///< ADC1_AMC0         校准值     2/3倍增益
#define TRIM_VAL_ADC2_DC0 0x00000840  ///< ADC2_DC0          校准值     2/3倍增益
#define TRIM_VAL_ADC2_AMC0 0x00000848 ///< ADC2_AMC0         校准值     2/3倍增益

#define TRIM_VAL_ADC0_DC1 0x00000824  ///< ADC0_DC1          校准值     1倍增益
#define TRIM_VAL_ADC0_AMC1 0x0000082C ///< ADC0_AMC1         校准值     1倍增益
#define TRIM_VAL_ADC1_DC1 0x00000834  ///< ADC1_DC1          校准值     1倍增益
#define TRIM_VAL_ADC1_AMC1 0x0000083C ///< ADC1_AMC1         校准值     1倍增益
#define TRIM_VAL_ADC2_DC1 0x00000844  ///< ADC2_DC1          校准值     1倍增益
#define TRIM_VAL_ADC2_AMC1 0x0000084C ///< ADC2_AMC1         校准值     1倍增益

#define TRIM_VAL_TMP_A 0x00000940 ///< SYS_TMP_A         校准值 温度传感器，斜率校准值
#define TRIM_VAL_TMP_B 0x00000944 ///< SYS_TMP_B         校准值 温度传感器，偏置校准值

u32 TRIM_Read(u32 TRIM_VAL_x); 
void TRIM_ReadUid(u32 *);          //  定义一个数组u32 lks_id[7];  调用该函数 TRIM_ReadUid(lks_id);
 
// 兼容之前的写法
#define Read_Trim TRIM_Read

#endif /*_LKS32MC45X_TRIM_ */
