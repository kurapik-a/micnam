#include "at32f435_437.h" // Device header
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/**
 * @brief  配置PA8引脚作为时钟输出引脚
 * @note   该函数将PA8配置为复用功能模式，用于输出系统时钟信号
 * @param  无
 * @retval 无
 */
void clkout_pin_PA8(void)
{
    gpio_init_type gpio_init_struct;
    /* 使能GPIOA外设时钟 */
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    /* 配置GPIO参数 */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER; /* 设置驱动强度为较强 */
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;                          /* 设置为复用功能模式 */
    gpio_init_struct.gpio_pins = GPIO_PINS_8;                            /* 选择引脚8 */
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;                         /* 不设置上下拉 */
    /* 初始化GPIOA */
    gpio_init(GPIOA, &gpio_init_struct);
    /* 配置时钟输出分频 */
    crm_clkout_div_set(CRM_CLKOUT_INDEX_1, CRM_CLKOUT_DIV1_4, CRM_CLKOUT_DIV2_1);
    /* 配置时钟输出源为PLL */
    crm_clock_out1_set(CRM_CLKOUT1_PLL);
}

/**
 * @brief  初始化指定的GPIO引脚
 * @param  gpio_x: 指向GPIO端口的指针，可以是GPIOA、GPIOB、GPIOC、GPIOD或GPIOE
 * @param  PIN: 要初始化的GPIO引脚，可以是多个引脚的组合
 * @param  mode: GPIO工作模式，如输入模式、输出模式、复用功能模式等
 * @param  type: GPIO输出类型，如推挽输出或开漏输出
 * @param  pull: GPIO上下拉类型，如上拉、下拉或不设置上下拉
 * @retval None
 * gpio_input_data_bit_read(GPIOA, GPIO_PINS_8);
 * gpio_bits_write(GPIOA, GPIO_PINS_8,1);
 * GPIO_INIT(GPIOB, GPIO_PINS_5, GPIO_MODE_OUTPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_NONE);
 */
void GPIO_INIT(gpio_type *gpio_x, uint32_t PIN, gpio_mode_type mode, gpio_output_type type, gpio_pull_type pull)
{
    gpio_init_type gpio_init_struct;
    switch ((uint32_t)gpio_x)
    {
    case GPIOA_BASE:
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
        break;
    case GPIOB_BASE:
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        break;
    case GPIOC_BASE:
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
        break;
    case GPIOD_BASE:
        crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
        break;
    case GPIOE_BASE:
        crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
        break;
    }
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER; /* 设置驱动强度为较强 */
    gpio_init_struct.gpio_mode = mode;                                   /* 设置为复用功能模式 */
    gpio_init_struct.gpio_pins = PIN;                                    /* 选择引脚8 */
    gpio_init_struct.gpio_pull = pull;                                   /* 不设置上下拉 */
    gpio_init_struct.gpio_out_type = type;
    /* 初始化GPIOA */
    gpio_init(gpio_x, &gpio_init_struct);
}
// -------------------------------- CAN --------------------------------

/**
 * @brief CAN配置函数
 * @return error_status 配置成功返回SUCCESS，失败返回ERROR
 */
error_status can_configuration(void)
{
    /* 定义CAN相关结构体变量 */
    can_base_type can_base_struct;               // CAN基础配置结构体
    can_baudrate_type can_baudrate_struct;       // CAN波特率配置结构体
    can_filter_init_type can_filter_init_struct; // CAN过滤器初始化结构体
    gpio_init_type gpio_init_struct;             // GPIO初始化结构体
    /* 使能GPIOA时钟 */
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    /* 配置GPIO引脚 */
    gpio_default_para_init(&gpio_init_struct);                           // 使用默认参数初始化GPIO结构体
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER; // 设置GPIO驱动强度为更强
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;              // 设置GPIO输出类型为推挽输出
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;                          // 设置GPIO模式为复用功能
    gpio_init_struct.gpio_pins = GPIO_PINS_11 | GPIO_PINS_12;            // 选择PA11和PA12引脚
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;                         // 设置GPIO上下拉为无
    gpio_init(GPIOA, &gpio_init_struct);                                 // 初始化GPIOA
    /* 配置GPIO引脚复用功能为CAN */
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE11, GPIO_MUX_9); // PA11复用为CAN_RX
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE12, GPIO_MUX_9); // PA12复用为CAN_TX
    /* 检查外部晶振是否稳定 */
    if (crm_flag_get(CRM_HEXT_STABLE_FLAG) != SET)
    {
        return ERROR; // 如果晶振不稳定，返回错误
    }
    /* 使能CAN1时钟 */
    crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, TRUE);
    /* 配置CAN基础参数 */
    can_default_para_init(&can_base_struct);                          // 使用默认参数初始化CAN基础结构体
    can_base_struct.mode_selection = CAN_MODE_COMMUNICATE;            // 设置CAN模式为正常通信模式
    can_base_struct.ttc_enable = FALSE;                               // 禁止时间触发通信
    can_base_struct.aebo_enable = TRUE;                               // 启用自动总线恢复
    can_base_struct.aed_enable = TRUE;                                // 启用自动禁用管理
    can_base_struct.prsf_enable = FALSE;                              // 禁止自动重传
    can_base_struct.mdrsel_selection = CAN_DISCARDING_FIRST_RECEIVED; // 丢弃第一个接收的报文
    can_base_struct.mmssr_selection = CAN_SENDING_BY_ID;              // 按ID发送报文
    can_base_init(CAN1, &can_base_struct);                            // 初始化CAN基础参数
    /* 配置CAN波特率 */
    can_baudrate_struct.baudrate_div = 12;        // 波特率分频
    can_baudrate_struct.rsaw_size = CAN_RSAW_3TQ; // 重新同步采样宽度
    can_baudrate_struct.bts1_size = CAN_BTS1_8TQ; // 时间段1
    can_baudrate_struct.bts2_size = CAN_BTS2_3TQ; // 时间段2
    if (can_baudrate_set(CAN1, &can_baudrate_struct) != SUCCESS)
    {
        return ERROR; // 如果波特率设置失败，返回错误
    }
    /* 配置CAN过滤器 */
    can_filter_init_struct.filter_activate_enable = TRUE;         // 启用过滤器
    can_filter_init_struct.filter_mode = CAN_FILTER_MODE_ID_MASK; // 设置过滤器模式为ID掩码模式
    can_filter_init_struct.filter_fifo = CAN_FILTER_FIFO0;        // 使用FIFO0
    can_filter_init_struct.filter_number = 0;                     // 使用过滤器0
    can_filter_init_struct.filter_bit = CAN_FILTER_32BIT;         // 使用32位过滤器
    can_filter_init_struct.filter_id_high = 0;                    // 过滤器ID高16位
    can_filter_init_struct.filter_id_low = 0;                     // 过滤器ID低16位
    can_filter_init_struct.filter_mask_high = 0;                  // 过滤器掩码高16位
    can_filter_init_struct.filter_mask_low = 0;                   // 过滤器掩码低16位
    can_filter_init(CAN1, &can_filter_init_struct);               // 初始化CAN过滤器
    /* 配置CAN中断 */
    nvic_irq_enable(CAN1_SE_IRQn, 0x00, 0x00);         // 使能CAN1状态改变中断
    nvic_irq_enable(CAN1_RX0_IRQn, 0x00, 0x00);        // 使能CAN1 FIFO0接收中断
    can_interrupt_enable(CAN1, CAN_RF0MIEN_INT, TRUE); // 使能FIFO0满中断
    can_interrupt_enable(CAN1, CAN_ETRIEN_INT, TRUE);  // 使能错误类型中断
    can_interrupt_enable(CAN1, CAN_EOIEN_INT, TRUE);   // 使能错误中断
    return SUCCESS;                                    // 配置成功，返回成功
}
void can_transmit_data(void)
{
    uint8_t transmit_mailbox;
    can_tx_message_type tx_message_struct;
    tx_message_struct.standard_id = 0x400;
    tx_message_struct.extended_id = 0;
    tx_message_struct.id_type = CAN_ID_STANDARD;
    tx_message_struct.frame_type = CAN_TFT_REMOTE;
    tx_message_struct.dlc = 0;
    tx_message_struct.data[0] = 0x51;
    tx_message_struct.data[1] = 0x52;
    tx_message_struct.data[2] = 0x53;
    tx_message_struct.data[3] = 0x54;
    tx_message_struct.data[4] = 0x55;
    tx_message_struct.data[5] = 0x56;
    tx_message_struct.data[6] = 0x57;
    tx_message_struct.data[7] = 0x58;
    transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct);
    while (can_transmit_status_get(CAN1, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL)
        ;
}

/**
 * @brief CAN2配置函数
 * @return error_status 配置结果，SUCCESS表示成功，ERROR表示失败
 */
