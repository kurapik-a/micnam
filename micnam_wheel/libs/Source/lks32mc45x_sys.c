/**
 * @file
 * @defgroup sys
 * @brief        SYS库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_afe.h"
#include "lks32mc45x_sys.h"
#include "lks32mc45x.h"
static u32 sys_clk = 192000000;

/**
 * @brief    SYS清除复位标志
 * @note 由于复位记录工作于低速时钟域，清除执行完成需要一定时间，不应清除后立即读记录状态
 * @par 更新记录  V1.0  2016/5/24  YangZJ  创建
 */
void SYS_ClearRst(void)
{
    SYS->PROTECT = 0x7a83;
    SYS->CLK_CFG |= (BIT12 | BIT13);
    SYS->PROTECT = 0;
}
/**
 * @brief    模块时钟使能和停止
 * @param u32 nMODULE：对应的模块
 * @par 更新记录  V1.0  2016/6/28  YangZJ  创建
 */
void SYS_ModuleClockCmd(u32 nMODULE, u8 state)
{
    u32 val;
    SYS->PROTECT = 0x7a83;
    val = SYS->CLK_FEN;
    if (state != DISABLE)
    {
        SYS->CLK_FEN = val | nMODULE;
    }
    else
    {
        SYS->CLK_FEN &= ~nMODULE;
    }
    SYS->PROTECT = 0;
}

/**
 * @brief    模块软复位
 * @param u32 nMODULE：对应的模块
 * @par 更新记录  V1.0  2016/6/28  YangZJ  创建
 */
void SYS_SoftResetMODULE(u32 nMODULE)
{
    SYS->PROTECT = 0x7a83;
    SYS->SFT_RST = nMODULE;
    SYS->SFT_RST = 0;
    SYS->PROTECT = 0;
}

/**
 * @brief    系统主时钟配置
 * @param SYS_MCLK_x 系统主时钟频率
 * @par 更新记录  V1.0  20220512  YangZJ  创建
 */
void SYS_MclkChoice(u8 SYS_MCLK_x)
{
    AFE_ClkInitTypeDef AFE_ClkInitStruct;
    u32 reg = 0;

    switch (SYS_MCLK_x)
    {
    case SYS_MCLK_192M_RC:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 0;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 0;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0xff);
        sys_clk = 192000000;
        break;
    }
    case SYS_MCLK_96M_RC:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 0;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 0;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x55);
        sys_clk = 96000000;
        break;
    }
    case SYS_MCLK_48M_RC:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 0;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 0;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x11);
        sys_clk = 48000000;
        break;
    }
    case SYS_MCLK_24M_RC:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 0;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 0;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x01);
        sys_clk = 24000000;
        break;
    }
    case SYS_MCLK_12M_RC:
    {
        AFE_ClkInitStruct.PLLPDN = DISABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;         //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 0;    //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 0;      //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;       //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);    //   模拟部分配置
        reg = (0 << 8) | (0xff);
        sys_clk = 12000000;
        break;
    }
    case SYS_MCLK_32K_RC:
    {
        AFE_ClkInitStruct.PLLPDN = DISABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;         //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 0;    //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 0;      //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;       //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);    //   模拟部分配置
        reg = (2 << 8) | (0xff);
        sys_clk = 32000;
        break;
    }
    case SYS_MCLK_192M_XTAL12:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0xff);
        sys_clk = 192000000;
        break;
    }
    case SYS_MCLK_96M_XTAL12:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x55);
        sys_clk = 96000000;
        break;
    }
    case SYS_MCLK_48M_XTAL12:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x11);
        sys_clk = 48000000;
        break;
    }
    case SYS_MCLK_24M_XTAL12:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x01);
        sys_clk = 24000000;
        break;
    }
    case SYS_MCLK_12M_XTAL12:
    {
        AFE_ClkInitStruct.PLLPDN = DISABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 0;         //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;    //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;      //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 0;       //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);    //   模拟部分配置
        reg = (3 << 8) | (0xff);
        sys_clk = 12000000;
        break;
    }
    case SYS_MCLK_192M_XTAL24:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 1;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 1;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0xff);
        sys_clk = 192000000;
        break;
    }
    case SYS_MCLK_96M_XTAL24:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 1;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 1;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x55);
        sys_clk = 96000000;
        break;
    }
    case SYS_MCLK_48M_XTAL24:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 1;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 1;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x11);
        sys_clk = 48000000;
        break;
    }
    case SYS_MCLK_24M_XTAL24:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 1;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 1;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (1 << 8) | (0x01);
        sys_clk = 24000000;
        break;
    }
    case SYS_MCLK_12M_XTAL24:
    {
        AFE_ClkInitStruct.PLLPDN = ENABLE; //   PLL使能0: 关闭1: 使能
        AFE_ClkInitStruct.XDIV = 1;        //   晶体频率除以2   0:使用 12MHz 外部晶体   1:使用 24MHz 外部晶体，同时需要将 AFE_REG4.XTRSEL[0]置为 1
        AFE_ClkInitStruct.PLLSR_SEL = 1;   //   PLL参考时钟选择0: PLL使用RCH时钟1: PLL使用晶体时钟该信号在不使用晶体时钟的产品或应用下，时钟输出0;在使用晶体时钟且晶体未停振时输出1，如晶体停振则输出0
        AFE_ClkInitStruct.XTALPDN = 1;     //   晶体起振电路使能0: 关闭1: 使能
        AFE_ClkInitStruct.XTRSEL = 1;      //   晶体起振电路电阻调节 XTRSEL<1>=1: N端阻值增加XTRSEL<0>=1: P端电阻减小一倍
        AFE_ClkInit(&AFE_ClkInitStruct);   //   模拟部分配置
        reg = (3 << 8) | (0xff);
        sys_clk = 12000000;
        break;
    }
    }
    SYS->PROTECT = 0x7a83;
    SYS_AFE_REG3 &= ~BIT12;
    SYS_AFE_REG3 |= BIT15;
    SYS->CLK_CFG = reg;
    SYS->PROTECT = 0;
}
/**
 * @brief    读取当前时钟
 * @return sys_clk 系统时钟频率
 * @par 更新记录  V1.0  20220512  YangZJ  创建
 */
