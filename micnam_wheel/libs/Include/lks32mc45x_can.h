/**
 * @file
 * @brief        CAN库函数头文件
 * @author       YangZJ
 * @date         2023-01-02
*  */
#ifndef _LKS32MC45X_CAN_H_
#define _LKS32MC45X_CAN_H_

///< Includes ------------------------------------------------------------------
#include "lks32mc45x.h"
#include "basic.h"

typedef struct
{
    __IO u32 RBUF[20]; ///< 0x00 CAN Rx Buffer寄存器
    __IO u32 TBUF[20]; ///< 0x50 CAN Tx Buffer寄存器
    __IO u8 CFG_STAT;  ///< 0xA0 CAN配置和状态寄存器
    __IO u8 TCMD;      ///< 0xA1 CAN发送命令寄存器
    __IO u8 TCTRL;     ///< 0xA2 CAN发送控制寄存器
    __IO u8 RCTRL;     ///< 0xA3 CAN接收控制寄存器
    __IO u8 RTIE;      ///< 0xA4 CAN发送接收中断控制寄存器
    __IO u8 RTIF;      ///< 0xA5 CAN发送接收中断标志寄存器
    __IO u8 ERRINT;    ///< 0xA6 CAN错误中断使能和标志寄存器
    __IO u8 LIMIT;     ///< 0xA7 CAN警告寄存器
    __IO u32 SBAUD;    ///< 0xA8 CAN波特率配置寄存器
    __IO u32 FBAUD;    ///< 0xA8 CAN波特率配置寄存器
    __IO u8 EALCAP;    ///< 0xB0 CAN错误信息和丢失仲裁信息记录寄存器
    __IO u8 TDC;       ///< 0xB1 CAN发送延时补偿寄存器
    __IO u8 RECNT;     ///< 0xB2 CAN接收错误计数器寄存器
    __IO u8 TECNT;     ///< 0xB3 CAN发送错误计数器寄存器
    __IO u8 ACFCTRL;   ///< 0xB4 CAN ID过滤器控制寄存器
    __IO u8 TIMECFG;   ///< 0xB5 CAN IDCiA603时间戳配置寄存器
    __IO u16 ACFEN;    ///< 0xB6 CAN ID过滤器使能寄存器
    __IO u32 ACF;      ///< 0xB8 CAN ID过滤器选择寄存器
    __IO u8 null1;     ///< 0xBC 地址对齐
    __IO u8 null2;     ///< 0xBD 地址对齐
    __IO u8 TBSLOT;    ///< 0xBE TTCAN 发送缓冲区指针
    __IO u8 TTCFG;     ///< 0xBF TTCAN配置寄存器
    __IO u32 REF_MSG;  ///< 0xC0 TTCAN参考帧寄存器
    __IO u16 TRG_CFG;  ///< 0xC4 TTCAN触发配置寄存器
    __IO u16 TT_TRIG;  ///< 0xC6 TTCAN触发时刻寄存器
    __IO u16 TT_WTRIG; ///< 0xC8 TTCAN超时监测寄存器
    __IO u16 null3;    ///< 0xCA TTCAN超时监测寄存器
    __IO u32 null4;    ///< 0xCC TTCAN超时监测寄存器
    __IO u32 CIAWDAT0; ///< 0xD0 CiA603时间戳低32位写入值
    __IO u32 CIAWDAT1; ///< 0xD4 CiA603时间戳高32位写入值
} CAN_TypeDef;

#ifndef CAN
#define CAN ((CAN_TypeDef *)CAN_BASE)
#endif

#define CAN_ERROR_NO 0     ///<  无错误
#define CAN_ERROR_BIT 1    ///<  位错误
#define CAN_ERROR_FORM 2   ///<  形式错误
#define CAN_ERROR_STUFF 3  ///<  填充错误
#define CAN_ERROR_ACK 4    ///<  应答错误
#define CAN_ERROR_CRC 5    ///<  CRC 错误
#define CAN_ERROR_UNKOWN 6 ///<  其他错误

#define CAN_TBSEL_STB 1 ///<  Tx Buffer 选择 STB
#define CAN_TBSEL_PTB 0 ///<  Tx Buffer 选择 PTB