error_status can2_configuration(void)
{
    // 定义CAN基础结构体
    can_base_type can_base_struct;
    // 定义CAN波特率结构体
    can_baudrate_type can_baudrate_struct;
    // 定义CAN过滤器初始化结构体
    can_filter_init_type can_filter_init_struct;

    GPIO_INIT(GPIOB, GPIO_PINS_5, GPIO_MODE_MUX, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_NONE);
    GPIO_INIT(GPIOB, GPIO_PINS_6, GPIO_MODE_MUX, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_NONE);

    // 配置PA11引脚复用功能为CAN
    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE5, GPIO_MUX_9);
    // 配置PA12引脚复用功能为CAN
    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_9);
    // 检查外部晶振是否稳定
    if (crm_flag_get(CRM_HEXT_STABLE_FLAG) != SET)
    {
        return ERROR;
    }
    // 使能CAN2时钟
    crm_periph_clock_enable(CRM_CAN2_PERIPH_CLOCK, TRUE);
    // 初始化CAN参数为默认值
    can_default_para_init(&can_base_struct);
    // 配置CAN模式为普通通信模式
    can_base_struct.mode_selection = CAN_MODE_COMMUNICATE;
    // 配置CAN时间触发通信为禁用
    can_base_struct.ttc_enable = FALSE;
    // 配置CAN自动恢复总线为启用
    can_base_struct.aebo_enable = TRUE;
    // 配置CAN自动退出安静模式为启用
    can_base_struct.aed_enable = TRUE;
    // 配置CAN接收报文存储为禁用
    can_base_struct.prsf_enable = FALSE;
    // 配置CAN多报文存储模式为丢弃第一个接收到的报文
    can_base_struct.mdrsel_selection = CAN_DISCARDING_FIRST_RECEIVED;
    // 配置CAN发送报文优先级为按ID排序
    can_base_struct.mmssr_selection = CAN_SENDING_BY_ID;
    // 初始化CAN2
    can_base_init(CAN2, &can_base_struct);
    // 配置CAN波特率参数
    can_baudrate_struct.baudrate_div = 12;        // 波特率分频
    can_baudrate_struct.rsaw_size = CAN_RSAW_3TQ; // 重新同步采样宽度为3个时间单位
    can_baudrate_struct.bts1_size = CAN_BTS1_8TQ; // 时间段1为8个时间单位
    can_baudrate_struct.bts2_size = CAN_BTS2_3TQ; // 时间段2为3个时间单位
    // 设置CAN波特率
    if (can_baudrate_set(CAN2, &can_baudrate_struct) != SUCCESS)
    {
        return ERROR;
    }
    // 配置CAN过滤器参数
    can_filter_init_struct.filter_activate_enable = TRUE;         // 启用过滤器
    can_filter_init_struct.filter_mode = CAN_FILTER_MODE_ID_MASK; // 过滤器模式为掩码模式
    can_filter_init_struct.filter_fifo = CAN_FILTER_FIFO0;        // 过滤器关联FIFO0
    can_filter_init_struct.filter_number = 0;                     // 过滤器编号为0
    can_filter_init_struct.filter_bit = CAN_FILTER_32BIT;         // 过滤器位宽为32位
    can_filter_init_struct.filter_id_high = 0;                    // 过滤器ID高16位
    can_filter_init_struct.filter_id_low = 0;                     // 过滤器ID低16位
    can_filter_init_struct.filter_mask_high = 0;                  // 过滤器掩码高16位
    can_filter_init_struct.filter_mask_low = 0;                   // 过滤器掩码低16位
    // 初始化CAN过滤器
    can_filter_init(CAN2, &can_filter_init_struct);
    // 使能CAN2发送中断
    nvic_irq_enable(CAN2_SE_IRQn, 0x00, 0x00);
    // 使能CAN2接收中断
    nvic_irq_enable(CAN2_RX0_IRQn, 0x00, 0x00);
    // 使能CAN2FIFO0报文中断
    can_interrupt_enable(CAN2, CAN_RF0MIEN_INT, TRUE);
    // 使能CAN2错误中断
    can_interrupt_enable(CAN2, CAN_ETRIEN_INT, TRUE);
    // 使能CAN2溢出中断
    can_interrupt_enable(CAN2, CAN_EOIEN_INT, TRUE);
    return SUCCESS;
}
void can2_transmit_data(void)
{
    uint8_t transmit_mailbox;
    can_tx_message_type tx_message_struct;
    tx_message_struct.standard_id = 0x400;
    tx_message_struct.extended_id = 0;
    tx_message_struct.id_type = CAN_ID_STANDARD;
    tx_message_struct.frame_type = CAN_TFT_DATA;
    tx_message_struct.dlc = 8;
    tx_message_struct.data[0] = 0x21;
    tx_message_struct.data[1] = 0x22;
    tx_message_struct.data[2] = 0x23;
    tx_message_struct.data[3] = 0x24;
    tx_message_struct.data[4] = 0x25;
    tx_message_struct.data[5] = 0x26;
    tx_message_struct.data[6] = 0x27;
    tx_message_struct.data[7] = 0x28;
    transmit_mailbox = can_message_transmit(CAN2, &tx_message_struct);
    while (can_transmit_status_get(CAN2, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL)
        ;
}

// -------------------------------- CAN --------------------------------

// -------------------------------- IIC1 --------------------------------
#define I2C_TIMEOUT 0xFFFFFFF

// #define I2Cx_CLKCTRL 0xB170FFFF // 10K
// #define I2Cx_CLKCTRL                   0xC0E06969   //50K
// #define I2Cx_CLKCTRL                     0x80504C4E   //100K
// #define I2Cx_CLKCTRL 0x30F03C6B // 200K
// #define I2Cx_CLKCTRL 0x10F03C6B // 390khz
#define I2Cx_CLKCTRL 0x00F03C6B // 450khz

#define I2Cx_ADDRESS 0x01 // 设置本地通信地址

#define I2Cx_PORT I2C1
#define I2Cx_CLK CRM_I2C1_PERIPH_CLOCK
#define I2Cx_DMA DMA1
#define I2Cx_DMA_CLK CRM_DMA1_PERIPH_CLOCK

#define I2Cx_SCL_GPIO_CLK CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SCL_GPIO_PIN GPIO_PINS_8
#define I2Cx_SCL_GPIO_PinsSource GPIO_PINS_SOURCE8
#define I2Cx_SCL_GPIO_PORT GPIOB
#define I2Cx_SCL_GPIO_MUX GPIO_MUX_4

#define I2Cx_SDA_GPIO_CLK CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SDA_GPIO_PIN GPIO_PINS_9
#define I2Cx_SDA_GPIO_PinsSource GPIO_PINS_SOURCE9
#define I2Cx_SDA_GPIO_PORT GPIOB
#define I2Cx_SDA_GPIO_MUX GPIO_MUX_4

#define I2Cx_DMA_TX_Channel DMA1_CHANNEL1
#define I2Cx_DMA_TX_DMAMUX_Channel DMA1MUX_CHANNEL1
#define I2Cx_DMA_TX_DMAREQ DMAMUX_DMAREQ_ID_I2C1_TX
#define I2Cx_DMA_TX_IRQn DMA1_Channel1_IRQn

#define I2Cx_DMA_RX_Channel DMA1_CHANNEL2
#define I2Cx_DMA_RX_DMAMUX_Channel DMA1MUX_CHANNEL2
#define I2Cx_DMA_RX_DMAREQ DMAMUX_DMAREQ_ID_I2C1_RX
#define I2Cx_DMA_RX_IRQn DMA1_Channel2_IRQn

#define I2Cx_EVT_IRQn I2C1_EVT_IRQn
#define I2Cx_ERR_IRQn I2C1_ERR_IRQn

#define I2Cx_DMA_TX_IRQHandler DMA1_Channel1_IRQHandler
#define I2Cx_DMA_RX_IRQHandler DMA1_Channel2_IRQHandler
#define I2Cx_EVT_IRQHandler I2C1_EVT_IRQHandler
#define I2Cx_ERR_IRQHandler I2C1_ERR_IRQHandler

uint8_t I2C1_tx_buf[I2C_size]; // 1kb刚好可以刷新一遍OLED
uint8_t I2C1_rx_buf[I2C_size];

i2c_handle_type hi2cx;
void I2C1_init(void)
{
    hi2cx.i2cx = I2Cx_PORT;
    i2c_config(&hi2cx);
    hi2cx.status = 1;
}

void I2C1_send_data(uint16_t address, uint8_t *pdata, uint16_t size)
{
    if (hi2cx.status == 1)
        i2c_master_transmit_dma(&hi2cx, address, pdata, size, I2C_TIMEOUT);
}
void I2C1_read_data(uint16_t address, uint8_t *pdata, uint16_t size)
{
    if (hi2cx.status == 1)
        i2c_master_receive_dma(&hi2cx, address, pdata, size, I2C_TIMEOUT);
}
void I2Cx_DMA_RX_IRQHandler(void)
{
    i2c_dma_rx_irq_handler(&hi2cx);
    /* DMA 接收完成即视为本次 I2C 操作结束，直接给出信号量
       （不再依赖 STOP 中断，避免其未触发导致任务卡死在 xSemaphoreTake） */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xSemaphoreGiveFromISR(xI2C1_Sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	
}

/**
 * @brief  this function handles dma interrupt request.
 * @param  none
 * @retval none
 */
void I2Cx_DMA_TX_IRQHandler(void)
{
    i2c_dma_tx_irq_handler(&hi2cx);
    /* DMA 发送完成即视为本次 I2C 操作结束，直接给出信号量 */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xSemaphoreGiveFromISR(xI2C1_Sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

/**
 * @brief  this function handles i2c event interrupt request.
 * @param  none
 * @retval none
 */
void I2Cx_EVT_IRQHandler(void)
{
    /* 仅处理 STOP/状态清理，信号量改由 DMA 完成中断给出（最可靠），避免重复 give */
    i2c_evt_irq_handler(&hi2cx);
}

/**
 * @brief  this function handles i2c error interrupt request.
 * @param  none
 * @retval none
 */
void I2Cx_ERR_IRQHandler(void)
{
    i2c_err_irq_handler(&hi2cx);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xSemaphoreGiveFromISR(xI2C1_Sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//-------------------------------- 硬件IIC2 --------------------------------
#define I2C_TIMEOUT 0xFFFFFFF

// #define I2Cy_CLKCTRL 0xB170FFFF // 10K
//#define I2Cy_CLKCTRL 0xC0E06969 // 50K
// #define I2Cy_CLKCTRL                     0x80504C4E   //100K
// #define I2Cy_CLKCTRL 0x30F03C6B // 200K
// #define I2Cy_CLKCTRL 0x10F03C6B // 390khz
 #define I2Cy_CLKCTRL 0x00F03C6B // 450khz

#define I2Cy_ADDRESS 0x01 // 设置本地通信地址

#define I2Cy_PORT I2C2
#define I2Cy_CLK CRM_I2C2_PERIPH_CLOCK
#define I2Cy_DMA DMA1
#define I2Cy_DMA_CLK CRM_DMA1_PERIPH_CLOCK

#define I2Cy_SCL_GPIO_CLK CRM_GPIOB_PERIPH_CLOCK
#define I2Cy_SCL_GPIO_PIN GPIO_PINS_10
#define I2Cy_SCL_GPIO_PinsSource GPIO_PINS_SOURCE10
#define I2Cy_SCL_GPIO_PORT GPIOB
#define I2Cy_SCL_GPIO_MUX GPIO_MUX_4

#define I2Cy_SDA_GPIO_CLK CRM_GPIOB_PERIPH_CLOCK
#define I2Cy_SDA_GPIO_PIN GPIO_PINS_11
#define I2Cy_SDA_GPIO_PinsSource GPIO_PINS_SOURCE11
#define I2Cy_SDA_GPIO_PORT GPIOB
#define I2Cy_SDA_GPIO_MUX GPIO_MUX_4

#define I2Cy_DMA_TX_Channel DMA1_CHANNEL3
#define I2Cy_DMA_TX_DMAMUX_Channel DMA1MUX_CHANNEL3
#define I2Cy_DMA_TX_DMAREQ DMAMUX_DMAREQ_ID_I2C2_TX
#define I2Cy_DMA_TX_IRQn DMA1_Channel3_IRQn

#define I2Cy_DMA_RX_Channel DMA1_CHANNEL4
#define I2Cy_DMA_RX_DMAMUX_Channel DMA1MUX_CHANNEL4
#define I2Cy_DMA_RX_DMAREQ DMAMUX_DMAREQ_ID_I2C2_RX
#define I2Cy_DMA_RX_IRQn DMA1_Channel4_IRQn

#define I2Cy_EVT_IRQn I2C2_EVT_IRQn
#define I2Cy_ERR_IRQn I2C2_ERR_IRQn

 #define I2Cy_DMA_TX_IRQHandler DMA1_Channel3_IRQHandler
 #define I2Cy_DMA_RX_IRQHandler DMA1_Channel4_IRQHandler
#define I2Cy_EVT_IRQHandler I2C2_EVT_IRQHandler
#define I2Cy_ERR_IRQHandler I2C2_ERR_IRQHandler

uint8_t I2C2_tx_buf[I2C_size]; // 1kb刚好可以刷新一遍OLED
uint8_t I2C2_rx_buf[I2C_size];

i2c_handle_type hi2cy;
void I2C2_init(void) // 若要使用DMA就需要将中断函数解除注释
{
    hi2cy.i2cx = I2Cy_PORT;
    i2c_config(&hi2cy);
    hi2cy.status = 1;
}
void I2C2_send_data(uint16_t address, uint8_t *pdata, uint16_t size)
{
    if (hi2cy.status == 1)
        i2c_master_transmit_dma(&hi2cy, address, pdata, size, I2C_TIMEOUT);
}
void I2C2_read_data(uint16_t address, uint8_t *pdata, uint16_t size)
{
    if (hi2cy.status == 1)
        i2c_master_receive_dma(&hi2cy, address, pdata, size, I2C_TIMEOUT);
}
void I2Cy_DMA_RX_IRQHandler(void)
{
    i2c_dma_rx_irq_handler(&hi2cy);
    /* DMA 接收完成即给出信号量，避免任务卡死 */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xSemaphoreGiveFromISR(xI2C2_Sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief  this function handles dma interrupt request.
 * @param  none
 * @retval none
 */
void I2Cy_DMA_TX_IRQHandler(void)
{
    i2c_dma_tx_irq_handler(&hi2cy);
    /* DMA 发送完成即给出信号量，避免任务卡死 */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xSemaphoreGiveFromISR(xI2C2_Sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief  this function handles i2c event interrupt request.
 * @param  none
 * @retval none
 */
void I2Cy_EVT_IRQHandler(void)
{
    i2c_evt_irq_handler(&hi2cy);
}

/**
 * @brief  this function handles i2c error interrupt request.
 * @param  none
 * @retval none
 */
void I2Cy_ERR_IRQHandler(void)
{
    i2c_err_irq_handler(&hi2cy);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xSemaphoreGiveFromISR(xI2C2_Sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief  阻塞等待 I2C 传输完成（替代 while(i2c_wait_end) 硬等）
 * @note   调度器运行期用信号量阻塞等待；初始化期（调度器未启动）回退到轮询
 * @param  hi2c : I2C 句柄
 * @param  xSem : 对应的二进制信号量
 * @retval none
 */
void i2c_wait_complete(i2c_handle_type *hi2c, SemaphoreHandle_t xSem)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
    {
        if (xSemaphoreTake(xSem, pdMS_TO_TICKS(100)) != pdTRUE)
        {
            /* 100ms 内未收到完成信号：I2C 总线可能卡死，复位外设以自恢复 */
            i2c_config(hi2c);   /* i2c_config 内部会先 i2c_reset 再重新初始化 */
            hi2c->status = 1;   /* 标记 I2C_END，保证后续传输可继续 */
        }
    }
    else
        while (i2c_wait_end(hi2c, 0xFFFFFFF) != I2C_OK); // 初始化期：轮询回退
}

void i2c_lowlevel_init(i2c_handle_type *hi2c)
{
    gpio_init_type gpio_init_structure;

    if (hi2c->i2cx == I2Cx_PORT)
    {
        /* i2c periph clock enable */
        crm_periph_clock_enable(I2Cx_CLK, TRUE);
        crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
        crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

        /* gpio configuration */
        gpio_pin_mux_config(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_GPIO_PinsSource, I2Cx_SCL_GPIO_MUX);

        gpio_pin_mux_config(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_GPIO_PinsSource, I2Cx_SDA_GPIO_MUX);

        /* configure i2c pins: scl */
        gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_structure.gpio_mode = GPIO_MODE_MUX;
        gpio_init_structure.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
        gpio_init_structure.gpio_pull = GPIO_PULL_NONE;

        gpio_init_structure.gpio_pins = I2Cx_SCL_GPIO_PIN;
        gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_init_structure);

        /* configure i2c pins: sda */
        gpio_init_structure.gpio_pins = I2Cx_SDA_GPIO_PIN;
        gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_init_structure);

        /* configure and enable i2c interrupt */
        /* 注意：EVT/ERR 中断会调用 FreeRTOS FromISR API，
           优先级必须 >= configMAX_SYSCALL_INTERRUPT_PRIORITY(5)，否则破坏内核 */
        nvic_irq_enable(I2Cx_EVT_IRQn, 5, 0);
        nvic_irq_enable(I2Cx_ERR_IRQn, 6, 0);

        /* configure and enable i2c dma channel interrupt */
        /* DMA 完成中断也要调用 xSemaphoreGiveFromISR，优先级必须 >= configMAX_SYSCALL_INTERRUPT_PRIORITY(5) */
        nvic_irq_enable(I2Cx_DMA_TX_IRQn, 5, 0);
        nvic_irq_enable(I2Cx_DMA_RX_IRQn, 5, 0);

        /* i2c dma tx and rx channels configuration */
        /* enable the dma clock */
        crm_periph_clock_enable(I2Cx_DMA_CLK, TRUE);

        /* i2c dma channel configuration */
        hi2c->dma_tx_channel = I2Cx_DMA_TX_Channel;
        hi2c->dma_rx_channel = I2Cx_DMA_RX_Channel;

        dma_reset(hi2c->dma_tx_channel);
        dma_reset(hi2c->dma_rx_channel);

        hi2c->dma_init_struct.peripheral_base_addr = (uint32_t)&hi2c->i2cx->txdt;     // 设置DMA外设基地址为I2C发送数据寄存器地址
        hi2c->dma_init_struct.memory_base_addr = 0;                                   // 设置DMA内存基地址（将在传输时设置）
        hi2c->dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;               // 设置DMA传输方向为内存到外设
        hi2c->dma_init_struct.buffer_size = 0xFFFF;                                   // 设置DMA缓冲区大小（将在传输时设置）
        hi2c->dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
        hi2c->dma_init_struct.memory_inc_enable = TRUE;                               // 允许内存地址自增
        hi2c->dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为字节
        hi2c->dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为字节
        hi2c->dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
        hi2c->dma_init_struct.priority = DMA_PRIORITY_LOW;                            // 设置DMA优先级为低

        dma_init(hi2c->dma_tx_channel, &hi2c->dma_init_struct);
        dma_init(hi2c->dma_rx_channel, &hi2c->dma_init_struct);

        dmamux_init(I2Cx_DMA_TX_DMAMUX_Channel, I2Cx_DMA_TX_DMAREQ);
        dmamux_init(I2Cx_DMA_RX_DMAMUX_Channel, I2Cx_DMA_RX_DMAREQ);

        dmamux_enable(I2Cx_DMA, TRUE);

        /* config i2c */
        i2c_init(hi2c->i2cx, 0x0F, I2Cx_CLKCTRL);

        i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
    }
    else if (hi2c->i2cx == I2Cy_PORT)
    {
        /* i2c periph clock enable */
        crm_periph_clock_enable(I2Cy_CLK, TRUE);
        crm_periph_clock_enable(I2Cy_SCL_GPIO_CLK, TRUE);
        crm_periph_clock_enable(I2Cy_SDA_GPIO_CLK, TRUE);

        /* gpio configuration */
        gpio_pin_mux_config(I2Cy_SCL_GPIO_PORT, I2Cy_SCL_GPIO_PinsSource, I2Cy_SCL_GPIO_MUX);

        gpio_pin_mux_config(I2Cy_SDA_GPIO_PORT, I2Cy_SDA_GPIO_PinsSource, I2Cy_SDA_GPIO_MUX);

        /* configure i2c pins: scl */
        gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_structure.gpio_mode = GPIO_MODE_MUX;
        gpio_init_structure.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
        gpio_init_structure.gpio_pull = GPIO_PULL_NONE;

        gpio_init_structure.gpio_pins = I2Cy_SCL_GPIO_PIN;
        gpio_init(I2Cy_SCL_GPIO_PORT, &gpio_init_structure);

        /* configure i2c pins: sda */
        gpio_init_structure.gpio_pins = I2Cy_SDA_GPIO_PIN;
        gpio_init(I2Cy_SDA_GPIO_PORT, &gpio_init_structure);

        /* configure and enable i2c interrupt */
        /* 注意：EVT/ERR 中断会调用 FreeRTOS FromISR API，
           优先级必须 >= configMAX_SYSCALL_INTERRUPT_PRIORITY(5)，否则破坏内核 */
        nvic_irq_enable(I2Cy_EVT_IRQn, 5, 0);
        nvic_irq_enable(I2Cy_ERR_IRQn, 6, 0);

        /* configure and enable i2c dma channel interrupt */
        nvic_irq_enable(I2Cy_DMA_TX_IRQn, 5, 0);
        nvic_irq_enable(I2Cy_DMA_RX_IRQn, 5, 0);

        /* i2c dma tx and rx channels configuration */
        /* enable the dma clock */
        crm_periph_clock_enable(I2Cy_DMA_CLK, TRUE);

        /* i2c dma channel configuration */
        hi2c->dma_tx_channel = I2Cy_DMA_TX_Channel;
        hi2c->dma_rx_channel = I2Cy_DMA_RX_Channel;

        dma_reset(hi2c->dma_tx_channel);
        dma_reset(hi2c->dma_rx_channel);

        hi2c->dma_init_struct.peripheral_base_addr = (uint32_t)&hi2c->i2cx->txdt;     // 设置DMA外设基地址为I2C发送数据寄存器地址
        hi2c->dma_init_struct.memory_base_addr = 0;                                   // 设置DMA内存基地址（将在传输时设置）
        hi2c->dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;               // 设置DMA传输方向为内存到外设
        hi2c->dma_init_struct.buffer_size = 0xFFFF;                                   // 设置DMA缓冲区大小（将在传输时设置）
        hi2c->dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
        hi2c->dma_init_struct.memory_inc_enable = TRUE;                               // 允许内存地址自增
        hi2c->dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为字节
        hi2c->dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为字节
        hi2c->dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
        hi2c->dma_init_struct.priority = DMA_PRIORITY_LOW;                            // 设置DMA优先级为低

        dma_init(hi2c->dma_tx_channel, &hi2c->dma_init_struct);
        dma_init(hi2c->dma_rx_channel, &hi2c->dma_init_struct);

        dmamux_init(I2Cy_DMA_TX_DMAMUX_Channel, I2Cy_DMA_TX_DMAREQ);
        dmamux_init(I2Cy_DMA_RX_DMAMUX_Channel, I2Cy_DMA_RX_DMAREQ);

        dmamux_enable(I2Cy_DMA, TRUE);

        /* config i2c */
        i2c_init(hi2c->i2cx, 0x0F, I2Cy_CLKCTRL);

        i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cy_ADDRESS);
    }
}

// -------------------------------- IIC2 --------------------------------

#include "at32f435_437_board.h"

void I2C_SOFTWARE_INIT(void)
{
    GPIO_INIT(scl, GPIO_MODE_OUTPUT, GPIO_OUTPUT_OPEN_DRAIN, GPIO_PULL_UP);
    GPIO_INIT(sda, GPIO_MODE_OUTPUT, GPIO_OUTPUT_OPEN_DRAIN, GPIO_PULL_UP);
    gpio_bits_write(scl, 1);
    gpio_bits_write(sda, 1);
    // gpio_input_data_bit_read(scl);
    // gpio_input_data_bit_read(sda);
}

void I2C_START(void)
{
    gpio_bits_write(sda, 1);
    delay_us(time_delay);
    gpio_bits_write(scl, 1);
    delay_us(time_delay);
    gpio_bits_write(sda, 0);
    delay_us(time_delay);
    gpio_bits_write(scl, 0);
    delay_us(time_delay);
}
void I2C_STOP(void)
{
    gpio_bits_write(sda, 0);
    delay_us(time_delay);
    gpio_bits_write(scl, 1);
    delay_us(time_delay);
    gpio_bits_write(sda, 1);
    delay_us(time_delay);
}

void I2C_SEND_BYTE(uint8_t data)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        gpio_bits_write(sda, (data >> 7) & 0x01);
        delay_us(time_delay);
        gpio_bits_write(scl, 1);
        delay_us(time_delay);
        gpio_bits_write(scl, 0);
        delay_us(time_delay);
        data <<= 1;
    }
}
uint8_t I2C_READ_BYTE(uint8_t send_ack)
{
    uint8_t i, data = 0;
    gpio_bits_write(sda, 1); // 先释放，由上拉拉高
    for (i = 0; i < 8; i++)
    {
        gpio_bits_write(scl, 1);
        delay_us(time_delay);
        data = (data << 1) | gpio_input_data_bit_read(sda);
        gpio_bits_write(scl, 0);
        delay_us(time_delay);
    }
    /* 发送 ACK/NACK */
    gpio_bits_write(sda, send_ack ? 0 : 1); // 0=拉低(ACK)  1=释放(NACK)
    delay_us(time_delay);
    gpio_bits_write(scl, 1);
    delay_us(time_delay);
    gpio_bits_write(scl, 0);
    delay_us(time_delay);
    gpio_bits_write(sda, 1); // 释放数据线
    return data;
}
void I2C_ACK(void)
{
    gpio_bits_write(sda, 0);
    delay_us(time_delay);
    gpio_bits_write(scl, 1);
    delay_us(time_delay);
    gpio_bits_write(scl, 0);
    delay_us(time_delay);
}
void I2C_NACK(void)
{
    gpio_bits_write(sda, 1);
    delay_us(time_delay);
    gpio_bits_write(scl, 1);
    delay_us(time_delay);
    gpio_bits_write(scl, 0);
    delay_us(time_delay);
}
// 返回 0:ACK  1:NACK
uint8_t I2C_GET_ACK(void)
{
    gpio_bits_write(sda, 1); // 主机释放 SDA，由上拉拉高
    delay_us(time_delay);
    gpio_bits_write(scl, 1); // 拉高时钟
    delay_us(time_delay);
    uint8_t ack = gpio_input_data_bit_read(sda); // 读总线电平
    gpio_bits_write(scl, 0);                     // 拉低时钟
    delay_us(time_delay);
    return ack; // 0:ACK(被从机拉低)  1:NACK(上拉保持高)
}
void I2C_BUS_RECOVER(void)
{
    uint8_t i;
    gpio_bits_write(sda, 1);
    gpio_bits_write(scl, 1);
    for (i = 0; i < 9; i++)
    {
        delay_us(10);
        gpio_bits_write(scl, 0);
        delay_us(10);
        gpio_bits_write(scl, 1);
    }
    I2C_STOP();
}

uint8_t I2C_WRITE(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    I2C_START();
    I2C_SEND_BYTE(slave_addr << 1);
    if (I2C_GET_ACK())
        goto nack;
    I2C_SEND_BYTE(reg_addr);
    if (I2C_GET_ACK())
        goto nack;
    I2C_SEND_BYTE(data);
    if (I2C_GET_ACK())
        goto nack;
    I2C_STOP();
    return 0; // 成功
nack:
    I2C_STOP();
    return 1; // 失败
}

/**
 * @brief  读单个寄存器
 * @param  slave_addr : 7 位从机地址
 * @param  reg_addr   : 寄存器地址
 * @param  p_data     : 接收缓冲区（单字节）
 * @retval 0:成功  1:寻址或写寄存器阶段无应答
 */
uint8_t I2C_READ_REG(uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_data)
{
    /* 第 1 步：写寄存器地址（写方向） */
    I2C_START();
    I2C_SEND_BYTE(slave_addr << 1); // 写
    if (I2C_GET_ACK())
        goto nack;
    I2C_SEND_BYTE(reg_addr);
    if (I2C_GET_ACK())
        goto nack;

    /* 第 2 步：重复起始 + 读方向 */
    I2C_START();                          // 重复起始
    I2C_SEND_BYTE((slave_addr << 1) | 1); // 读
    if (I2C_GET_ACK())
        goto nack;

    /* 第 3 步：读 1 字节并发送 NACK */
    *p_data = I2C_READ_BYTE(0); // 0=发送 NACK

    I2C_STOP();
    return 0; // 成功

nack:
    I2C_STOP();
    return 1; // 失败
}

/**
 * @brief  向从机连续写多个字节
 * @param  slave_addr : 7位从机地址（调用前右移1位）
 * @param  reg_addr   : 起始寄存器地址
 * @param  p_buf      : 数据缓冲区指针
 * @param  len        : 要写的字节数
 * @retval 0:全部成功  1:中途无应答
 */
uint8_t I2C_WRITE_BATCH(uint8_t slave_addr,
                        uint8_t reg_addr,
                        const uint8_t *p_buf,
                        uint16_t len)
{
    uint16_t i;
    I2C_START();

    /* 发送写地址 */
    I2C_SEND_BYTE(slave_addr << 1);
    if (I2C_GET_ACK())
        goto nack;

    /* 发送寄存器起始地址 */
    I2C_SEND_BYTE(reg_addr);
    if (I2C_GET_ACK())
        goto nack;

    /* 连续写数据 */
    for (i = 0; i < len; i++)
    {
        I2C_SEND_BYTE(p_buf[i]);
        if (I2C_GET_ACK())
            goto nack;
    }

    I2C_STOP();
    return 0; /* 全部成功 */

nack:
    I2C_STOP();
    return 1; /* 中途失败 */
}
/**
 * @brief  从从机连续读多个字节
 * @param  slave_addr : 7 位从机地址（调用前右移 1 位）
 * @param  reg_addr   : 起始寄存器地址（若设备不需要，可填 0）
 * @param  p_buf      : 接收缓冲区指针
 * @param  len        : 要读的字节数
 * @retval 0:全部成功  1:寻址或写寄存器阶段无应答
 */
uint8_t I2C_READ_BATCH(uint8_t slave_addr,
                       uint8_t reg_addr,
                       uint8_t *p_buf,
                       uint16_t len)
{
    uint16_t i;

    /* 第一步：写寄存器地址（需要） */
    I2C_START();
    I2C_SEND_BYTE(slave_addr << 1); // 写方向
    if (I2C_GET_ACK())
        goto nack;
    I2C_SEND_BYTE(reg_addr);
    if (I2C_GET_ACK())
        goto nack;

    /* 第二步：重复起始 + 读方向 */
    I2C_START();                          // 重复起始
    I2C_SEND_BYTE((slave_addr << 1) | 1); // 读方向
    if (I2C_GET_ACK())
        goto nack;

    /* 第三步：连续读数据 */
    for (i = 0; i < len; i++)
    {
        /* 最后一字节发 NACK，其余发 ACK */
        p_buf[i] = I2C_READ_BYTE(i == len - 1 ? 0 : 1);
    }

    I2C_STOP();
    return 0; /* 全部成功 */

nack:
    I2C_STOP();
    return 1; /* 寻址或寄存器阶段失败 */
}
// ------------------------------ USART1 ------------------------------
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
UART_T UART1_STR;
void UART1_INIT(int BAUD_)
{
//    gpio_init_type gpio_init_struct;
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE); // 使能USART2外设时钟
	
	GPIO_INIT(GPIOA, GPIO_PINS_10, GPIO_MODE_MUX, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_UP);		//RX
	GPIO_INIT(GPIOA, GPIO_PINS_9, GPIO_MODE_MUX, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_NONE);	//TX
	gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE10, GPIO_MUX_7);     		// 配置PA10为USART1_RX
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE9, GPIO_MUX_7);     		// 配置PA9为USART1_TX
	
    usart_init(USART1, BAUD_, USART_DATA_8BITS, USART_STOP_1_BIT); 	// 波特率115200，8位数据，1位停止位
    usart_transmitter_enable(USART1, TRUE);                         	// 使能发送
    usart_receiver_enable(USART1, TRUE);                            	// 使能接收
    usart_dma_transmitter_enable(USART1, TRUE);                     	// 使能DMA发送
    usart_dma_receiver_enable(USART1, TRUE);                        	// 使能DMA接收
    usart_interrupt_enable(USART1, USART_IDLE_INT, TRUE);           	// 使能USART2空闲中断
    usart_interrupt_enable(USART1, USART_TDC_INT, TRUE);            	// 使能USART2发送完成中断
    usart_enable(USART1, TRUE);                                     	// 使能USART2
    dma_init_type dma_init_struct;
	
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE); // 使能DMA1外设时钟
    dmamux_enable(DMA1, TRUE);                            // 使能DMA多路复用器
    dma_reset(DMA1_CHANNEL1);                                               // 复位DMA通道5
    dma_default_para_init(&dma_init_struct);                                // 初始化DMA配置为默认值
    dma_init_struct.buffer_size = UART_size;								// 设置缓冲区大小
    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;               // 设置传输方向：内存到外设
    dma_init_struct.memory_base_addr = (uint32_t)UART1_STR.TX_buffer;       // 设置内存基地址
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为8位
    dma_init_struct.memory_inc_enable = TRUE;                               // 使能内存地址自增
    dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;           // 设置外设基地址
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为8位
    dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;                         // 设置DMA优先级为中等
    dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
    dma_init(DMA1_CHANNEL1, &dma_init_struct);                              // 初始化DMA通道5
//    dma_interrupt_enable(DMA1_CHANNEL3, DMA_FDT_INT, TRUE); // 使能DMA传输完成中断
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
//    nvic_irq_enable(DMA1_Channel3_IRQn, 0, 0); // 使能DMA通道3中断
    dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_USART1_TX); // 配置DMA多路复用器通道3为USART1发送
	
    dma_reset(DMA1_CHANNEL2);                                               // 复位DMA通道4
    dma_default_para_init(&dma_init_struct);                                // 初始化DMA配置为默认值
    dma_init_struct.buffer_size = UART_size;                   			 	// 设置缓冲区大小
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;               // 设置传输方向：外设到内存
    dma_init_struct.memory_base_addr = (uint32_t)UART1_STR.RX_buffer;       // 设置内存基地址
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为8位
    dma_init_struct.memory_inc_enable = TRUE;                               // 使能内存地址自增
    dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;           // 设置外设基地址
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为8位
    dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;                         // 设置DMA优先级为中等
    dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
    dma_init(DMA1_CHANNEL2, &dma_init_struct);                              // 初始化DMA通道4
//    dma_interrupt_enable(DMA1_CHANNEL4, DMA_FDT_INT, TRUE); // 使能DMA传输完成中断
//    nvic_irq_enable(DMA1_Channel4_IRQn, 0, 0); // 使能DMA通道4中断
    dmamux_init(DMA1MUX_CHANNEL2, DMAMUX_DMAREQ_ID_USART1_RX); // 配置DMA多路复用器通道4为USART1接收
    nvic_irq_enable(USART1_IRQn, 1, 0);
    dma_channel_enable(DMA1_CHANNEL2, TRUE); /* 启动USART1的DMA接收 */
    dma_channel_enable(DMA1_CHANNEL1, TRUE); /* 启动USART1的DMA发送 */
}
void uart1_sendstring(char *str, u16 len)
{
    if (UART1_STR.TX_complete == 0)
    {
        UART1_STR.TX_complete = 1;
        DMA1_CHANNEL1->ctrl_bit.chen = FALSE;         // 1. 先关闭DMA通道，确保操作安全
        DMA1_CHANNEL1->maddr = (uint32_t)str;         // 2. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL1->paddr = (uint32_t)&USART1->dt; // 3. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL1->dtcnt_bit.cnt = len;           // 4. 重置传输计数器（核心触发条件）
        DMA1_CHANNEL1->ctrl_bit.chen = TRUE;          // 5. 重新使能通道，启动传输
    }
}
void uart1_printf(char *stringg, ...) // 格式化发送
{
	u16 i;
//	static char string[100]; // 一个足够长的字符串
	static va_list arg;
	va_start(arg, stringg);
	vsprintf(UART1_STR.TX_buffer, stringg, arg);
	va_end(arg);
	while(UART1_STR.TX_buffer[i])
		i++;
	uart1_sendstring(UART1_STR.TX_buffer,i);
}
void ChassisCmd_Parse(unsigned char *data_);
void USART1_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART1, USART_IDLEF_FLAG))
    {
        usart_flag_clear(USART1, USART_IDLEF_FLAG);
        // --------------------------- 中断处理内容 ---------------------------
		ChassisCmd_Parse((u8*)UART1_STR.RX_buffer);
        // --------------------------- 中断处理内容 ---------------------------
        DMA1_CHANNEL2->ctrl_bit.chen = FALSE;                 	// 1. 先关闭DMA通道，确保操作安全
        DMA1_CHANNEL2->maddr = (uint32_t)UART1_STR.RX_buffer;	// 2. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL2->paddr = (uint32_t)&USART1->dt;         	// 3. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL2->dtcnt_bit.cnt = UART_size; 				// 4. 重置传输计数器（核心触发条件）
        DMA1_CHANNEL2->ctrl_bit.chen = TRUE;                  	// 5. 重新使能通道，启动传输
    }
    if (usart_interrupt_flag_get(USART1, USART_TDC_FLAG))
    {
        UART1_STR.TX_complete = 0; // 清空发送完成标志
        usart_flag_clear(USART1, USART_TDC_FLAG);
    }
}

