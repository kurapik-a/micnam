/*
 * can_wheel.c - 底盘主控的轮端 CAN 通信层
 *
 * 基于原轮腿机器人主控通信框架改造:
 *   1. 节点从 6 电机(2轮+4关节)精简为 4 麦轮节点, ID 1~4 全挂 CAN1
 *   2. 删除平衡/关节/高度控制相关逻辑
 *   3. 变量命名统一为底盘语义 (Chassis_t chassis / wheel_reg_float)
 *   4. 原文件名 CAN_MOTO.c, 随命名规范整改更名
 *
 * 通信协议(与轮端固件一致):
 *   写寄存器: 数据帧 6 字节 [寄存器地址, float(4B小端), 校验和(前5字节累加)]
 *   读寄存器: 远程帧 → 节点返回 8 字节 [测量速度(float), 测量位置(float)]
 *   节点 ID:  轮端串口命令 StrinW1;2;X 写入 REG_addr 并 NVR 保存
 */
#include "can_wheel.h"
#include "bsp.h"
#include "main.h"
#include "at32f435_437_can.h"
#include "FreeRTOS.h"
#include "task.h"

Chassis_t chassis;

float wheel_reg_float[5][45];	//float 类型保存轮端寄存器镜像, 下标0弃用, 1~4为节点ID
int   wheel_reg_int[5][45];		//寄存器数据,int类型保存

/**
 * @brief 通过CAN1总线发送数据帧
 * @param ID 要发送的消息ID（标准标识符）
 * @param data 指向要发送的数据缓冲区
 * @param len 要发送的数据长度（最大8字节）
 */
void CanBus1_SendDataFrame(u16 ID, u8 *data, u8 len)
{
	can_tx_message_type tx_message_struct;

	tx_message_struct.standard_id = ID;          // 使用传入的标准ID
	tx_message_struct.extended_id = 0;           // 不使用扩展ID
	tx_message_struct.id_type = CAN_ID_STANDARD; // 使用标准ID格式
	tx_message_struct.frame_type = CAN_TFT_DATA; // 设置为数据帧
	tx_message_struct.dlc = len;                 // 设置数据长度
	for (int i = 0; i < len && i < 8; i++)       // 拷贝发送数据到消息结构体（最多8字节）
		tx_message_struct.data[i] = data[i];
	can_message_transmit(CAN1, &tx_message_struct);
}

/**
 * @brief 通过CAN1总线发送远程帧
 * @param ID 要发送的远程帧ID
 * @note 若目标ID有设备,数据将在CAN1接收中断中获取
 */
void CanBus1_SendRemoteFrame(u16 ID)
{
	can_tx_message_type tx_message_struct;

	tx_message_struct.standard_id = ID;
	tx_message_struct.extended_id = 0;
	tx_message_struct.id_type = CAN_ID_STANDARD;
	tx_message_struct.frame_type = CAN_TFT_REMOTE;   // 远程帧,无数据字段
	tx_message_struct.dlc = 0;
	can_message_transmit(CAN1, &tx_message_struct);
}

/**
 * @brief 通过CAN2总线发送数据帧(预留, 4轮场景默认全部挂CAN1)
 */
void CanBus2_SendDataFrame(u16 ID, u8 *data, u8 len)
{
	can_tx_message_type tx_message_struct;

	tx_message_struct.standard_id = ID;
	tx_message_struct.extended_id = 0;
	tx_message_struct.id_type = CAN_ID_STANDARD;
	tx_message_struct.frame_type = CAN_TFT_DATA;
	tx_message_struct.dlc = len;
	for (int i = 0; i < len && i < 8; i++)
		tx_message_struct.data[i] = data[i];
	can_message_transmit(CAN2, &tx_message_struct);
}

/**
 * @brief 通过CAN2总线发送远程帧(预留)
 */
void CanBus2_SendRemoteFrame(u16 ID)
{
	can_tx_message_type tx_message_struct;

	tx_message_struct.standard_id = ID;
	tx_message_struct.extended_id = 0;
	tx_message_struct.id_type = CAN_ID_STANDARD;
	tx_message_struct.frame_type = CAN_TFT_REMOTE;
	tx_message_struct.dlc = 0;
	can_message_transmit(CAN2, &tx_message_struct);
}

/**
 * @brief 写轮端节点寄存器(6字节数据帧)
 * @param CAN_LIN 总线号 1/2
 * @param ID      节点ID
 * @param REG     寄存器地址
 * @param da      写入的float值
 */
