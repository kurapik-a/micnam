#ifndef _main_h_
#define _main_h_

#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define PI_1 3.141593f
#define PI_2 6.283185f

/* ============================ 轮端节点 ID 表 ============================
 * CAN 总线: 全部挂在 CAN1 (500kbps)
 * 节点 ID: 1~4, 由轮端驱动板串口命令 StrinW1;2;X 写入 REG_addr 并 NVR 保存
 * 宏格式: CAN总线号, 节点ID (对应 CanWheel_SetReg / CanWheel_GetReg 的前两个参数)
 */
#define WHEEL_NUM           4

#define wheel_LF            1,1         /* 左前轮 */
#define wheel_RF            1,2         /* 右前轮 */
#define wheel_LB            1,3         /* 左后轮 */
#define wheel_RB            1,4         /* 右后轮 */

/* 麦轮逆解轮序数组下标, 与 wheel_speed[] 数组对应 */
#define WHEEL_IDX_LF        0
#define WHEEL_IDX_RF        1
#define WHEEL_IDX_LB        2
#define WHEEL_IDX_RB        3

/* ============================ 底盘速度限幅 ============================ */
#define CHASSIS_VX_MAX      1.0f        /* 纵向最大速度 m/s */
#define CHASSIS_VY_MAX      1.0f        /* 横向最大速度 m/s */
#define CHASSIS_WZ_MAX      5.0f        /* 最大自转角速度 rad/s */

/* 遥控通道值域 -1000~1000, 线性映射到限幅速度 */
#define CHASSIS_CMD_SCALE   (1.0f/1000.0f)

/* 节点心跳: 超过该时间(毫秒)未收到某节点回读, 判定该节点失联 */
#define CHASSIS_NODE_TIMEOUT_MS     200

#define debug 1

extern SemaphoreHandle_t xI2C1_Sem;	//信号量
extern SemaphoreHandle_t xI2C2_Sem;	//信号量

#endif