// ------------------------------ USART1 ------------------------------
// ------------------------------ USART_2&USART_3 ------------------------------
uint8_t usart2_tx_buffer[USART2_TX_BUFFER_SIZE];
uint8_t usart3_tx_buffer[USART3_TX_BUFFER_SIZE];
uint8_t usart2_rx_buffer[USART3_TX_BUFFER_SIZE];
uint8_t usart3_rx_buffer[USART2_TX_BUFFER_SIZE];
volatile uint8_t usart2_tx_counter = 0x00; // 发送完成
volatile uint8_t usart3_tx_counter = 0x00; // 发送完成
volatile uint8_t usart2_rx_counter = 0x00; // 接收完成
volatile uint8_t usart3_rx_counter = 0x00; // 接收完成
uint8_t usart2_tx_buffer_size = USART2_TX_BUFFER_SIZE;
uint8_t usart3_tx_buffer_size = USART3_TX_BUFFER_SIZE;

void usart2_3_configuration(void)	//使用串口3需要打开注释 包括DMA中断
{
    gpio_init_type gpio_init_struct;
    /* 使能USART2和GPIO时钟 */
    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE); // 使能USART2外设时钟
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);  // 使能GPIOA端口时钟
    /* 使能USART3和GPIO时钟 */
//    crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, TRUE); // 使能USART3外设时钟
//    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);  // 使能GPIOB端口时钟
//    gpio_default_para_init(&gpio_init_struct);
    /* 配置USART2的TX(PA2)和RX(PA3)引脚 */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER; // 设置GPIO驱动强度为较强
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;              // 设置为推挽输出
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;                          // 设置为复用功能模式
    gpio_init_struct.gpio_pins = GPIO_PINS_2 | GPIO_PINS_3;              // 选择PA2和PA3引脚
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;                         // 不设置上下拉
    gpio_init(GPIOA, &gpio_init_struct);                                 // 初始化GPIOA
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_7);           // 配置PA2为USART2_TX
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE3, GPIO_MUX_7);           // 配置PA3为USART2_RX
    /* 配置USART3的TX(PB10)和RX(PB11)引脚 */
