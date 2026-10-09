/*
 * main.c - CAN 总线分布式 FOC 麦克纳姆轮全向底盘 主控
 *
 * 硬件: AT32F435 + 双 CAN + USART1(蓝牙/遥控接收)
 * 架构: FreeRTOS 多任务
 *   T_CanPoll  (2ms,  优先级3) 轮询 4 个轮端节点, 回读速度/位置, 刷新心跳
 *   T_Motion   (10ms, 优先级3) 遥控目标 -> 斜坡平滑 -> 麦轮逆解 -> 下发四轮目标转速
 *   T_Monitor  (100ms,优先级1) 节点心跳超时保护 + VOFA 状态输出
 *   遥控帧解析在 USART1 空闲中断中完成(ChassisCmd_Parse), 保持中断轻量
 *
 * 数据流: 蓝牙帧(IRQ解析) -> chassis.cmd_* -> 麦轮逆解 -> CAN 写 REG_Output_speed
 *         轮端远程帧回读 -> wheel_reg_float 镜像 + 节点心跳
 */
#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"
#include "main.h"
#include "bsp.h"
#include "can_wheel.h"
#include "mecanum.h"
#include "i2c_application.h"
//#include "oled.h"
#include "QMI8658.H"

#include "stdio.h"
#include "string.h"

float euler[3]; // 欧拉角数组(QMI8658, 预留航向闭环)
float acc[3];   // 加速度数组
float gyro[3];  // 角速度数组
float dat[10];  // VOFA+ 调试发送缓冲

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
SemaphoreHandle_t xI2C1_Sem = NULL;	//信号量
SemaphoreHandle_t xI2C2_Sem = NULL;	//信号量

/* FreeRTOS 错误回调 (仅在异常时触发) */
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for(;;);
}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for(;;);
}

/* ============================ 任务声明 ============================ */
void start(void *pvParameters);
void T_CanPoll(void *pvParameters);
void T_Motion(void *pvParameters);
void T_Monitor(void *pvParameters);

/* 轮端节点 ID 表: 与 main.h 的 wheel_LF/RF/LB/RB 一致 */
static const u16 wheel_id[WHEEL_NUM] = {1, 2, 3, 4};

int main(void)
{
	crm_clocks_freq_type crm_clocks_freq_struct = {0};
    system_clock_config();   // 配置系统时钟，初始化系统时钟频率
    crm_clocks_freq_get(&crm_clocks_freq_struct); // 获取系统时钟
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

    if (can_configuration() == ERROR)	//CAN1 初始化
    {
        while (1)
            ;
    }
    if (can2_configuration() == ERROR)	//CAN2 初始化(预留)
    {
        while (1)
            ;
    }

	//开启多线程:
	if (xTaskCreate(start, "start", 128, NULL, 3, NULL) != pdPASS) { while(1); }
	vTaskStartScheduler();

    while (1)
    {
    }
}

/*============================================================================
 * 任务名称: start
 * 功    能: 初始化外设与轮端节点, 创建工作任务后删除自己
 *============================================================================*/
void start(void *pvParameters)
{
	(void)pvParameters;

	#if(debug)
//		I2C1_init();
//		xI2C1_Sem = xSemaphoreCreateBinary();	//信号量初始化
//		OLED_Init();
//		I2C2_init();
//		xI2C2_Sem = xSemaphoreCreateBinary();	//信号量初始化
//		qmi8658c_init();
	#endif

	UART1_INIT(57600);	//遥控/蓝牙接收串口(24字节定长帧)

	constant_init();	//轮端节点初始化: 复位 + 速度电流模式 + 通信超时

	/* 创建工作任务 */
	if (xTaskCreate(T_CanPoll, "T_CanPoll", 256, NULL, 3, NULL) != pdPASS) { while(1); }
	if (xTaskCreate(T_Motion,  "T_Motion",  512, NULL, 3, NULL) != pdPASS) { while(1); }
	if (xTaskCreate(T_Monitor, "T_Monitor", 256, NULL, 1, NULL) != pdPASS) { while(1); }

	/* 删除自己 */
	vTaskDelete(NULL);
	while (1);
}

