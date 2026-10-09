#ifndef _CAN_WHEEL_H_
#define _CAN_WHEEL_H_

#include "at32f435_437.h" // Device header
#include "bsp.h"

/* ============================ 轮端寄存器表 ============================
 * 每个轮端节点映射 45 个寄存器, 下标 0 弃用, 节点 ID 1~4
 * wheel_reg_float[节点ID][寄存器地址]
 * 通信协议: 数据帧[寄存器地址, float(4B小端), 校验和(前5字节累加)]
 *           远程帧回读 → 节点返回 8 字节[测量速度, 测量位置]
 */
extern float wheel_reg_float[5][45];	//float 类型保存 电机数据 wheel_reg_float[0][]弃用 1~4维表示节点ID,2维表示寄存器地址
extern int   wheel_reg_int[5][45];		//寄存器数据,int类型保存

/* ============================ 底盘状态结构 ============================
 * 汇集遥控指令、运动解算结果与各轮节点状态
 */
typedef struct
{
	u16  state;				        //底盘状态位: CHASSIS_ENABLE / 0(停机)

	float cmd_vx;			        //遥控目标纵向速度 m/s (遥控值已映射)
	float cmd_vy;			        //遥控目标横向速度 m/s
	float cmd_wz;			        //遥控目标自转角速度 rad/s

	float wheel_speed_target[4];	//麦轮逆解后的四轮目标转速 rad/s [LF,RF,LB,RB]
	float wheel_speed_meas[4];	    //四轮测量转速 rad/s (回读寄存器)

	u32  node_rsp_tick[4];	        //各节点最近一次回读的系统 tick (心跳)
	u8   node_alive[4];	            //节点在线标志 1在线 0失联

	u8   write_req;			        //单次寄存器写请求标志(如四轮标定下发)
	u32  reg_addr;				    //待写寄存器地址
	u16  node_id;			        //待写节点 ID
	u8   can_bus;			        //待写总线号
	float reg_val;				    //待写寄存器的值

} Chassis_t;

extern Chassis_t chassis;

/* ============================ 底盘状态位定义 ============================ */
#define CHASSIS_ENABLE          0x01	//允许运动解算与速度下发
#define CHASSIS_NODE_LOST       0x02	//有节点失联(自动触发保护)

/* ============================ 轮端工作模式码 ============================
 * 写入 REG_MOTO_MODE 的组合值, 与轮端固件 MOTO.state 位对应
 */
#define WHEEL_MODE_IDLE         25472	//0x6380 断开电机
#define WHEEL_MODE_BRAKE        21376	//0x5380 电机短路刹车
#define WHEEL_MODE_CURRENT      17308	//0x439C 电流环控制模式
#define WHEEL_MODE_SPEED_CUR    17340	//0x43BC 速度控制+电流闭环

/* ============================ 轮端故障码位定义 ============================ */
#define err_voltage				0x01	//电压异常
#define err_Overcurrent			0x02	//过流
#define err_Overtemperature		0x04	//过温
#define err_Over_ctrl_time		0x08	//超时
#define err_currentLOOP_Failure	0x10	//电流环失效
#define err_SpeedLOOP_Failure	0x20	//速度环失效

/* ============================ 寄存器地址定义 ============================ */
#define   	REG_SYS_MODE			1		// 模式 写1 进入BOOT升级模式
#define		REG_addr				2		// 地址 设备ID
#define   	REG_Version				3		// 版本
#define   	REG_Output_VoltageD		4		// 输出电压(D轴)
#define   	REG_Output_VoltageQ		5		// 输出电压(Q轴)
#define   	REG_Output_currentD		6		// 目标电流	ID
#define   	REG_Output_currentQ		7		// 目标电流	IQ
#define   	REG_Output_speed		8		// 目标速度
#define   	REG_Output_POS			9		// 目标位置
#define   	REG_Actual_redS			11		// 测量速度
#define   	REG_Actual_POS			12		// 测量位置
#define   	REG_Actual_currentQ		13		// 测量电流		IQ
#define   	REG_Actual_currentD		14		// 测量电流		ID
#define		REG_DIV					15		// 线序
#define		REG_PP					16		// 磁极对数
#define		REG_offsetTheta			17		// 电角度
#define		REG_L					18		// 电机电阻
#define		REG_R					19		// 电机电感
#define		REG_debug				20		// 调试模式	输入'1'为停止发送电流等数据,输入'0'为一直发送电流数据
#define		REG_save				21		// 参数保存寄存器
#define		REG_flash_EE			22		// 恢复出厂设置
#define		REG_reset				23		// 复位寄存器
#define		REG_Overcurrent 		24		// 过流保护参数
#define		REG_ERR					25		// 故障码	REG_ERR_
#define		REG_Overtime			26		// 通信超时时间(ms)最大可以设置2^32(ms)
#define		REG_ERR_hide			27		// 对某个故障码屏蔽
#define		REG_MOTO_MODE           28		// 电机模式	与 MOTO.state 有关
#define		REG_LOOP_DQ_P           29		// 电流环DQ轴P参数
#define		REG_LOOP_DQ_I           30		// 电流环DQ轴I参数
#define		REG_LOOP_SPEED_P		31		// 速度环P参数
#define		REG_LOOP_SPEED_I		32		// 速度环I参数
#define		REG_LOOP_POS_P			33		// 位置环P参数
#define		REG_LOOP_POS_I			34		// 位置环I参数
#define		REG_Calibration_A       35		// 电角度校准模式
#define		REG_Overtemperature		36		// 过温保护参数
#define		REG_DEADZONE_CUR_THR    37		// 死区补偿电流阈值 正值 电流大于这个值的时候使用电流死区补偿,小于这个值将使用电压死区补偿 一般是堵转时电流噪声峰峰值的2-3倍
#define		REG_DEADZONE_VOL_THR    38		// 死区补偿电压阈值 正值 是一个电压值应在死区时间附近
#define		REG_BATTERY_MAX_VOLTAGE	39		// 电池电压最大值	三元锂电池设置为4.1V*串数 	电压低于该值,停止启用刹车电阻
#define		REG_BATTERY_MIN_VOLTAGE	40		// 电池电压最小值	三元锂电池设置为3.7V*串数	电压低于该值,报故障
#define		REG_NONE_41             41
#define		REG_NONE_42             42

/* ============================ 通信函数 ============================ */
void CanBus1_SendDataFrame(u16 ID, u8 *data, u8 len);          // CAN1发送数据帧
void CanBus1_SendRemoteFrame(u16 ID);                          // CAN1发送远程帧
void CanBus2_SendDataFrame(u16 ID, u8 *data, u8 len);          // CAN2发送数据帧
void CanBus2_SendRemoteFrame(u16 ID);                          // CAN2发送远程帧

void CanWheel_SetReg(u8 CAN_LIN,u16 ID,u32 REG,float da);     // 写节点寄存器
void CanWheel_GetReg(u8 CAN_LIN,u16 ID,u32 REG);              // 远程帧请求节点寄存器

void CAN1_RX0_IRQHandler(void); // CAN1接收中断
void CAN2_RX0_IRQHandler(void); // CAN2接收中断
void CAN1_SE_IRQHandler(void);  // CAN1发送中断
void CAN2_SE_IRQHandler(void);  // CAN2发送中断

void constant_init(void);                       // 底盘上电初始化(轮端模式配置)
void ChassisCmd_Parse(unsigned char *data_);    // 遥控帧解析(USART1空闲中断中调用)

#endif