//    gpio_init_struct.gpio_pins = GPIO_PINS_10 | GPIO_PINS_11;   // 选择PB10和PB11引脚
//    gpio_init(GPIOB, &gpio_init_struct);                        // 初始化GPIOB
//    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE10, GPIO_MUX_7); // 配置PB10为USART3_TX
//    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE11, GPIO_MUX_7); // 配置PB11为USART3_RX
    /* 配置USART2参数 */
    usart_init(USART2, 921600, USART_DATA_8BITS, USART_STOP_1_BIT); // 波特率115200，8位数据，1位停止位
    usart_transmitter_enable(USART2, TRUE);                         // 使能发送
    usart_receiver_enable(USART2, TRUE);                            // 使能接收
    usart_dma_transmitter_enable(USART2, TRUE);                     // 使能DMA发送
    usart_dma_receiver_enable(USART2, TRUE);                        // 使能DMA接收
    usart_interrupt_enable(USART2, USART_IDLE_INT, TRUE);           // 使能USART2空闲中断
    usart_interrupt_enable(USART2, USART_TDC_INT, TRUE);            // 使能USART2发送完成中断
    usart_enable(USART2, TRUE);                                     // 使能USART2
    /* 配置USART3参数 */
//    usart_init(USART3, 115200, USART_DATA_8BITS, USART_STOP_1_BIT); // 波特率115200，8位数据，1位停止位
//    usart_transmitter_enable(USART3, TRUE);                         // 使能发送
//    usart_receiver_enable(USART3, TRUE);                            // 使能接收
//    usart_dma_transmitter_enable(USART3, TRUE);                     // 使能DMA发送
//    usart_dma_receiver_enable(USART3, TRUE);                        // 使能DMA接收
//    usart_interrupt_enable(USART3, USART_IDLE_INT, TRUE);           // 使能USART3空闲中断
//    usart_interrupt_enable(USART3, USART_TDC_INT, TRUE);            // 使能USART3发送完成中断
//    usart_enable(USART3, TRUE);                                     // 使能USART3
    dma_init_type dma_init_struct;
    /* 使能DMA1时钟 */
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE); // 使能DMA1外设时钟
    dmamux_enable(DMA1, TRUE);                            // 使能DMA多路复用器
    /* 配置USART2的DMA发送通道 */
    dma_reset(DMA1_CHANNEL5);                                               // 复位DMA通道5
    dma_default_para_init(&dma_init_struct);                                // 初始化DMA配置为默认值
    dma_init_struct.buffer_size = USART2_TX_BUFFER_SIZE;                    // 设置缓冲区大小
    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;               // 设置传输方向：内存到外设
    dma_init_struct.memory_base_addr = (uint32_t)usart2_tx_buffer;          // 设置内存基地址
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为8位
    dma_init_struct.memory_inc_enable = TRUE;                               // 使能内存地址自增
    dma_init_struct.peripheral_base_addr = (uint32_t)&USART2->dt;           // 设置外设基地址
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为8位
    dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;                         // 设置DMA优先级为中等
    dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
    dma_init(DMA1_CHANNEL5, &dma_init_struct);                              // 初始化DMA通道5
    /* 使能传输完成中断 */
    dma_interrupt_enable(DMA1_CHANNEL5, DMA_FDT_INT, TRUE); // 使能DMA传输完成中断
    /* 配置DMA通道5中断 */
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    // 设置中断优先级组
    nvic_irq_enable(DMA1_Channel5_IRQn, 0, 0); // 使能DMA通道5中断
    /* 配置USART2的DMA发送请求 */
    dmamux_init(DMA1MUX_CHANNEL5, DMAMUX_DMAREQ_ID_USART2_TX); // 配置DMA多路复用器通道5为USART2发送
    /* 配置USART2的DMA接收通道 */
    dma_reset(DMA1_CHANNEL6);                                               // 复位DMA通道6
    dma_default_para_init(&dma_init_struct);                                // 初始化DMA配置为默认值
    dma_init_struct.buffer_size = USART3_TX_BUFFER_SIZE;                    // 设置缓冲区大小
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;               // 设置传输方向：外设到内存
    dma_init_struct.memory_base_addr = (uint32_t)usart2_rx_buffer;          // 设置内存基地址
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为8位
    dma_init_struct.memory_inc_enable = TRUE;                               // 使能内存地址自增
    dma_init_struct.peripheral_base_addr = (uint32_t)&USART2->dt;           // 设置外设基地址
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为8位
    dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;                         // 设置DMA优先级为中等
    dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
    dma_init(DMA1_CHANNEL6, &dma_init_struct);                              // 初始化DMA通道6
    /* 使能传输完成中断 */
    dma_interrupt_enable(DMA1_CHANNEL6, DMA_FDT_INT, TRUE); // 使能DMA传输完成中断
    /* 配置DMA通道6中断 */
    nvic_irq_enable(DMA1_Channel6_IRQn, 0, 0); // 使能DMA通道6中断
    /* 配置USART2的DMA接收请求 */
    dmamux_init(DMA1MUX_CHANNEL6, DMAMUX_DMAREQ_ID_USART2_RX); // 配置DMA多路复用器通道6为USART2接收
    /* 配置USART3的DMA发送通道 */
