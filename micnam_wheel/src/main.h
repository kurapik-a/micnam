#ifndef __main_h_
#define __main_h_

#include "lks32mc45x.h"                 // Device header
#include "lks32mc45x_adc.h    "
#include "lks32mc45x_afe.h    "
#include "lks32mc45x_aon.h    "
#include "lks32mc45x_can.h    "
#include "lks32mc45x_cmp.h    "
#include "lks32mc45x_cordic.h "
#include "lks32mc45x_crc.h    "
#include "lks32mc45x_dma.h    "
#include "lks32mc45x_flash.h  "
#include "lks32mc45x_fmac.h   "
#include "lks32mc45x_gpio.h   "
#include "lks32mc45x_hall.h   "
#include "lks32mc45x_i2c.h    "
#include "lks32mc45x_iwdg.h   "
#include "lks32mc45x_mcpwm.h  "
#include "lks32mc45x_nvr.h    "
#include "lks32mc45x_qep.h    "
#include "lks32mc45x_spi.h    "
#include "lks32mc45x_sys.h    "
#include "lks32mc45x_trim.h   "
#include "lks32mc45x_uart.h   "
#include "lks32mc45x_utimer.h "
#include "lks32mc45x_wwdg.h   "
#include "stdbool.h"
#include "stdio.h"
#include "stdarg.h"
#include "lks32mc45x_lib.h"
#include "math.h"

#define io24	GPIO2,GPIO_Pin_4
#define io27    GPIO2,GPIO_Pin_7
#define io28	GPIO2,GPIO_Pin_8
#define io410	GPIO4,GPIO_Pin_10
#define io485	GPIO0,GPIO_Pin_4
#define led1	GPIO2,GPIO_Pin_4
#define led2	GPIO3,GPIO_Pin_8
#define led3	GPIO2,GPIO_Pin_8
#define key30	GPIO3,GPIO_Pin_0
#define key215	GPIO2,GPIO_Pin_15

#define tiaoshi 0		//调试宏定义,调试的时候给1 



#endif
