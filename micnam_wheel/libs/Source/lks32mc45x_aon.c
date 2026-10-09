/**
 * @file
 * @defgroup aon
 * @brief        后背存储器库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_aon.h"
#include "lks32mc45x.h"

/**
 * @brief    向后背存储器写入数据数据
 * @param len : 需要写入的数据长度，单位字，最大32
 * @param p   : 需要写入数据的地址
 * @par 更新记录  V1.0  2022/05/12  YangZJ  创建
 */
void AON_SaveData(u8 len, u32 *p)
{
    u32 i;
    len = (len > 32) ? 32 : len;
    for (i = 0; i < len; i++)
    {
        AON->BRAM[i] = p[i];
    }
    __NOP();
}
/**
 * @brief    读取后备存储器数据
 * @param len : 需要读取的数据长度，单位字，最大32
 * @param p   : 需要读取数据的地址
 * @par 更新记录  V1.0  2022/05/12  YangZJ  创建
 */
void AON_RadeData(u8 len, u32 *p)
{
    u32 i;
    len = (len > 32) ? 32 : len;
    for (i = 0; i < len; i++)
    {
        p[i] = AON->BRAM[i];
    }
    __NOP();
}
/**
 * @brief    读取掉电记录
 * @param PMU_RCD_x 需要读取的复位记录
 * @par 更新记录  V1.0  2022/05/12  YangZJ  创建
 */
u8 AON_ReadEVTRCD(u16 PMU_RCD_x)
{
    if (AON->EVT_RCD & PMU_RCD_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    读取掉电记录
 * @param PMU_RCD_x 需要读取的复位记录
 * @par 更新记录  V1.0  2022/05/12  YangZJ  创建
 */
void AON_ClearEVTRCD(void)
{
    u32 i = 0;
    AON->EVT_RCD = 0xca40;
    while (AON->EVT_RCD != 0)
    {
        i++;
        if (i > 0x1ffff)
        {
            return;
        }
    }
}
/**
 * @brief    设置IO唤醒极性以及唤醒IO
 * @param WK_Pin    唤醒IO
 * @param Polarity  唤醒极性
 * @par 更新记录  V1.0  2022/05/12  YangZJ  创建
 */
void AON_SetGpioWakePolarity(u16 AON_Wake_Pinx, u16 AON_Polarity_x)
{
    AON->IO_WAKE_EN = (AON_Polarity_x != 0);
    AON->IO_WAKE_POL = AON_Wake_Pinx;
}

/*! @} */