//    dma_reset(DMA1_CHANNEL3);                                               // 复位DMA通道3
//    dma_default_para_init(&dma_init_struct);                                // 初始化DMA配置为默认值
//    dma_init_struct.buffer_size = USART3_TX_BUFFER_SIZE;                    // 设置缓冲区大小
//    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;               // 设置传输方向：内存到外设
//    dma_init_struct.memory_base_addr = (uint32_t)usart3_tx_buffer;          // 设置内存基地址
//    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为8位
//    dma_init_struct.memory_inc_enable = TRUE;                               // 使能内存地址自增
//    dma_init_struct.peripheral_base_addr = (uint32_t)&USART3->dt;           // 设置外设基地址
//    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为8位
//    dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
//    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;                         // 设置DMA优先级为中等
//    dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
//    dma_init(DMA1_CHANNEL3, &dma_init_struct);                              // 初始化DMA通道3
    /* 使能传输完成中断 */
//    dma_interrupt_enable(DMA1_CHANNEL3, DMA_FDT_INT, TRUE); // 使能DMA传输完成中断
    /* 配置DMA通道3中断 */
//    nvic_irq_enable(DMA1_Channel3_IRQn, 0, 0); // 使能DMA通道3中断
    /* 配置USART3的DMA发送请求 */
//    dmamux_init(DMA1MUX_CHANNEL3, DMAMUX_DMAREQ_ID_USART3_TX); // 配置DMA多路复用器通道3为USART3发送
    /* 配置USART3的DMA接收通道 */