u32 SYS_ReadMcuClk(void)
{
    return sys_clk;
}
/**
 * @brief    系统主时钟配置
 * @param SYS_MCLK_x 系统主时钟频率
 * @par 更新记录  V1.0  20220512  YangZJ  创建
 */
void SYS_UartClkDiv(u16 div)
{
    SYS->PROTECT = 0x7a83;
    SYS->CLK_DIV2 = div;
    SYS->PROTECT = 0;
}

void SYS_StructInit(SYS_InitTypeDef *SYS_InitStruct)
{
    SYS_InitStruct->PLL_SrcSel = SYS_PLLSRSEL_RCH;
    SYS_InitStruct->PLL_DivSel = 0xFF;
    SYS_InitStruct->Clk_Sel = CLK_SEL_PLL;

    SYS_InitStruct->WDT_Ena = DISABLE;
}

/**
 *@brief @b 函数名称:   void SYS_Init(SYS_InitTypeDef* SYS_InitStruct)
 *@brief @b 功能描述:   SYS模块初始化函数
 *@see被调用函数：      无
 *@param输入参数：      SYS_InitTypeDef* SYS_InitStruct
 *@param输出参数：      无
 *@return返 回 值：     无。
 *@warning             无
 *@par 示例代码：
 *@code
           SYS_InitTypeDef SYS_InitStruct;
           SYS_StructInit(&SYS_InitStruct);
           SYS_InitStruct.PLL_SrcSel = SYS_PLLSRSEL_RCH;    // 使用内部12MHz晶振作为时钟输入源
           SYS_Init(&SYS_InitStruct);
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version  <th>Author  <th>Description
 * <tr><td>2023年02月12日 <td>1.0     <td>yangl      <td>创建
 * </table>
 */

// 时钟源选择PLL_SrcSel  					RC/XTAL_12M/XTAL_24M
// 主时钟选择Clk_Sel							RC/PLL
void SYS_Init(SYS_InitTypeDef *SYS_InitStruct)
{
    uint32_t RCHPdn, XTALPdn;
    uint32_t temp1 = 0x0, temp4 = 0x0, tempA = 0x0, tempB = 0x0;
    SYS_WR_PROTECT = 0x7A83; // 解除系统寄存器写保护

    // 在配置sys寄存器前，先读回sys寄存器中不需要配置的位，以防止配置过程中影响起其它程序的运行效果
    temp1 = SYS_AFE_REG1 & (~(BIT10));
    temp4 = SYS_AFE_REG4 & (~(BIT6));
    tempA = SYS_AFE_REGA & (~(BIT1 | BIT6 | BIT7));
    tempB = SYS_AFE_REGB & (~(BIT7));

    if (SYS_InitStruct->PLL_SrcSel == SYS_PLLSRSEL_CRYSTAL_12M) //  选择12M 外置晶体作为时钟源
    {
        RCHPdn = DISABLE;       /*RCH(12MHz)时钟关闭*/
        XTALPdn = ENABLE;       /*晶体起振电路开启*/
        tempB |= (ENABLE << 7); // PLL 参考时钟选择XTAL
        SYS_AFE_REGB = tempB;
    }
    else if (SYS_InitStruct->PLL_SrcSel == SYS_PLLSRSEL_CRYSTAL_24M) //  选择24M 外置晶体作为时钟源
    {
        RCHPdn = DISABLE;
        XTALPdn = ENABLE;
        temp1 |= (ENABLE << 10);
        temp4 |= (ENABLE << 6);
        tempB |= (ENABLE << 7); // PLL 参考时钟选择XTAL
        SYS_AFE_REG1 = temp1;
        SYS_AFE_REG4 = temp4;
        SYS_AFE_REGB = tempB;
    }
    else //  选择HSI 作为时钟源
    {
        RCHPdn = ENABLE;
        XTALPdn = DISABLE;
    }

    tempA |= (RCHPdn << 1);
    tempA |= (XTALPdn << 6);
    SYS_AFE_REGA = tempA;

    if (SYS_InitStruct->Clk_Sel == CLK_SEL_PLL) // 主时钟最后选择PLL, 才使能PLL
    {
        SYS_AFE_REGA |= (ENABLE << 7);
    }

    SYS_CLK_CFG = SYS_InitStruct->PLL_DivSel + (SYS_InitStruct->Clk_Sel << 8); // 配置时钟控制寄存器

    // 看门狗
    if (SYS_InitStruct->WDT_Ena == DISABLE)
    {
        IWDG_PSW = 0xA6B4;
        IWDG_CFG = 0x3C00; // disable iwdg
        WWDG_CR = 0x3C7F;  // disable wwdg
    }
    SYS_WR_PROTECT = 0;
}

/*! @} */
