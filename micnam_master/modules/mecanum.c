/*
 * mecanum.c - 麦克纳姆轮运动学逆解与指令平滑
 *
 * 将底盘三自由度速度指令 (vx, vy, wz) 实时分解为四轮独立目标转速,
 * 输出经斜坡限幅后由 T_Motion 任务下发至各轮端 CAN 节点的
 * REG_Output_speed 寄存器。
 */
#include "mecanum.h"
#include "main.h"	//WHEEL_IDX_* 轮序下标定义

float wheel_dir_comp[4] = {1.0f, 1.0f, 1.0f, 1.0f};	//轮序 [LF, RF, LB, RB], 实车逐轮标定

/**
 * @brief  麦克纳姆轮运动学逆解
 * @param  vx: 纵向速度 m/s (向前为正)
 * @param  vy: 横向速度 m/s (向左为正)
 * @param  wz: 自转角速度 rad/s (逆时针为正)
 * @param  wheel_speed: 输出四轮目标转速 rad/s, 轮序 [LF, RF, LB, RB]
 */
void Mecanum_Solve(float vx, float vy, float wz, float *wheel_speed)
{
	float k = MECANUM_HALF_LENGTH + MECANUM_HALF_WIDTH;	//(lx+ly)
	float r = MECANUM_WHEEL_RADIUS;

	wheel_speed[WHEEL_IDX_LF] = wheel_dir_comp[0] * (vx - vy - k * wz) / r;
	wheel_speed[WHEEL_IDX_RF] = wheel_dir_comp[1] * (vx + vy + k * wz) / r;
	wheel_speed[WHEEL_IDX_LB] = wheel_dir_comp[2] * (vx + vy - k * wz) / r;
	wheel_speed[WHEEL_IDX_RB] = wheel_dir_comp[3] * (vx - vy + k * wz) / r;
}

/**
 * @brief  斜坡限幅
 * @param  target:  目标值
 * @param  current: 当前值
 * @param  step:    单周期最大变化量 (正值)
 * @return 限幅后的值
 */
float Ramp_Limit(float target, float current, float step)
{
	float diff = target - current;

	if (diff > step)
		return current + step;
	if (diff < -step)
		return current - step;
	return target;
}