void CanWheel_SetReg(u8 CAN_LIN,u16 ID,u32 REG,float da)
{
	u8 dat[6];
	dat[0] = REG;      // 目标寄存器
	*(uint32_t*)(void*)&dat[1]=*(uint32_t*)(void*)&da;	// float复制到CAN数据中
	dat[5] = dat[0] + dat[1] + dat[2] + dat[3] + dat[4]; // 校验和
	if(CAN_LIN==1)
		CanBus1_SendDataFrame(ID, dat, 6);
	else if(CAN_LIN==2)
		CanBus2_SendDataFrame(ID, dat, 6);
}

/**
 * @brief 请求读取轮端节点寄存器(发送远程帧, 数据在接收中断中更新到寄存器镜像)
 */
void CanWheel_GetReg(u8 CAN_LIN,u16 ID,u32 REG)
{
	u8 dat[6];
	dat[0] = REG;      // 目标寄存器
	if(CAN_LIN==1)
		CanBus1_SendDataFrame(ID, dat, 1);
	else if(CAN_LIN==2)
		CanBus2_SendDataFrame(ID, dat, 1);
}

/**
 * @brief 将8字节回读帧(测量速度+测量位置)更新到节点镜像与心跳
 * @param id  节点ID(1~4)
 * @param dat 8字节数据
 */
static void Wheel_Rsp_Update(u16 id, u8 *dat)
{
	if (id < 1 || id > WHEEL_NUM)
		return;

	*(uint32_t *)&wheel_reg_float[id][REG_Actual_redS] = *(uint32_t *)&dat[0];
	*(uint32_t *)&wheel_reg_float[id][REG_Actual_POS]  = *(uint32_t *)&dat[4];

	chassis.wheel_speed_meas[id - 1] = wheel_reg_float[id][REG_Actual_redS];
	chassis.node_rsp_tick[id - 1] = xTaskGetTickCount();	//刷新节点心跳
	chassis.node_alive[id - 1] = 1;
}

/**
 * @brief CAN1 RX0接收中断服务函数
 * @note 处理两种帧: 6字节寄存器回读 / 8字节速度位置回读
 */
can_rx_message_type CAN1_rx_message_struct; // 定义CAN接收消息结构体变量
void CAN1_RX0_IRQHandler(void)
{
	if (can_interrupt_flag_get(CAN1, CAN_RF0MN_FLAG) != RESET)
	{
		can_message_receive(CAN1, CAN_RX_FIFO0, &CAN1_rx_message_struct);

		if (CAN1_rx_message_struct.frame_type == CAN_TFT_DATA && CAN1_rx_message_struct.dlc == 6)
		{
			// 单寄存器回读: 第0字节是寄存器地址,第1-4字节是数据,第5字节是校验和
			static u8 sum_can1;
			sum_can1 = CAN1_rx_message_struct.data[0]
					+ CAN1_rx_message_struct.data[1]
					+ CAN1_rx_message_struct.data[2]
					+ CAN1_rx_message_struct.data[3]
					+ CAN1_rx_message_struct.data[4];
			if (sum_can1 == CAN1_rx_message_struct.data[5]) // 校验和正确
			{
				*(uint32_t *)&wheel_reg_float[CAN1_rx_message_struct.standard_id][CAN1_rx_message_struct.data[0]] = *(uint32_t *)&CAN1_rx_message_struct.data[1];
				*(uint32_t *)&wheel_reg_int[CAN1_rx_message_struct.standard_id][CAN1_rx_message_struct.data[0]]  = *(uint32_t *)&CAN1_rx_message_struct.data[1];
			}
		}
		else if (CAN1_rx_message_struct.frame_type == CAN_TFT_DATA && CAN1_rx_message_struct.dlc == 8)
		{
			// 远程帧请求后的定长回读: [测量速度(float), 测量位置(float)]
			Wheel_Rsp_Update(CAN1_rx_message_struct.standard_id, CAN1_rx_message_struct.data);
		}
	}
}

/**
 * @brief CAN2 RX0接收中断服务函数(预留, 4轮场景默认全部挂CAN1)
 */
can_rx_message_type CAN2_rx_message_struct;
void CAN2_RX0_IRQHandler(void)
{
	if (can_interrupt_flag_get(CAN2, CAN_RF0MN_FLAG) != RESET)
	{
		can_message_receive(CAN2, CAN_RX_FIFO0, &CAN2_rx_message_struct);

		if (CAN2_rx_message_struct.frame_type == CAN_TFT_DATA && CAN2_rx_message_struct.dlc == 6)
		{
			static u8 sum_can2;
			sum_can2 = CAN2_rx_message_struct.data[0]
					+ CAN2_rx_message_struct.data[1]
					+ CAN2_rx_message_struct.data[2]
					+ CAN2_rx_message_struct.data[3]
					+ CAN2_rx_message_struct.data[4];
			if (sum_can2 == CAN2_rx_message_struct.data[5])
			{
				*(uint32_t *)&wheel_reg_float[CAN2_rx_message_struct.standard_id][CAN2_rx_message_struct.data[0]] = *(uint32_t *)&CAN2_rx_message_struct.data[1];
				*(uint32_t *)&wheel_reg_int[CAN2_rx_message_struct.standard_id][CAN2_rx_message_struct.data[0]]  = *(uint32_t *)&CAN2_rx_message_struct.data[1];
			}
		}
		else if (CAN2_rx_message_struct.frame_type == CAN_TFT_DATA && CAN2_rx_message_struct.dlc == 8)
		{
			Wheel_Rsp_Update(CAN2_rx_message_struct.standard_id, CAN2_rx_message_struct.data);
		}
	}
}