//    dma_reset(DMA1_CHANNEL4);                                               // 复位DMA通道4
//    dma_default_para_init(&dma_init_struct);                                // 初始化DMA配置为默认值
//    dma_init_struct.buffer_size = USART2_TX_BUFFER_SIZE;                    // 设置缓冲区大小
//    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;               // 设置传输方向：外设到内存
//    dma_init_struct.memory_base_addr = (uint32_t)usart3_rx_buffer;          // 设置内存基地址
//    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;         // 设置内存数据宽度为8位
//    dma_init_struct.memory_inc_enable = TRUE;                               // 使能内存地址自增
//    dma_init_struct.peripheral_base_addr = (uint32_t)&USART3->dt;           // 设置外设基地址
//    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE; // 设置外设数据宽度为8位
//    dma_init_struct.peripheral_inc_enable = FALSE;                          // 禁止外设地址自增
//    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;                         // 设置DMA优先级为中等
//    dma_init_struct.loop_mode_enable = FALSE;                               // 禁止循环模式
//    dma_init(DMA1_CHANNEL4, &dma_init_struct);                              // 初始化DMA通道4
    /* 使能传输完成中断 */
//    dma_interrupt_enable(DMA1_CHANNEL4, DMA_FDT_INT, TRUE); // 使能DMA传输完成中断
    /* 配置DMA通道4中断 */