/*============================================================================
 * 任务名称: T_CanPoll
 * 功    能: 轮询轮端节点, 远程帧回读测量速度/位置(接收中断刷新心跳)
 * 周    期: 2ms, 单次轮询一个节点, 4 节点全量刷新周期 8ms
 * 优 先 级: 3
 *============================================================================*/
void T_CanPoll(void *pvParameters)
{
	(void)pvParameters;
	static u8 poll_idx = 0;			//轮询游标
	static u8 calib_step = 0;		//标定下发步进

	TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
		if (chassis.write_req)	//有单次寄存器写请求(四轮电角度标定)
		{
			CanWheel_SetReg(1, wheel_id[calib_step], chassis.reg_addr, chassis.reg_val);
			calib_step++;
			if (calib_step >= WHEEL_NUM)	//四个节点全部下发完毕
			{
				calib_step = 0;
				chassis.write_req = 0;
			}
		}
		else	//常规轮询: 逐节点发远程帧回读
		{
			CanWheel_GetReg(1, wheel_id[poll_idx], REG_Actual_redS);
			poll_idx = (poll_idx + 1) % WHEEL_NUM;
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2));
    }
}

/*============================================================================
 * 任务名称: T_Motion
 * 功    能: 遥控目标 -> 斜坡平滑 -> 麦轮逆解 -> 下发四轮目标转速
 * 周    期: 10ms
 * 优 先 级: 3
 *============================================================================*/
#define MOTION_RAMP_STEP    0.05f	//单周期(10ms)四轮目标转速最大变化量 rad/s

void T_Motion(void *pvParameters)
{
	(void)pvParameters;
	u8 i;
	static float wheel_speed_ramp[WHEEL_NUM] = {0};	//四轮斜坡中间状态

	TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
		if (chassis.state & CHASSIS_ENABLE)
		{
			Mecanum_Solve(chassis.cmd_vx, chassis.cmd_vy, chassis.cmd_wz,
			              chassis.wheel_speed_target);
		}
		else	//停机状态: 目标斜坡归零
		{
			for (i = 0; i < WHEEL_NUM; i++)
				chassis.wheel_speed_target[i] = 0.0f;
		}

		for (i = 0; i < WHEEL_NUM; i++)	//斜坡限幅后逐节点下发目标转速
		{
			wheel_speed_ramp[i] = Ramp_Limit(chassis.wheel_speed_target[i],
			                                 wheel_speed_ramp[i], MOTION_RAMP_STEP);
			CanWheel_SetReg(1, wheel_id[i], REG_Output_speed, wheel_speed_ramp[i]);
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}

/*============================================================================
 * 任务名称: T_Monitor
 * 功    能: 节点心跳超时保护 + VOFA 状态输出
 * 周    期: 100ms
 * 优 先 级: 1
 *============================================================================*/
void T_Monitor(void *pvParameters)
{
	(void)pvParameters;
	u8 i;

	TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
		for (i = 0; i < WHEEL_NUM; i++)	//心跳检查: 超时节点置失联并触发底盘停机
		{
			if ((xTaskGetTickCount() - chassis.node_rsp_tick[i])
				> pdMS_TO_TICKS(CHASSIS_NODE_TIMEOUT_MS))
			{
				chassis.node_alive[i] = 0;
				chassis.state &= ~CHASSIS_ENABLE;	//任一节点失联立即停机
				chassis.state |= CHASSIS_NODE_LOST;
			}
			else
			{
				chassis.state &= ~CHASSIS_NODE_LOST;
			}
		}

		//VOFA+ JustFloat 调试输出: 指令速度 / 四轮目标 / 四轮测量
		dat[0] = chassis.cmd_vx;
		dat[1] = chassis.cmd_vy;
		dat[2] = chassis.cmd_wz;
		dat[3] = chassis.wheel_speed_target[0];
		dat[4] = chassis.wheel_speed_target[1];
		dat[5] = chassis.wheel_speed_target[2];
		dat[6] = chassis.wheel_speed_target[3];
		dat[7] = chassis.wheel_speed_meas[0];
		dat[8] = chassis.wheel_speed_meas[1];
		dat[9] = chassis.wheel_speed_meas[2];
		Vofa_Justfloat_send(dat, 10);

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}