#define CAN_RTIE_RIE BIT7   ///< [7] 接收中断使能（Receive Interrupt Enable）
#define CAN_RTIE_ROIE BIT6  ///< [6] 接收溢出中断使能（Receive Overrun Interrupt Enable）
#define CAN_RTIE_RFIE BIT5  ///< [5] Rx Buffer 满中断使能（Rx Buffer Full Interrupt Enable）
#define CAN_RTIE_RAFIE BIT4 ///< [4] Rx Buffer 将满中断使能（Rx Buffer Almost Full Interrupt Enable）
#define CAN_RTIE_TPIE BIT3  ///< [3] PTB 发送中断使能（Transmission Primary Interrupt Enable）
#define CAN_RTIE_TSIE BIT2  ///< [2] STB 发送中断使能（Transmission Secondary Interrupt Enable）
#define CAN_RTIE_EIE BIT1   ///< [1] 错误中断使能（Error Interrupt Enable）
#define CAN_RTIE_TSFF BIT0  ///< [0] Tx Buffer 标志位    1：STB Slot 被全部填满  0：STB Slot 没有被全部填满

#define CAN_MODE_LOM BIT8   ///< [8] 监听模式使能位
#define CAN_MODE_RESET BIT7 ///< [7] 模块复位模式
#define CAN_MODE_LBME BIT6  ///< [6] 外部回环模式使能位
#define CAN_MODE_LBMI BIT5  ///< [5] 内部回环模式使能位
#define CAN_MODE_TPSS BIT4  ///< [4] PTB 单次传输模式使能位
#define CAN_MODE_TSSS BIT3  ///< [3] STB 单次传输模式使能位

#define CAN_IE_EPIE BIT13 ///< [13] 被动错误中断使能（Error Passive Interrupt Enable） 1：使能0：禁止
#define CAN_IE_ALIE BIT11 ///< [11] 仲裁失败中断使能（Arbitration Lost Interrupt Enable） 1：使能0：禁止
#define CAN_IE_BEIE BIT8  ///< [8 ] 总线错误中断使能（Bus Error Interrupt Enable） 1：使能0：禁止
#define CAN_IE_RIE BIT7   ///< [7 ] 接收中断使能（Receive Interrupt Enable）
#define CAN_IE_ROIE BIT6  ///< [6 ] 接收溢出中断使能（Receive Overrun Interrupt Enable）
#define CAN_IE_RFIE BIT5  ///< [5 ] Rx Buffer 满中断使能（Rx Buffer Full Interrupt Enable）
#define CAN_IE_RAFIE BIT4 ///< [4 ] Rx Buffer 将满中断使能（Rx Buffer Almost Full Interrupt Enable）
#define CAN_IE_TPIE BIT3  ///< [3 ] PTB 发送中断使能（Transmission Primary Interrupt Enable）
#define CAN_IE_TSIE BIT2  ///< [2 ] STB 发送中断使能（Transmission Secondary Interrupt Enable）
#define CAN_IE_EIE BIT1   ///< [1 ] 错误中断使能（Error Interrupt Enable）