//    nvic_irq_enable(DMA1_Channel4_IRQn, 0, 0); // 使能DMA通道4中断
    /* 配置USART3的DMA接收请求 */
//    dmamux_init(DMA1MUX_CHANNEL4, DMAMUX_DMAREQ_ID_USART3_RX); // 配置DMA多路复用器通道4为USART3接收
                                                               // 开启串口中断使能
    nvic_irq_enable(USART2_IRQn, 0, 0);
//    nvic_irq_enable(USART3_IRQn, 0, 0);
    /* 启动所有DMA通道 */
    dma_channel_enable(DMA1_CHANNEL6, TRUE); /* 启动USART2的DMA接收 */
//    dma_channel_enable(DMA1_CHANNEL4, TRUE); /* 启动USART3的DMA接收 */
    dma_channel_enable(DMA1_CHANNEL5, TRUE); /* 启动USART2的DMA发送 */
//    dma_channel_enable(DMA1_CHANNEL3, TRUE); /* 启动USART3的DMA发送 */
}
void uart3_sendstring(u8 *str, u16 len)
{
    if (usart3_tx_counter == 0)
    {
        usart3_tx_counter = 1;
        DMA1_CHANNEL3->ctrl_bit.chen = FALSE;         // 1. 先关闭DMA通道，确保操作安全
        DMA1_CHANNEL3->maddr = (uint32_t)str;         // 2. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL3->paddr = (uint32_t)&USART3->dt; // 3. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL3->dtcnt_bit.cnt = len;           // 4. 重置传输计数器（核心触发条件）
        DMA1_CHANNEL3->ctrl_bit.chen = TRUE;          // 5. 重新使能通道，启动传输
    }
}

void uart3_printf(char *stringg, ...) // 格式化发送
{
	u16 i;
	static char string[100]; // 一个足够长的字符串
	static va_list arg;
	va_start(arg, stringg);
	vsprintf(string, stringg, arg);
	va_end(arg);
	while(string[i])
		i++;
	uart3_sendstring((u8*)string,i);
}
void uart2_sendstring(u8 *str, u16 len)
{
    if (usart2_tx_counter == 0)
    {
        usart2_tx_counter = 1;
        DMA1_CHANNEL5->ctrl_bit.chen = FALSE;         // 1. 先关闭DMA通道，确保操作安全
        DMA1_CHANNEL5->maddr = (uint32_t)str;         // 2. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL5->paddr = (uint32_t)&USART2->dt; // 3. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL5->dtcnt_bit.cnt = len;           // 4. 重置传输计数器（核心触发条件）
        DMA1_CHANNEL5->ctrl_bit.chen = TRUE;          // 5. 重新使能通道，启动传输
    }
}

void uart2_printf(char *stringg, ...) // 格式化发送
{
	u16 i;
	static char string[100]; // 一个足够长的字符串
	static va_list arg;
	va_start(arg, stringg);
	vsprintf(string, stringg, arg);
	va_end(arg);
	while(string[i])
		i++;
	uart2_sendstring((u8*)string,i);
}

//void DMA1_Channel3_IRQHandler(void) // USART3 DMA发送中断
//{
//    if (dma_interrupt_flag_get(DMA1_FDT3_FLAG))
//    {

//        dma_flag_clear(DMA1_FDT3_FLAG);
//        //        dma_channel_enable(DMA1_CHANNEL3, TRUE);
//    }
//}
//void DMA1_Channel4_IRQHandler(void) // USART3 DMA接收中断
//{
//    if (dma_interrupt_flag_get(DMA1_FDT4_FLAG))
//    {
//        dma_flag_clear(DMA1_FDT4_FLAG);
//        //        dma_channel_enable(DMA1_CHANNEL4, TRUE);
//    }
//}
void DMA1_Channel5_IRQHandler(void) // USART2 DMA发送中断
{
    if (dma_interrupt_flag_get(DMA1_FDT5_FLAG))
    {
        dma_flag_clear(DMA1_FDT5_FLAG);
    }
}
void DMA1_Channel6_IRQHandler(void) // USART2 DMA接收中断
{
    if (dma_interrupt_flag_get(DMA1_FDT6_FLAG))
    {
        dma_flag_clear(DMA1_FDT6_FLAG);
    }
}
void USART2_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART2, USART_IDLEF_FLAG))
    {
        usart_flag_clear(USART2, USART_IDLEF_FLAG);
        // --------------------------- 中断处理内容 ---------------------------

        // --------------------------- 中断处理内容 ---------------------------

        DMA1_CHANNEL6->ctrl_bit.chen = FALSE;                 // 1. 先关闭DMA通道，确保操作安全
        DMA1_CHANNEL6->maddr = (uint32_t)usart2_rx_buffer;    // 2. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL6->paddr = (uint32_t)&USART2->dt;         // 3. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL6->dtcnt_bit.cnt = USART2_TX_BUFFER_SIZE; // 4. 重置传输计数器（核心触发条件）
        DMA1_CHANNEL6->ctrl_bit.chen = TRUE;                  // 5. 重新使能通道，启动传输
    }
    if (usart_interrupt_flag_get(USART2, USART_TDC_FLAG))
    {
        usart2_tx_counter = 0; // 清空发送完成标志
        usart_flag_clear(USART2, USART_TDC_FLAG);
    }
}

