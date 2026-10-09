
#ifndef _JY61P_h_
#define _JY61P_h_

#include "at32f435_437_board.h"
#include "bsp.h"

// 寄存器地址:  这几个常用,其他别管
#define save 0   // 写入0x00保存,写入0xff重启,写入0x01恢复出厂设置
#define AX 52    // 加速度X
#define AY 53    // 加速度Y
#define AZ 54    // 加速度Z
#define GX 55    // 角速度X
#define GY 56    // 角速度Y
#define GZ 57    // 角速度Z
#define HX 58    // 磁场X
#define HY 59    // 磁场Y
#define HZ 60    // 磁场Z
#define Roll 61  // 姿态角X
#define Pitch 62 // 姿态角Y
#define Yaw 63   // 姿态角Z

void JY61P_I2C_read(uint16_t reg, uint16_t *dat);
void get_JY61P(u8 *C, float *G, float *AW, float *A, float *T);


#endif