#define CAN_IF_EWARN BIT15 ///< [15] 错误计数值超过门限中断标志位1：RECNT 或者 TECNT 大于等于 EWL 设定值，写 1 清零0：RECNT 或者 TECNT 小于 EWL 设定值
#define CAN_IF_EPASS BIT14 ///< [14] CAN 设备处于被动错误状态1：CAN 设备处于被动错误状态0：CAN 设备处于主动错误状态
#define CAN_IF_EPIF BIT12  ///< [12] 被动错误中断标志（Error Passive Interrupt Flag） 1：发生主动错误到被动错误（或者相反）的改变，写 1 清零0：未发生
#define CAN_IF_ALIF BIT10  ///< [10] 仲裁失败中断标志位（Arbitration Lost Interrupt Flag） 1：仲裁失败，写 1 清零    0：仲裁成功
#define CAN_IF_BEIF BIT8   ///< [8 ] 总线错误中断标志（Bus Error Interrupt Flag）1：总线错误，写 1 清零0：无总线错误
#define CAN_IF_RIF BIT7    ///< [7 ] 接收中断标志位（Receive Interrupt Flag） 1：接收到有效帧（数据帧或者远程帧），写 1 清零   0：没有接收到有效帧
#define CAN_IF_ROIF BIT6   ///< [6 ] 接收溢出中断标志位（Receive Overrun Interrupt Flag） 1：Rx Buffer 至少有一帧未读走的数据被覆盖    0：Rx Buffer 无覆盖 溢出时 ROIF 和 RFIF 同时置 1，写 1 清零
#define CAN_IF_RFIF BIT5   ///< [5 ] Rx Buffer 满中断标志位（Rx Buffer Full Interrupt Flag） 1：Rx Buffer 满，写 1 清零    0：Rx Buffer 未满
#define CAN_IF_RAFIF BIT4  ///< [4 ] Rx Buffer 将满中断标志位（Rx Buffer Almost Full Interrupt Flag） 1：被填充的 Rx Buffer Slot 数目大于等于 AFWL 设定值   0：被填充的 Rx Buffer Slot 数目小于 AFWL 设定值
#define CAN_IF_TPIF BIT3   ///< [3 ] PTB 发送中断标志（Transmission Primary Interrupt Flag） 1：触发 PTB 发送，发送成功完成。写 1 清零   0：无 PTB 发送请求，无完成标志
#define CAN_IF_TSIF BIT2   ///< [2 ] STB 发送中断标志（Transmission Secondary Interrupt Flag） 1：触发 STB 发送，发送成功完成。写 1 清零 0：无 STB 发送请求，无完成标志
#define CAN_IF_EIF BIT1    ///< [1 ] 错误中断标志（Error Interrupt Flag） 1：错误计数器的值发生变化，大于或者小于错误警告寄存器的设定值。   写 1 清零  0：无错误计数器的值从小于错误警告寄存器的设定值变为大于设定值，或者从大于设定值变为小于设定值，均会触发中断标志。另外，进入 Bus Off 或从 Bus Off 退出，也会触发。
#define CAN_IF_AIF BIT0    ///< [0 ] 取消发送中断标志（Abort Interrupt Flag） 1：通过 TPA 和 TSA 请求的发送消息被成功取消。写 1 清零0：未取消发送数据  AIF 没有对应的使能寄存器

// 过滤器配置
typedef struct
{
    u32 ID_CODE; ///< 需要过滤的ID地址
    u32 ID_MASK; ///< CAN_FilterMask屏蔽寄存器
    u8 ACFADR;   ///< ID 过滤器地址（Acceptance Filter Address）ACFADR 指向具体的 ID 过滤器器，通过 SELMASK 去区分 ACR 和 AMR。
    u8 IDCMP;    ///< SELMASK=1 时有效，ID AMR（MASK）选择作用范围  1：ID 过滤器只接收标准帧或者扩展帧，由 IDMASK 进一步指定  0：ID 过滤器接收标准帧和扩展帧两种格式
    u8 IDMASK;   ///< IDCMP 位为 1 时，MASK 选择哪种帧格式    1：ID 过滤器仅接收扩展帧格式    0：ID 过滤器仅接收标准帧格式
    u8 null;     ///< 地址对齐
} CAN_FilterTypeDef;

// 配置can发送帧
typedef struct
{
    u32 TBUF_0;
    u32 TBUF_1;
    u32 TBUF_2;
    u32 TBUF_3;
    u32 enBufferSel;
} CAN_TxFrameTypeDef;

// 配置can接收帧
typedef struct
{
    u32 RBUF_0;
    u32 RBUF_1;
    u32 RBUF_2;
    u32 RBUF_3;
} CAN_RxFrameTypeDef;

