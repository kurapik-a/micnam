/**
 * @file
 * @brief        后背存储器库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __LKS32MC45X_AON_H
#define __LKS32MC45X_AON_H

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"
typedef struct
{
    __IO u32 BRAM[32];    ///< 0x00~0x7F 后备存储器
    __IO u32 PWR_CFG;     ///< 0x80     功耗管理配置寄存器
    __IO u32 EVT_RCD;     ///< 0x84     事件记录寄存器
    __IO u32 IO_WAKE_POL; ///< 0x88     IO唤醒极性寄存器
    __IO u32 IO_WAKE_EN;  ///< 0x8C     IO唤醒使能寄存器
} AON_TypeDef;
#ifndef AON
#define AON ((AON_TypeDef *)AON_BASE)
#endif

#define AON_RCD_STANDBY BIT14     ///< [14] 停机(掉电)模式记录，高表示发生过
#define AON_RCD_DEEPSLEEP BIT13   ///< [13] 深度休眠记录，高表示发生过
#define AON_RCD_SLEEP BIT12       ///< [12] 休眠记录，高表示发生过
#define AON_RCD_IWDG_WK BIT9      ///< [9]  IWDG定时唤醒记录，高表示休眠被IWDG定时唤醒，包括Deep Sleep以及Standby模式的唤醒
#define AON_RCD_IO_WK BIT8        ///< [8]  IO唤醒记录，高表示休眠被IO唤醒，包括Deep Sleep以及Standby模式的唤醒
#define AON_RCD_CPOR_RST BIT5     ///< [5]  CPU掉电后上电复位发生记录，高表示发生过
#define AON_RCD_WWDG_RST BIT4     ///< [4]  窗口看门狗复位发生记录，高表示发生过
#define AON_RCD_IWDG_RST BIT3     ///< [3]  独立看门狗复位发生记录，高表示发生过
#define AON_RCD_KEY_RST_RCD BIT2  ///< [2]  按键复位发生记录，高表示发生过
#define AON_RCD_HPOR_RST_RCD BIT1 ///< [1]  HPOR复位发生记录，高表示发生过
#define AON_RCD_LPOR_RST_RCD BIT0 ///< [0]  LPOR复位发生记录，高表示发生过

#define AON_WK_P3_2 BIT7  ///< [7] P3[2]外部唤醒触发使能
#define AON_WK_P2_14 BIT6 ///< [6] P2[14]外部唤醒触发使能
#define AON_WK_P1_11 BIT5 ///< [5] P1[11]外部唤醒触发使能
#define AON_WK_P0_13 BIT4 ///< [4] P0[13]外部唤醒触发使能
#define AON_WK_P0_5 BIT3  ///< [3] P0[5]外部唤醒触发使能
#define AON_WK_P0_4 BIT2  ///< [2] P0[4]外部唤醒触发使能
#define AON_WK_P0_3 BIT1  ///< [1] P0[3]外部唤醒触发使能
#define AON_WK_P0_2 BIT0  ///< [0] P0[2]外部唤醒触发使能

void AON_SaveData(u8, u32 *);
void AON_RadeData(u8, u32 *);
u8 AON_ReadEVTRCD(u16);
void AON_ClearEVTRCD(void);
void AON_SetGpioWakePolarity(u16 WK_Pin, u16 Polarity);
#endif /*__lks32mc03x_IWDG_H */
