#ifndef _MYMODE_h_
#define _MYMODE_h_

#include "at32f435_437.h"      // Device header
#include "at32f435_437_conf.h" // Library configuration file
#include "string.h"
#include "i2c_application.h"
/* 信号量类型依赖 FreeRTOS.h，必须在其之前包含（semphr.h 强制要求） */
#include "FreeRTOS.h"
#include "semphr.h"

void clkout_pin_PA8(void);
void GPIO_INIT(gpio_type *gpio_x, uint32_t PIN, gpio_mode_type mode, gpio_output_type type, gpio_pull_type pull);
error_status can_configuration(void);
void can_transmit_data(void);
error_status can2_configuration(void);
void can2_transmit_data(void);
// -------------------------------- IIC1 --------------------------------  DMA1-1.2
#define I2C_size 128
extern uint8_t I2C1_tx_buf[I2C_size];
extern uint8_t I2C1_rx_buf[I2C_size];
extern i2c_handle_type hi2cx;
void I2C1_init(void);
void I2C1_send_data(uint16_t address, uint8_t *pdata, uint16_t size);
void I2C1_read_data(uint16_t address, uint8_t *pdata, uint16_t size);
// -------------------------------- IIC2 --------------------------------  DMA1-3.4
extern uint8_t I2C2_tx_buf[I2C_size];
extern uint8_t I2C2_rx_buf[I2C_size];
extern i2c_handle_type hi2cy;
void I2C2_init(void);
void I2C2_send_data(uint16_t address, uint8_t *pdata, uint16_t size);
void I2C2_read_data(uint16_t address, uint8_t *pdata, uint16_t size);
/* 组合格式（repeated-start）寄存器读写，供 QMI8658/JY61P 等需要"写地址+重复起始读"的 IMU 使用 */
i2c_status_type I2Cx_WriteReg(uint16_t address, uint8_t reg, uint8_t *pdata, uint16_t size);
i2c_status_type I2Cx_ReadReg(uint16_t address, uint8_t reg, uint8_t *pdata, uint16_t size);
i2c_status_type I2Cy_WriteReg(uint16_t address, uint8_t reg, uint8_t *pdata, uint16_t size);
i2c_status_type I2Cy_ReadReg(uint16_t address, uint8_t reg, uint8_t *pdata, uint16_t size);
/* 阻塞等待 I2C 传输完成：运行期用信号量阻塞，初始化期回退轮询 */
void i2c_wait_complete(i2c_handle_type *hi2c, SemaphoreHandle_t xSem);
// -------------------------------- 软件I2C --------------------------------
// 使用软件iic
#include "at32f435_437_board.h"
#define scl GPIOB, GPIO_PINS_10
#define sda GPIOB, GPIO_PINS_11
#define time_delay 1
void I2C_SOFTWARE_INIT(void);
void I2C_START(void);
void I2C_STOP(void);
void I2C_SEND_BYTE(uint8_t data);    // 发送一个字节
uint8_t I2C_READ_BYTE(uint8_t send_ack);    // 读取一个字节
void I2C_ACK(void);
void I2C_NACK(void);
uint8_t I2C_GET_ACK(void);
void I2C_BUS_RECOVER(void);
uint8_t I2C_WRITE(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);  // 写一个数据
uint8_t I2C_READ_REG(uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_data); // 读一个数据
uint8_t I2C_WRITE_BATCH(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *p_buf, uint16_t len); // 写多个数据
uint8_t I2C_READ_BATCH(uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_buf, uint16_t len);        // 读多个数据
// ------------------------------ USART1 ------------------------------		DMA1-1.2
#define UART_size 32
typedef struct
{
	char TX_buffer[UART_size];
	char RX_buffer[UART_size];
	char TX_complete;

} UART_T;
extern UART_T UART1_STR;
void UART1_INIT(int BAUD_);
void uart1_sendstring(char *str, u16 len);
// ------------------------------ USART1 ------------------------------
// -------------------------------- 串口2&3 --------------------------------  DMA1-5.6
#define USART2_TX_BUFFER_SIZE 64
#define USART3_TX_BUFFER_SIZE 64
extern uint8_t usart2_tx_buffer[USART2_TX_BUFFER_SIZE];
extern uint8_t usart3_tx_buffer[USART3_TX_BUFFER_SIZE];
extern uint8_t usart2_rx_buffer[USART3_TX_BUFFER_SIZE];
extern uint8_t usart3_rx_buffer[USART2_TX_BUFFER_SIZE];
void usart2_3_configuration(void);
void uart3_sendstring(u8 *str, u16 len);
void uart2_sendstring(u8 *str, u16 len);
void uart2_printf(char *stringg, ...); // 格式化发送
void uart3_printf(char *stringg, ...); // 格式化发送
void Vofa_Justfloat_send(float *p,u16 sum);	//vofa发送格式
typedef struct
{
    float Kp;   
    float Ki;   
    float Kd;   
    float Ref;  
    float fdbk; 
    float err;  
    float errlast;
    float Integrator;      
    float Integrator_Limit;
    float D_limit;         
    float PID_Ts;          
    float OUT_Limit;       
    float Result;          
	float FeedForward;		
} PID_Handle_t;
typedef struct
{
    volatile float Out;
    float T_s;          
    float F;            
    float Last;         
    float Actual;       
} LPF_Handle_t;
#define _2PI	6.2832f
#define limit(value, min_value, max_value) ((value) < (min_value) ? (min_value) : (value) > (max_value) ? (max_value) : (value))
#include "math.h"
float PID_Control(PID_Handle_t *p, float fdbk);
void LowPassFilter (LPF_Handle_t *LPF , float in);
void TIMER_IQR_INIT(tmr_type *tmr_xx,uint32_t tmr_prx, uint32_t tmr_divx);
float MedianFilter_Update(float a,float b,float c) ;


#endif