void USART3_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART3, USART_IDLEF_FLAG))
    {
        usart_flag_clear(USART3, USART_IDLEF_FLAG);
        // --------------------------- 中断处理内容 ---------------------------
		
		
		
        // --------------------------- 中断处理内容 ---------------------------
        DMA1_CHANNEL4->ctrl_bit.chen = FALSE;                 // 1. 先关闭DMA通道，确保操作安全
        DMA1_CHANNEL4->maddr = (uint32_t)usart3_rx_buffer;    // 2. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL4->paddr = (uint32_t)&USART3->dt;         // 3. 设置DMA通道的源地址和目的地址
        DMA1_CHANNEL4->dtcnt_bit.cnt = USART3_TX_BUFFER_SIZE; // 4. 重置传输计数器（核心触发条件）
        DMA1_CHANNEL4->ctrl_bit.chen = TRUE;                  // 5. 重新使能通道，启动传输
    }
    if (usart_interrupt_flag_get(USART3, USART_TDC_FLAG))
    {
        usart3_tx_counter = 0; // 清空发送完成标志
        usart_flag_clear(USART3, USART_TDC_FLAG);
    }
}

// -------------------------- VOFA+JustFloat --------------------------
// -------------------------- 滤波器 --------------------------
void Vofa_Justfloat_send(float *p,u16 sum)
{
    static u8 TX_BUFF[100];
	TX_BUFF[4*sum+2]=0x80;
	TX_BUFF[4*sum+3]=0x7f;
	memcpy(TX_BUFF, (uint8_t *)p, sum*4);		//?ú???????????ъ??????????????????
	uart2_sendstring(TX_BUFF,4*sum+4);      //使用串口3发送数据
}
// -------------------------- VOFA+JustFloat --------------------------
float PID_Control(PID_Handle_t *p, float fdbk)
{
    float err      = p->Ref - fdbk;   // 1. 先算本次
    float err_last = p->err;          // 2. 再拿上次
    p->err         = err;             // 3. 立即更新
    /* 积分 */
    p->Integrator += err * p->Ki * p->PID_Ts;
    if(p->Integrator >  p->Integrator_Limit) p->Integrator =  p->Integrator_Limit;
    if(p->Integrator < -p->Integrator_Limit) p->Integrator = -p->Integrator_Limit;
    /* 微分（对误差） */
    float d = (err - err_last) * p->Kd / p->PID_Ts;
    if(d >  p->D_limit) d =  p->D_limit;
    if(d < -p->D_limit) d = -p->D_limit;

    /* 位置式 PID */
    p->Result = p->Kp * err + p->Integrator + d;

    /* 输出限幅 */
    if(p->Result >  p->OUT_Limit) p->Result =  p->OUT_Limit;
    if(p->Result < -p->OUT_Limit) p->Result = -p->OUT_Limit;

    p->fdbk = fdbk;      // 如果别处要用到测量值
    return p->Result;
}
void LowPassFilter (LPF_Handle_t *LPF , float in)
{
	float temp1,temp2;
	temp1 = 1.0f / (1.0f + LPF->T_s * _2PI * LPF->F);
	temp2 = 1.0f - temp1;
	LPF->Out = temp1 * LPF->Last + temp2 * in;
	LPF->Last = LPF->Out;
}
// -------------------------- 滤波器 --------------------------

// -------------------------- 定时器中断 --------------------------
/**
 * @brief 定时器基础初始化函数
 * 该函数用于配置定时器1的基本参数，包括时钟使能、基础配置、中断设置等
   * @param  tmr_x: 选择要配置的定时器外设。
  *         可选以下值之一:
  *         TMR1, TMR2, TMR3, TMR4, TMR5, TMR6, TMR7, TMR8,
  *         TMR9, TMR10, TMR11, TMR12, TMR13, TMR14, TMR20
  * @param  tmr_pr (16位定时器取值范围:0x0000~0xFFFF,
  *                  32位定时器取值范围:0x0000_0000~0xFFFF_FFFF)
  * @param  tmr_div (定时器分频值:0x0000~0xFFFF)
  
  TIMER_IQR_INIT(TMR1,300,288);	//300us
  TIMER_IQR_INIT(TMR2,1000,2880); //10ms
  TIMER_IQR_INIT(TMR3,1000,2880); //10ms
  TIMER_IQR_INIT(TMR4,1000,2880); //10ms
  TIMER_IQR_INIT(TMR5,1000,2880); //10ms
  TIMER_IQR_INIT(TMR6,1000,2880); //10ms
  TIMER_IQR_INIT(TMR7,1000,2880); //10ms
  TIMER_IQR_INIT(TMR8,1000,2880); //10ms
  
void TMR2_GLOBAL_IRQHandler     (void)
{tmr_flag_clear(TMR2, TMR_OVF_FLAG);}
void TMR3_GLOBAL_IRQHandler     (void)
{tmr_flag_clear(TMR3, TMR_OVF_FLAG);}
void TMR4_GLOBAL_IRQHandler     (void)
{tmr_flag_clear(TMR4, TMR_OVF_FLAG);}
void TMR5_GLOBAL_IRQHandler     (void)
{tmr_flag_clear(TMR5, TMR_OVF_FLAG);}
void TMR6_DAC_GLOBAL_IRQHandler (void)
{tmr_flag_clear(TMR6, TMR_OVF_FLAG);}
void TMR7_GLOBAL_IRQHandler     (void)
{tmr_flag_clear(TMR7, TMR_OVF_FLAG);}
void TMR8_OVF_TMR13_IRQHandler  (void)
{tmr_flag_clear(TMR8, TMR_OVF_FLAG);}

注意:只有TMR1和TMR8是288mhz 其他都是---------------------------144MHZ
 */
void TIMER_IQR_INIT(tmr_type *tmr_xx,uint32_t tmr_prx, uint32_t tmr_divx)
{
    /* 使能TMR1时钟 */
	switch((uint32_t)tmr_xx)
	{
		case TMR1_BASE:crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR1_OVF_TMR10_IRQn	, 3, 3);	break;
		case TMR2_BASE:crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR2_GLOBAL_IRQn		, 3, 3);	break;
		case TMR3_BASE:crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR3_GLOBAL_IRQn		, 3, 3);	break;
		case TMR4_BASE:crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR4_GLOBAL_IRQn		, 3, 3);	break;
		case TMR5_BASE:crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR5_GLOBAL_IRQn		, 3, 3);	break;
		case TMR6_BASE:crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR6_DAC_GLOBAL_IRQn	, 3, 3);	break;
		case TMR7_BASE:crm_periph_clock_enable(CRM_TMR7_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR7_GLOBAL_IRQn		, 3, 3);	break;
		case TMR8_BASE:crm_periph_clock_enable(CRM_TMR8_PERIPH_CLOCK, TRUE); nvic_irq_enable(TMR8_OVF_TMR13_IRQn	, 3, 3);	break;
	}
    /* TMR1配置 */
    /* 时基配置 */
//	if(( (uint32_t)tmr_xx == TMR8_BASE)||(  (uint32_t)tmr_xx == TMR1_BASE))
		tmr_base_init(tmr_xx, tmr_prx - 1, tmr_divx - 1); 
//	else
//		tmr_base_init(tmr_xx, tmr_prx - 1, tmr_divx/2 - 1); 
    tmr_cnt_dir_set(tmr_xx, TMR_COUNT_UP);
    /* 使能溢出中断 */
    tmr_interrupt_enable(tmr_xx, TMR_OVF_INT, TRUE);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    /* 使能TMR1 */
    tmr_counter_enable(tmr_xx, TRUE);
}
// -------------------------- 定时器中断 --------------------------


float MedianFilter_Update(float a,float b,float c) 
{
    // 3. ???????????????? (???????buffer?????????)
    float temp;
    // 4. ?????? (???????м?????????????????ü????????)
    // ??? a <= b <= c
    if (a > b) { temp = a; a = b; b = temp; }  // ??? a > b??????
    if (b > c) { temp = b; b = c; c = temp; } // ??? b > c??????
    if (a > b) { temp = a; a = b; b = temp; } // ??μ?? a ?? b (?????????????С???????b)
    return b;// 5. ?????м?? (b ???????)
}
