#ifndef _MECANUM_H_
#define _MECANUM_H_

#include "bsp.h"

/* ============================ 底盘机械参数 ============================
 * 单位: 米 / rad/s, 须按实车标定后修改
 */
#define MECANUM_WHEEL_RADIUS    0.050f      // 轮子半径 m
#define MECANUM_HALF_LENGTH     0.100f      // 底盘中心到轮轴的纵向半距 lx m
#define MECANUM_HALF_WIDTH      0.100f      // 底盘中心到轮轴的横向半距 ly m

/* 轮子转向补偿: +1.0f 正转, -1.0f 反转, 逐轮实测标定
 * 用于消除电机安装方向与逆解公式符号的差异
 */
extern float wheel_dir_comp[4];

/* 逆解轮序: 与 main.h 的 WHEEL_IDX_* 对应 [LF, RF, LB, RB]
 * 运动学(XY坐标系: x向前 y向左 wz逆时针为正, 俯视):
 *   w_LF = (vx - vy - (lx+ly)*wz) / r
 *   w_RF = (vx + vy + (lx+ly)*wz) / r
 *   w_LB = (vx + vy - (lx+ly)*wz) / r
 *   w_RB = (vx - vy + (lx+ly)*wz) / r
 */
void Mecanum_Solve(float vx, float vy, float wz, float *wheel_speed);

/* 斜坡限幅: 目标值变化每周期不超过 step, 防止目标突变引起电流冲击
 * 返回限幅后的值
 */
float Ramp_Limit(float target, float current, float step);

#endif
