/**
 * @file
 * @brief        Flash库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef __lks32mc45x_FLASH__
#define __lks32mc45x_FLASH__

#include "lks32mc45x.h"
#include "basic.h"

void FLASH_EraseSector(u32 adr);
u32 FLASH_Program(u32 adr, u32 *buf, u32 sz);
u32 FLASH_Read(u32 adr);

#endif
