/**
 * @file
 * @defgroup nvr
 * @brief        NVR库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x.h"
#include "lks32mc45x_sys.h"

void NVR_EraseSector(u32 adr);
void NVR_Program(u32 adr, u32 *buf, u32 sz);
u32 NVR_Read(u32 adr);
/**
 * @brief    Flash按扇区擦除操作
 * @param adr：       扇区地址  (一个扇区512字节)
                    erase_flag：调用函数前必须赋值0x9A0D361F，否则不执行擦除，用来防程序跑飞
 * @par 更新记录  V1.0  2022/08/03  YangZJ  创建
*/
void NVR_EraseSector(u32 adr)
{

    u32 reg;
    reg = __get_PRIMASK();
    __disable_irq();
    SYS_PROTECT = 0x7a83;           //  使能系统寄存器写操作
    SYS->FLSE = 0x8FCA;             //  系统寄存器SYS_FLSE，写入密钥，解除擦除保护
    FSMC_ICFG = BIT31 | BIT11;      //  On-Chip FLASH擦除操作使能
    FSMC_ADDR = adr;                //  On-Chip FLASH地址寄存器FSMC_ADDR，写入擦除地址
    FSMC_ERAS = 0x7654dcba;         //  On-Chip FLASH擦除寄存器FSMC_ERAS，触发擦除操作
    while ((FSMC_REDY & BIT8) == 0) //  寄存器FSMC_REDY.IREADY位，指示当前S总线是否完成了当前擦除访问操作，1表示完成，0表示未完成。
    {
        __NOP();
    }
    FSMC_ICFG &= ~BIT31; //  On-Chip FLASH擦除操作使能
    SYS->FLSE = 0;       // 解除保护
    SYS_PROTECT = 0;     //  使能系统寄存器写操作
    __set_PRIMASK(reg);
}

/**
 * @brief    Flash编程操作
 * @param adr：扇区地址
               sz : 编程字节数量(0~512字节)
               buf: 要编程的数据数组指针；
               erase_flag：调用函数前必须赋值0x9AFDA40C，否则不执行编程，用来防程序跑飞
 * @return 1：编程成功；  0: 编程失败
 * @par 更新记录  V1.0  2022/08/03  YangZJ  创建
*/
void NVR_Program(u32 adr, u32 *buf, u32 sz)
{
    u32 i;
    u32 reg;
    reg = __get_PRIMASK();
    __disable_irq();
    adr &= (u32)~0XF;
    __disable_irq();
    SYS_PROTECT = 0x7a83;                //  使能系统寄存器写操作
    SYS->FLSP = 0x8f35;                  //  系统寄存器SYS_FLSE，写入密钥，解除擦除保护
    FSMC_ICFG = (BIT27 | BIT23 | BIT11); //  On-Chip FLASH擦除操作使能
    FSMC_ADDR = adr;                     //  On-Chip FLASH地址寄存器FSMC_ADDR，写入擦除地址
    sz >>= 2;
    for (i = 0; i < sz; i += 4)
    {
        FSMC_WDAT0 = buf[i];            //  On-Chip FLASH编程数据寄存器FSMC_WDAT0至FSMC_WDAT3，写入编程数据
        FSMC_WDAT1 = buf[i + 1];        //  On-Chip FLASH编程数据寄存器FSMC_WDAT0至FSMC_WDAT3，写入编程数据
        FSMC_WDAT2 = buf[i + 2];        //  On-Chip FLASH编程数据寄存器FSMC_WDAT0至FSMC_WDAT3，写入编程数据
        FSMC_WDAT3 = buf[i + 3];        //  On-Chip FLASH编程数据寄存器FSMC_WDAT0至FSMC_WDAT3，写入编程数据
        FSMC_WDAT = 0x7654dcba;         //  On-Chip FLASH编程触发寄存器FSMC_WDAT，写入触发值，触发编程
        while ((FSMC_REDY & BIT8) == 0) //  寄存器FSMC_REDY.IREADY位，指示当前S总线是否完成了当前操作，1表示完成，0表示未完成。
        {
            __NOP();
        }
        FSMC_WDAT = 0;
    }
    FSMC_ICFG = 0;   //  On-Chip FLASH擦除操作使能
    FSMC_ADDR = 0;   //  On-Chip FLASH地址寄存器FSMC_ADDR，写入擦除地址
    SYS->FLSP = 0;   //  系统寄存器SYS_FLSE，写入密钥，解除擦除保护
    SYS_PROTECT = 0; //  使能系统寄存器写操作
    __set_PRIMASK(reg);
}

/**
 * @brief    读取FLASH数据函数
 * @param adr：要读取数据的地址
 输出参数：    读取的数据值
 * @return data
 * @par 更新记录  V1.0  2022/08/03  YangZJ  创建
*/
u32 NVR_Read(u32 adr)
{
    u32 data;

    adr &= (u32)~0x3; // 32-bit, word-alignment
    {
        FSMC_ICFG |= 0x00000800; // set nvr bit
        FSMC_ADDR = adr;         // access address of nvr
        data = FSMC_RDAT;
        FSMC_ICFG &= (u32)~0x00000800; // reset nvr bit
        return data;
    }
}

/*! @} */
