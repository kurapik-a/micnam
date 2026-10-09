/**
 * @file
 * @defgroup gpio
 * @brief GPIO库函数
 * @author YangZJ
 * @date 2023-01-02
 * @{
 */
#include "lks32mc45x_gpio.h"
#include "lks32mc45x_sys.h"
#include "lks32mc45x.h"

/**
 * @brief GPIO初始化
 * @param GPIOx:GPIO对象, GPIO_InitStruct:GPIO初始化结构体
 * @par 更新记录  V1.0  2016/4/18  YangZJ  创建
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *this)
{
    SYS_ModuleClockCmd(SYS_MODULE_GPIO, ENABLE);
    switch (this->GPIO_PuPd) // 上拉选择
    {
    case GPIO_PuPd_NOPULL:
        GPIOx->PUE &= ~this->GPIO_Pin;
        GPIOx->PDE &= ~this->GPIO_Pin;
        break;
    case GPIO_PuPd_UP:
        GPIOx->PUE |= this->GPIO_Pin;
        GPIOx->PDE &= ~this->GPIO_Pin;
        break;
    case GPIO_PuPd_DOWN:
        GPIOx->PUE &= ~this->GPIO_Pin;
        GPIOx->PDE |= this->GPIO_Pin;
        break;
    }
    if (this->GPIO_PODEna)
    {
        GPIOx->PODE |= this->GPIO_Pin;
    }
    else
    {
        GPIOx->PODE &= ~this->GPIO_Pin;
    }
    if (this->GPIO_PFLT)
    {
        GPIOx->PFLT |= this->GPIO_Pin;
    }
    else
    {
        GPIOx->PFLT &= ~this->GPIO_Pin;
    }
    switch (this->GPIO_Mode)
    {
    case GPIO_Mode_IN:
        GPIOx->PIE |= this->GPIO_Pin;
        GPIOx->POE &= ~this->GPIO_Pin;
        break;
    case GPIO_Mode_OUT:
        GPIOx->PIE &= ~this->GPIO_Pin;
        GPIOx->POE |= this->GPIO_Pin;
        break;
    case GPIO_Mode_ANA:
        GPIOx->PIE &= ~this->GPIO_Pin;
        GPIOx->POE &= ~this->GPIO_Pin;
        break;
    case GPIO_Mode_INOUT:
        GPIOx->PIE |= this->GPIO_Pin;
        GPIOx->POE |= this->GPIO_Pin;
        break;
    }
}

/**
 * @brief    GPIO结构体初始化
 * @param GPIO_InitStruct:GPIO初始化结构体
 * @par 更新记录  V1.0  2016/4/18  YangZJ  创建
 */
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct)
{
    GPIO_InitStruct->GPIO_Pin = GPIO_Pin_NONE;
    GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct->GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct->GPIO_PODEna = DISABLE;
    GPIO_InitStruct->GPIO_PFLT = DISABLE;
}

/**
 * @brief    读取GPIO的指定Pin的输入值
 * @param GPIOx:GPIO对象, GPIO_Pin:指定的Pin
 * @return 指定Pin的输入值
 * @par 更新记录  V1.0  2016/7/1  YangZJ  创建
 */
u8 GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin)
{
    u8 bitstatus = 0x00;
    if ((GPIOx->PDI & GPIO_Pin) != 0)
    {
        bitstatus = (u8)Bit_SET;
    }
    else
    {
        bitstatus = (u8)Bit_RESET;
    }
    return bitstatus;
}

/**
 * @brief    读取GPIO的输入数据
 * @param GPIOx:GPIO对象
 * @return 输入数据
 * @par 更新记录  V1.0  2016/4/18  YangZJ  创建
 */
u32 GPIO_ReadInputData(GPIO_TypeDef *GPIOx)
{
    return GPIOx->PDI;
}

/**
 * @brief    读取GPIO的指定Pin的输出值
 * @param GPIOx:GPIO对象, GPIO_Pin:指定的Pin
 * @return 指定Pin的输出值
 * @par 更新记录  V1.0  2016/4/18  YangZJ  创建
 */
u8 GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin)
{
    u8 bitstatus = 0x00;
    if ((GPIOx->PDO & GPIO_Pin) != 0)
    {
        bitstatus = (u8)Bit_SET;
    }
    else
    {
        bitstatus = (u8)Bit_RESET;
    }
    return bitstatus;
}

