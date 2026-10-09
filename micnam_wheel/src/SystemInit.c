#include "lks32mc45x_lib.h"
void SystemInit(void);
void SystemInit(void)
{
    SYS_PROTECT = 0x7A83;
    SYS_SFT_RST = 0xffffffff; // 软复位
    SYS_SFT_RST = 0;          // 软复位释放

    __asm("NOP"); // 软复位释放后，延时最少一个总线周期
    __asm("NOP");
    __asm("NOP");
    __asm("NOP");
    __asm("NOP");
    __asm("NOP");

    SYS_MclkChoice(SYS_MCLK_192M_RC);
    IWDG_Disable();
    WWDG_Disable();
    SCB->CPACR = 0xf << 20; // 使能FPU
}