// CAN模块初始化
typedef struct
{
    u8 RBALL;                   ///< 接收 Rx Buffer 存储通过 ID 滤波的帧 1：存储所有的 CAN 帧，包括错误的帧 0：正常模式，仅存储正确的 CAN 帧
    u8 ROM;                     ///< Rx Buffer 满，溢出控制位
    u8 SACK;                    ///< 自应答
    u8 TSMODE;                  ///< 发送缓冲器 STB 工作模式  0 - FIFO 模式  1 - 优先级模式
    u8 S_PRESC;                 ///< 慢速时间量子（Time Quanta）TQ 预分频设定值（S_Prescaler），主要配置 TQ 的大小。慢速 TQ = system clock period *(S_PRESC+1)
    u8 S_SJW;                   ///< 再同步补偿宽度时间设定（Bit Timing Segment 2）再同步补偿宽度时间=(S_SJW+1)*TQ
    u8 S_SEG_2;                 ///< 慢速 bit 时间段 2 时间单元设定（Bit Timing Segment 2）慢速 bit 时间段 2 时间=(S_SEG_2+1)*TQ
    u8 S_SEG_1;                 ///< 慢速 bit 时间段 1 时间单元设定（Bit Timing Segment 1）慢速 bit 时间段 1 时间=(S_SEG_1+2)*TQ
    u8 F_PRESC;                 ///< 快速时间量子 TQ 预分频率设定值（F_Prescaler），主要配置 TQ 的大小。快速 TQ = system clock period *(F_PRESC+1)
    u8 F_SJW;                   ///< 再同步补偿宽度时间设定（Bit Timing Segment 2）再同步补偿宽度时间=(F_SJW+1)*TQ
    u8 F_SEG_2;                 ///< 快速 bit 时间段 2 时间单元设定（Bit Timing Segment 2）快速 bit 时间段 2 时间=(F_SEG_2+1)*TQ
    u8 F_SEG_1;                 ///< 快速 bit 时间段 1 时间单元设定（Bit Timing Segment 1）快速 bit 时间段 1 时间=(F_SEG_1+2)*TQ
    CAN_FilterTypeDef *pFilter; ///< 过滤器结构体指针
    u8 AFWL;                    ///< Rx Buffer 快满警告配置值AFWL 同 Rx Buffer 实际接收到的消息数量进行比对，实际数量超过AFWL，触发 RAFIF。AFWL=0，无意义，硬件强制配置为 1；AFWL 超过 Rx Buffer 实际容量，无意义，硬件强制配置为 Rx Buffer 实际容量
    u8 EWL;                     ///< 可编程错误警告限值=(EWL+1)×8。可能的极限值：8，16，…128。EWL的值控制 EIF。
    u8 FilterNumber;            ///< 过滤器数量
    u8 null;                    ///< 地址对齐
} CAN_InitTypeDef;



typedef struct
{
	u32 id;
	u8 len;
	u8 ide;
	u8 rtr;
	u8 TX[8];
	u8 RX[8];
	u16 rx_sta;
}can_parameter;

extern can_parameter can_par ;

void CAN_Initl(u8 BTR3,u8 BTR2,u8 BTR1,u8 BTR0);
u8 My_CAN_Send_Msg(u32 id,u8 ide,u8 rtr, u8 *msg,u8 len);
void ID0_Filter(u32 id, u32 MASK, u8 ide);
void ID1_Filter(u32 id, u32 MASK, u8 ide);
void ID2_Filter(u32 id, u32 MASK, u8 ide);
void ID3_Filter(u32 id, u32 MASK, u8 ide);
void ID4_Filter(u32 id, u32 MASK, u8 ide);
void ID5_Filter(u32 id, u32 MASK, u8 ide);
void ID6_Filter(u32 id, u32 MASK, u8 ide);
void ID7_Filter(u32 id, u32 MASK, u8 ide);
void ID8_Filter(u32 id, u32 MASK, u8 ide);
void ID9_Filter(u32 id, u32 MASK, u8 ide);
void ID10_Filter(u32 id, u32 MASK, u8 ide);
void ID11_Filter(u32 id, u32 MASK, u8 ide);
void ID12_Filter(u32 id, u32 MASK, u8 ide);
void ID13_Filter(u32 id, u32 MASK, u8 ide);
void ID14_Filter(u32 id, u32 MASK, u8 ide);
void ID15_Filter(u32 id, u32 MASK, u8 ide);


u8 CAN_Receive_Msg(u32* id,u8* len ,u8* ide,u8* rtr,u8 *buf);

void CAN_Init(CAN_InitTypeDef *this);
void CAN_StructInit(CAN_InitTypeDef *this);
void CAN_ClearIRQFlag(u32 CAN_IF_x);
u8 CAN_GetIRQFlag(u32 CAN_IF_x);
u8 CAN_GetFlag(u32 CAN_IF_x);

void CAN_Enable(void);
void CAN_Disable(void);

void CAN_Reset(void);
void CAN_ModeConfig(u16 CAN_MODE_x, FuncState state);
void CAN_FilterConfig(CAN_FilterTypeDef this[], u8 len);
void CAN_FilterCmd(u8 enFilter, FuncState state);
void CAN_SetFrame(CAN_TxFrameTypeDef *this);
u8 CAN_TransmitCmd(u8 enTxCmd);
u8 CAN_Receive(CAN_RxFrameTypeDef *this);
u8 CAN_ErrorStatusGet(void);
bool CAN_StatusGet(u8 enCanStatus);
u8 CAN_RxErrorCntGet(void);
u8 CAN_TxErrorCntGet(void);
u8 CAN_ArbitrationLostCap(void);

#endif /*_LKS32MC45X_CAN_H_ */