/**
 * @brief    读取GPIO的当前输出数据
 * @param GPIOx:GPIO对象
 * @return 输出数据
 * @par 更新记录  V1.0  2016/4/18  YangZJ  创建
 */
u32 GPIO_ReadOutputData(GPIO_TypeDef *GPIOx)
{
    return GPIOx->PDO;
}

/**
 * @brief    GPIO指定Pin置1
 * @param GPIOx:GPIO对象, GPIO_Pin:指定的Pin
 * @par 更新记录  V1.0  2016/7/2  YangZJ  创建
 */
void GPIO_SetBits(GPIO_TypeDef *GPIOx, u16 GPIO_Pin)
{
    GPIOx->BSRR = GPIO_Pin;
}

/**
 * @brief    GPIO指定Pin置0
 * @param GPIOx:GPIO对象, GPIO_Pin:指定的Pin
 * @par 更新记录  V1.0  2016/7/2  YangZJ  创建
 */
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, u16 GPIO_Pin)
{
    GPIOx->BRR = GPIO_Pin;
}

/**
 * @brief    向GPIO指定的Pin写入数据
 * @param GPIOx:GPIO对象, GPIO_Pin:指定的Pin, BitVal:写入的Bit值
 * @par 更新记录  V1.0  2016/4/18  YangZJ  创建
 */
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, u16 GPIO_Pin, GPIO_BitAction BitVal)
{
    if (BitVal != Bit_RESET)
    {
        GPIOx->BSRR = GPIO_Pin;
    }
    else
    {
        GPIOx->BRR = GPIO_Pin;
    }
}

/**
 * @brief    向GPIO写入数据
 * @param GPIOx:GPIO对象, Val:写入的值
 * @par 更新记录  V1.0  2016/4/18  YangZJ  创建
 */
void GPIO_Write(GPIO_TypeDef *GPIOx, u32 Val)
{
    GPIOx->PDO = Val;
}

/**
 * @brief    GPIO功能选择
 * @param GPIOx:GPIO对象, GPIO_PadSource:指定的PadSource, GPIO_AF:指定的功能
 * @par 更新记录  V1.0  2016/6/19  YangZJ  创建
 */
void GPIO_PinAFConfig(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource, u32 GPIO_AF)
{
    volatile u32 *p;
    u32 reg;
    p = &GPIOx->F3210;
    reg = p[GPIO_PinSource >> 2];
    reg &= (u32) ~(0xf << ((GPIO_PinSource & 3) * 4));
    reg |= GPIO_AF << ((GPIO_PinSource & 3) * 4);
    p[GPIO_PinSource >> 2] = reg;
}