/**
 * @brief CAN1中断服务处理函数
 * 处理CAN1总线错误中断
 */
void CAN1_SE_IRQHandler(void)
{
	if (can_interrupt_flag_get(CAN1, CAN_ETR_FLAG) != RESET)
	{
		can_flag_clear(CAN1, CAN_ETR_FLAG);
	}
}

/**
 * @brief CAN2中断服务处理函数(预留)
 */
void CAN2_SE_IRQHandler(void)
{
	if (can_interrupt_flag_get(CAN2, CAN_ETR_FLAG) != RESET)
	{
		can_flag_clear(CAN2, CAN_ETR_FLAG);
	}
}

/**
 * @brief 底盘上电初始化: 复位四个轮端节点并配置为速度+电流闭环模式
 * @note 在 start 任务中调用(依赖 vTaskDelay 任务延时)
 */
void constant_init(void)
{
	u8 i;
	u16 wheel_id[4] = {1, 2, 3, 4};

	for (i = 0; i < WHEEL_NUM; i++)	//复位四个轮端节点
	{
		CanWheel_SetReg(1, wheel_id[i], REG_reset, 1);
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	vTaskDelay(pdMS_TO_TICKS(2000));	//等待轮端完成复位

	for (i = 0; i < WHEEL_NUM; i++)	//配置为 速度控制+电流闭环 模式
	{
		CanWheel_SetReg(1, wheel_id[i], REG_MOTO_MODE, WHEEL_MODE_SPEED_CUR);
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	for (i = 0; i < WHEEL_NUM; i++)	//配置轮端通信超时 200ms, 通信丢失自动安全停机
	{
		CanWheel_SetReg(1, wheel_id[i], REG_Overtime, 200);
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	chassis.state = 0;	//上电默认停机, 等待遥控启动指令
}

/**
 * @brief 遥控帧解析(在 USART1 空闲中断中调用, 保持轻量只更新目标与标志)
 * @param data_ DMA接收缓冲区
 *
 * 帧格式(24字节定长, 沿用现有遥控器协议):
 *   [0:4]   float 遥控通道1 (-1000~1000)  → cmd_vx (前后)
 *   [4:8]   float 遥控通道2               → cmd_vy (横移)
 *   [8:12]  float 遥控通道3               → cmd_wz (自转)
 *   [12:16] float 遥控通道4               → 预留(速度档位缩放)
 *   [18:20] 帧尾签名 0x80 0x7F
 *   [20]    组合键: 触发四轮电角度标定
 *   [22]    按键: 0x10启动 0x20停机 0x80复位
 */
void ChassisCmd_Parse(unsigned char *data_)
{
	if (data_[18] == 0x80 && data_[19] == 0x7f)
	{
		float *ch = (float *)data_;	//按float解析前16字节
		float scale = CHASSIS_CMD_SCALE;

		//遥控通道 → 底盘速度指令, 值域 -1000~1000 线性映射到限幅速度
		chassis.cmd_vx = ch[0] * scale * CHASSIS_VX_MAX;
		chassis.cmd_vy = ch[1] * scale * CHASSIS_VY_MAX;
		chassis.cmd_wz = ch[2] * scale * CHASSIS_WZ_MAX;

		if (data_[20] == 0x41)	//组合键: 逐节点下发四轮电角度标定
		{
			chassis.reg_addr = REG_Calibration_A;
			chassis.reg_val = 1;		//写1进入电角度校准模式
			chassis.write_req = 1;	//由 T_Motion 逐节点下发
		}
		if (data_[22] == 0x10)	//启动: 允许运动解算与速度下发
		{
			chassis.state |= CHASSIS_ENABLE;
		}
		if (data_[22] == 0x20)	//停机: 清状态位, 四轮目标归零
		{
			chassis.state = 0;
		}
		if (data_[22] == 0x80)	//主控复位
		{
			NVIC_SystemReset();
		}
	}
	data_[18] = 0;	//清除签名, 防止同一帧重复解析
	data_[19] = 0;
}