/**
 * @brief    GPIO关闭
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_Config(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource, GPIO_Mode_TypeDef GPIO_Mode, u32 GPIO_AF)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
    GPIO_InitStruct.GPIO_Pin = 1 << GPIO_PinSource;
    if (GPIO_Mode == GPIO_Mode_INOUT)
    {
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStruct.GPIO_PODEna = ENABLE;
    }
    else
    {
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStruct.GPIO_PODEna = DISABLE;
    }
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOx, GPIO_PinSource, GPIO_AF);
}

/**
 * @brief    GPIO关闭
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_Enable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_GPIO, ENABLE);
}
/**
 * @brief    GPIO关闭
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_Disable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_GPIO, DISABLE);
}
/**
 * @brief    读取单个Pin的输入电平
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
u8 GPIO_ReadPinBit(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource)
{
    if (GPIO_ReadInputDataBit(GPIOx, (u16)(1 << GPIO_PinSource)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    向单个Pin写入输出电平
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_WritePinBit(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource, u8 vol)
{
    if (vol)
    {
        GPIO_SetBits(GPIOx, (u16)(1 << GPIO_PinSource));
    }
    else
    {
        GPIO_ResetBits(GPIOx, (u16)(1 << GPIO_PinSource));
    }
}
/**
 * @brief    将单个Pin脚的输出设置为高
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_SetPinBits(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource)
{
    GPIO_SetBits(GPIOx, (u16)(1 << GPIO_PinSource));
}
/**
 * @brief    将单个Pin脚的输出设置为低
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_ResetPinBits(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource)
{
    GPIO_ResetBits(GPIOx, (u16)(1 << GPIO_PinSource));
}
/**
 * @brief    读取单个Pin的输出电平
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
u8 GPIO_ReadPinOutBit(GPIO_TypeDef *GPIOx, u16 GPIO_PinSource)
{
    if (GPIO_ReadOutputDataBit(GPIOx, (u16)(1 << GPIO_PinSource)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief    GPIO外部中断配置
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_ExtiConfig(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource, u8 GPIO_EXTI_MODE_x)
{
    u8 crx;
    u16 clrbit;
    u16 setbit;

    crx = (GPIO_PinSource >= 8) ? 0 : 1;
    GPIO_PinSource = GPIO_PinSource & 0x7;
    clrbit = (u16)(~(0x3 << (GPIO_PinSource * 2)));
    setbit = (u16)(GPIO_EXTI_MODE_x << (GPIO_PinSource * 2));
    if (GPIOx == GPIO0)
    {
        if (crx)
        {
            EXTI0->CR0 = (EXTI0->CR0 & clrbit) | setbit;
        }
        else
        {
            EXTI0->CR1 = (EXTI0->CR1 & clrbit) | setbit;
        }
    }
    else if (GPIOx == GPIO1)
    {
        if (crx)
        {
            EXTI1->CR0 = (EXTI1->CR0 & clrbit) | setbit;
        }
        else
        {
            EXTI1->CR1 = (EXTI1->CR1 & clrbit) | setbit;
        }
    }
    else if (GPIOx == GPIO2)
    {
        if (crx)
        {
            EXTI2->CR0 = (EXTI2->CR0 & clrbit) | setbit;
        }
        else
        {
            EXTI2->CR1 = (EXTI2->CR1 & clrbit) | setbit;
        }
    }
    else if (GPIOx == GPIO3)
    {
        if (crx)
        {
            EXTI3->CR0 = (EXTI3->CR0 & clrbit) | setbit;
        }
        else
        {
            EXTI3->CR1 = (EXTI3->CR1 & clrbit) | setbit;
        }
    }
    else if (GPIOx == GPIO4)
    {
        if (crx)
        {
            EXTI4->CR0 = (EXTI4->CR0 & clrbit) | setbit;
        }
        else
        {
            EXTI4->CR1 = (EXTI4->CR1 & clrbit) | setbit;
        }
    }
    else if (GPIOx == GPIO5)
    {
        if (crx)
        {
            EXTI5->CR0 = (EXTI5->CR0 & clrbit) | setbit;
        }
        else
        {
            EXTI5->CR1 = (EXTI5->CR1 & clrbit) | setbit;
        }
    }
}

/**
 * @brief    清除指定引脚的中断标志
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
void GPIO_ClearIRQFlag(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource)
{
    if (GPIOx == GPIO0)
    {
        EXTI0->IF = (u16)1 << GPIO_PinSource;
    }
    else if (GPIOx == GPIO1)
    {
        EXTI1->IF = (u16)1 << GPIO_PinSource;
    }
    else if (GPIOx == GPIO2)
    {
        EXTI2->IF = (u16)1 << GPIO_PinSource;
    }
    else if (GPIOx == GPIO3)
    {
        EXTI3->IF = (u16)1 << GPIO_PinSource;
    }
    else if (GPIOx == GPIO4)
    {
        EXTI4->IF = (u16)1 << GPIO_PinSource;
    }
    else if (GPIOx == GPIO5)
    {
        EXTI5->IF = (u16)1 << GPIO_PinSource;
    }
}

/**
 * @brief    读取某个引脚的中断标志
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
u8 GPIO_GetIRQFlag(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource)
{

    if (GPIOx == GPIO0)
    {
        if (EXTI0->IF & ((u16)1 << GPIO_PinSource))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (GPIOx == GPIO1)
    {
        if (EXTI1->IF & ((u16)1 << GPIO_PinSource))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (GPIOx == GPIO2)
    {
        if (EXTI2->IF & ((u16)1 << GPIO_PinSource))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (GPIOx == GPIO3)
    {
        if (EXTI3->IF & ((u16)1 << GPIO_PinSource))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (GPIOx == GPIO4)
    {
        if (EXTI4->IF & ((u16)1 << GPIO_PinSource))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (GPIOx == GPIO5)
    {
        if (EXTI5->IF & ((u16)1 << GPIO_PinSource))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
/**
 * @brief    读取某引脚的中断标志
 * @par 更新记录  V1.0  2022/8/16  YangZJ  创建
 */
u8 GPIO_GetFlag(GPIO_TypeDef *GPIOx, u32 GPIO_PinSource)
{
    return GPIO_GetIRQFlag(GPIOx, GPIO_PinSource);
}

/*! @} */
