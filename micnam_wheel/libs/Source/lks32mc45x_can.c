/**
 * @file
 * @defgroup can
 * @brief        CAN库函数
 * @author       YangZJ
 * @date         2023-01-02
 * @{
 */
#include "lks32mc45x_can.h"
#include "lks32mc45x_sys.h"
#include "string.h"


can_parameter can_par =
{
	0x00,
	0x00,
	0x00,
	0x00,
	{0,0},
	{0,0},
	0x00,
};

/*******************************************************************************
函数名称: void CAN_Initl(u8 BTR3,u8 BTR2,u8 BTR1,u8 BTR0)
功能描述: CAN初始化函数,采用2.0B协议
操作的表:
波特率      AN_SBAUD.PRESC[7:0] CAN_SBAUD.SJW[6:0] CAN_SBAUD.SEG2[6:0] CAN_SBAUD.SEG1[7:0]               
	1000Kbps            0x0B            0x02               0x05               0x08               
	800Kbps             0x0B            0x02               0x06               0x0B                
	500Kbps             0x17            0x02               0x05               0x08                
	400Kbps             0x17            0x02               0x06               0x0B                
	250Kbps             0x2F            0x02               0x05               0x08                
	200Kbps             0x2F            0x02               0x06               0x0B                
	160Kbps             0x35            0x02               0x06               0x0B                
	80Kbps              0x77            0x02               0x06               0x0B                
	40Kbps              0xEF            0x02               0x06               0x0B                
	20Kbps              0xEF            0x06               0x0D               0x18                
输入参数: BTR0    S_SEG_1:段时间段 1 时间单元设定
          BTR1    S_SEG_2：段时间段 2 时间单元设定
          BTR2    S_SJW ： 再同步补偿宽度时间设定
          BTR3    S_PRESC ：TQ 预分频率设定值
输出参数: 无
返回值:   无
其它说明:  
修改日期     版本号     修改人    修改内容
-----------------------------------------------------------------------------
2023/02/13   V1.0     HuangMG     创建
*******************************************************************************/
/**
 * @brief    CAN初始化函数,采用2.0B协议
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void CAN_Initl(u8 BTR3,u8 BTR2,u8 BTR1,u8 BTR0)
{
		SYS_WR_PROTECT = 0x7a83;         // 关闭写保护 
	
	  SYS_SFT_RST |= BIT29;            // 复位CAN
	  SYS_SFT_RST &= ~BIT29;           // 释放CAN复位
	
	  SYS_CLK_FEN |= BIT29;
	  
		CAN_CFG_STAT |= 1 << 7;          // 复位模式
	
	  CAN_ACFEN = 0x00;                //关闭ID滤波功能
    CAN_SBAUD    = BTR0|(BTR1<<8)|(BTR2<<16)|(BTR3<<24);//波特率设置
	  CAN_CFG_STAT &= ~(1 << 7);       // 正常输出模式
	  CAN_TCMD    = 0x00;
  	CAN_RTIE |= 1 << 7;              // RFIFO有新的数据被接收到，中断源使能
	  CAN_RTIE |= 1 << 1;					     // 错误中断使能
	  SYS_WR_PROTECT = 0x0;            // 打开写保护 
}


/**
 * @brief    CAN ID15接收滤波函数,采用2.0B协议
 * 输入参数: id: 发送id ide: 标准/扩展帧 rtr：远程帧/数据帧 *msg: 发送的数据  len:发送字节长度
 * 输出参数: 无
 * 返回值:   0：发送成功 !0：发送失败
 * 其它说明: id:标准ID(11位)/扩展ID(11位+18位)	    
 *           ide:0,标准帧;1,扩展帧
 *           rtr:0,数据帧;1,遥控帧
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
u8 My_CAN_Send_Msg(u32 id,u8 ide,u8 rtr, u8 *msg,u8 len)
{
	u8 frame_inf = 0 ;
	u16 i = 0;
	CAN_CFG_STAT &= ~(1 << 7);       // 正常输出模式
	if(!(CAN_CFG_STAT & BIT1))       // 本CAN设备，发送空闲
  {
		frame_inf |= ide << 7;          //0标准/1扩展帧
		frame_inf |= rtr << 6;          //0,数据帧;1,遥控帧
		frame_inf |= len << 0;          //发送数据长度
		CAN_TBUF_01 = frame_inf;          //发送TX帧信息
		if(0 == ide)//标准帧
		{
			id &= 0x7ff;		
			//发送接收寄存器1
			CAN_TBUF_00 = id;          //TX ID
			CAN_TBUF_02 =  msg[0]|msg[1]<<8|msg[2]<<16|msg[3]<<24;          //TX DATA0123
      CAN_TBUF_03 =  msg[4]|msg[5]<<8|msg[6]<<16|msg[7]<<24;          //TX DATA4567
			
		}else//扩展帧
		{
			id &= 0X1FFFFFFF;
			//发送接收寄存器1
			CAN_TBUF_00 = id;          //TX ID
			CAN_TBUF_02 =  msg[0]|msg[1]<<8|msg[2]<<16|msg[3]<<24;          //TX DATA0123
      CAN_TBUF_03 =  msg[4]|msg[5]<<8|msg[6]<<16|msg[7]<<24;          //TX DATA4567
		}
		CAN_TCMD |= 1 << 4;         //CAN 发送传输请求
		while(CAN_CFG_STAT & BIT1)        //最近一次是否传输完成
		{
			if((i++) >= 0xfff) 
			{
				return 0xff;             //发送失败
			}
		};
		   return 0;                   //发送完成
	}
	else //TFIFO非空
	{
		   return 0xff;               //发送失败
	}
}


/*******************************************************************************
 函数名称：    void SoftDelay(void)
 功能描述：    软件延时函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          创建
 *******************************************************************************/
static void Delay(u32 cnt)
{
    volatile u32 t_cnt;

    for(t_cnt = 0; t_cnt < cnt; t_cnt++)
    {
        __NOP();
    }
}

/**
 * @brief    CAN ID0接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID0_Filter(u32 id, u32 MASK, u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式 
	 
	  CAN_ACFEN   |= BIT0;       //ACF0 is Enabled		  
    CAN_ACFCTRL  = 0x00000020; //SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x00;      //ID滤波器地址 指向 ACF_0
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF0 ID Enabled
    Delay(100);
}
/**
 * @brief    CAN ID1接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID1_Filter(u32 id, u32 MASK, u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT1;//ACF1 is Enabled		  
    CAN_ACFCTRL  = 0x00000021;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x01;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF0 ID Enabled
    Delay(100);
}
/**
 * @brief    CAN ID2接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */  
void ID2_Filter(u32 id , u32 MASK , u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式
	 
	  CAN_ACFEN   |= BIT2;//ACF2 is Enabled		  
    CAN_ACFCTRL  = 0x00000022;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x02;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF2 ID Enabled
    Delay(100);
}  

/**
 * @brief    CAN ID3接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID3_Filter(u32 id , u32 MASK , u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式
	 
	  CAN_ACFEN   |= BIT3;//ACF3 is Enabled		  
    CAN_ACFCTRL  = 0x00000023;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x03;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF3 ID Enabled
    Delay(100);
}  

/**
 * @brief    CAN ID4接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID4_Filter(u32 id, u32 MASK, u8 ide)
{
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式
	
	  CAN_ACFEN   |= BIT4;//ACF4 is Enabled		  
    CAN_ACFCTRL  = 0x00000023;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x04;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF4 ID Enabled
    Delay(100);
}  

/**
 * @brief    CAN ID5接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID5_Filter(u32 id, u32 MASK, u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式
	
	  CAN_ACFEN   |= BIT5;//ACF5 is Enabled		  
    CAN_ACFCTRL  = 0x00000025;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x05;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF5 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID6接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID6_Filter(u32 id, u32 MASK, u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式
	 
	  CAN_ACFEN   |= BIT6;//ACF6 is Enabled		  
    CAN_ACFCTRL  = 0x00000026;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x06;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF6 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID7接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID7_Filter(u32 id , u32 MASK, u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT7;//ACF7 is Enabled		  
    CAN_ACFCTRL  = 0x00000027;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x07;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF7 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID8接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID8_Filter(u32 id , u32 MASK , u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT8;//ACF8 is Enabled		  
    CAN_ACFCTRL  = 0x00000028;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x08;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF8 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID9接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID9_Filter(u32 id, u32 MASK, u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT9;//ACF9 is Enabled		  
    CAN_ACFCTRL  = 0x00000029;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x09;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF9 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID10接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID10_Filter(u32 id , u32 MASK, u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT10;//ACF10 is Enabled		  
    CAN_ACFCTRL  = 0x0000002A;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x0A;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF10 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID11接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID11_Filter(u32 id , u32 MASK , u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT11;//ACF11 is Enabled		  
    CAN_ACFCTRL  = 0x0000002B;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x0B;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF11 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID12接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID12_Filter(u32 id , u32 MASK , u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT12;//ACF12 is Enabled		  
    CAN_ACFCTRL  = 0x0000002C;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x0C;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF12 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID13接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID13_Filter(u32 id , u32 MASK , u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT13;//ACF13 is Enabled		  
    CAN_ACFCTRL  = 0x0000002D;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x0D;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF13 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN ID14接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID14_Filter(u32 id , u32 MASK , u8 ide)
{ 
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT14;//ACF14 is Enabled		  
    CAN_ACFCTRL  = 0x0000002E;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x0E;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF14 ID Enabled
    Delay(100);
}


/**
 * @brief    CAN ID15接收滤波函数,采用2.0B协议
 * 输入参数:  id: ID值。
 *            MASK：屏蔽位
 *            ide：0标准/1扩展帧 为0 11位ID长度 为1 29位ID长度，
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
void ID15_Filter(u32 id , u32 MASK , u8 ide)
{   
	  CAN_CFG_STAT |= 1 << 7;          // 复位模式

	  CAN_ACFEN   |= BIT15;//ACF15 is Enabled		  
    CAN_ACFCTRL  = 0x0000002F;//SELMASK==1 MASK
	  CAN_ACF |= BIT30; 
		if(ide == 0)
		 {
			 CAN_ACF &= ~BIT29;//接收标准帧
		 }else{
			 CAN_ACF |= BIT29; //接收扩展帧
		 }
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (MASK & 0x1FFFFFFF);//ACF0 Mask Disabled, All bits should be compared 
	  CAN_ACFCTRL = 0x0F;//SELMASK==0 ID
		CAN_ACF &= ~0x1FFFFFFF;
    CAN_ACF |= (id & 0x1FFFFFFF);//ACF15 ID Enabled
    Delay(100);
}

/**
 * @brief    CAN轮询接收数据程序
 * 输入参数: id: 发送id len:接收数据长度 ide: 标准/扩展帧 rtr：数据帧/遥控帧 *msg: 发送的数据  len:发送字节长度
 * 返回值:   0：接收成功 !0：接收失败
 * @par 更新记录  V1.0  2023/02/13  HuangMG  创建
 */
u8 CAN_Receive_Msg(u32* id,u8* len ,u8* ide,u8* rtr,u8 *buf)
{
	  u8 reg_inf;
	  u8 tmep = 0;
	  if(CAN_RCTRL != 0x00)/*Rx Buffer存有一帧或多帧数据，可以通过 RFIFO 寄存器读取*/
		{
	     reg_inf = CAN_RBUF_01;
			 if(reg_inf & BIT6)
			 {
			    *rtr = 1;
			 }else{
				  *rtr = 0;
			 }
			 *len = reg_inf & 0xf;
			 if(reg_inf & BIT7)/*扩展帧*/
			 {
			    *ide = 1;
				  *id = CAN_RBUF_00 & 0x1FFFFFFF;  
				 
			 }else{             /*标准帧*/
				  *ide = 0;
				  *id = CAN_RBUF_00 & 0x7ff;			 
			 }
			 	  buf[0] = CAN_RBUF_02;
				  buf[1] = CAN_RBUF_02>>8;
				  buf[2] = CAN_RBUF_02>>16;
				  buf[3] = CAN_RBUF_02>>24;
				  buf[4] = CAN_RBUF_03;
				  buf[5] = CAN_RBUF_03>>8;
				  buf[6] = CAN_RBUF_03>>16;
				  buf[7] = CAN_RBUF_03>>24;	
			    for(tmep = *len ; tmep < 8 ;tmep++)
			    {
			      buf[tmep] = 0;
			    }
			    CAN_RCTRL |= BIT4; /*释放 Rx Buffer Slot*/
					return 0;
		}
	   return 1 ;
}



/**
 * @brief    CAN模块初始化
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_Init(CAN_InitTypeDef *this)
{
    u8 i;
    CAN->CFG_STAT |= BIT7;
    CAN->SBAUD = (u32)(this->S_PRESC << 24) |
                 (u32)(this->S_SJW << 16) |
                 (u32)(this->S_SEG_2 << 8) |
                 (u32)(this->S_SEG_1 << 0);
    CAN->FBAUD = (u32)(this->F_PRESC << 24) |
                 (u32)(this->F_SJW << 16) |
                 (u32)(this->F_SEG_2 << 8) |
                 (u32)(this->F_SEG_1 << 0);
    if (this->TSMODE)
    {
        CAN->TCTRL |= BIT5;
    }
    else
    {
        CAN->TCTRL &= ~BIT5;
    }
    CAN_FilterConfig(this->pFilter, this->FilterNumber);

    do
    {
        CAN->CFG_STAT &= ~BIT7;
    } while (CAN->CFG_STAT & BIT7);
    CAN->RCTRL = (u8)(this->RBALL << 3) |
                 (u8)(this->ROM << 6) |
                 (u8)(this->SACK << 7);
    CAN->LIMIT = (u8)(this->AFWL << 4) |
                 (u8)(this->EWL);

    for (i = 0u; i < this->FilterNumber; i++)
    {
        CAN_FilterCmd(this->pFilter[i].ACFADR, ENABLE);
    }
}

/**
 * @brief    CAN配置结构体初始化
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_StructInit(CAN_InitTypeDef *this)
{
    memset(this, 0, sizeof(CAN_InitTypeDef));
}

/**
 * @brief
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_ClearIRQFlag(u32 CAN_IF_x)
{
    CAN->RTIF = CAN_IF_x & 0xff;
    CAN->ERRINT = (CAN_IF_x >> 8) & 0xff;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_GetIRQFlag(u32 CAN_IF_x)
{
    u8 val = 0;

    if (CAN_IF_x & CAN_IF_EWARN)
    {
        if (CAN_GetFlag(CAN_IF_EWARN))
        {
            val = 1;
        }
    }
    if (CAN_IF_x & CAN_IF_EPASS)
    {
        if (CAN_GetFlag(CAN_IF_EPASS))
        {
            val = 1;
        }
    }
    if (CAN_IF_x & CAN_IF_EPIF)
    {
        if (CAN->ERRINT & ((CAN_IE_EPIE >> 8) & 0xff))
        {
            if (CAN_GetFlag(CAN_IF_EPIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_ALIF)
    {
        if (CAN->ERRINT & ((CAN_IE_ALIE >> 8) & 0xff))
        {
            if (CAN_GetFlag(CAN_IF_ALIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_BEIF)
    {
        if (CAN->ERRINT & ((CAN_IE_BEIE >> 8) & 0xff))
        {
            if (CAN_GetFlag(CAN_IF_BEIF))
            {
                val = 1;
            }
        }
    }

    if (CAN_IF_x & CAN_IF_RIF)
    {
        if (CAN->RTIE & CAN_IE_RIE)
        {
            if (CAN_GetFlag(CAN_IF_RIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_ROIF)
    {
        if (CAN->RTIE & CAN_IE_ROIE)
        {
            if (CAN_GetFlag(CAN_IF_ROIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_RFIF)
    {
        if (CAN->RTIE & CAN_IE_RFIE)
        {
            if (CAN_GetFlag(CAN_IF_RFIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_RAFIF)
    {
        if (CAN->RTIE & CAN_IE_RAFIE)
        {
            if (CAN_GetFlag(CAN_IF_RAFIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_TPIF)
    {
        if (CAN->RTIE & CAN_IE_TPIE)
        {
            if (CAN_GetFlag(CAN_IF_TPIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_TSIF)
    {
        if (CAN->RTIE & CAN_IE_TSIE)
        {
            if (CAN_GetFlag(CAN_IF_TSIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_EIF)
    {
        if (CAN->RTIE & CAN_IE_EIE)
        {
            if (CAN_GetFlag(CAN_IF_EIF))
            {
                val = 1;
            }
        }
    }
    if (CAN_IF_x & CAN_IF_AIF)
    {
        if (CAN_GetFlag(CAN_IF_AIF))
        {
            val = 1;
        }
    }
    return val;
}
/**
 * @brief
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_GetFlag(u32 CAN_IF_x)
{
    u8 val = 0;
    if (CAN->RTIF & (CAN_IF_x & 0xff))
    {
        val = 1;
    }
    if (CAN->ERRINT & ((CAN_IF_x >> 8) & 0xff))
    {
        val = 1;
    }
    return val;
}

/**
 * @brief    打开CAN模块时钟
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_Enable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_CAN, ENABLE);
}
/**
 * @brief    关闭CAN模块时钟
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_Disable(void)
{
    SYS_ModuleClockCmd(SYS_MODULE_CAN, DISABLE);
}
/**
 * @brief    CAN模块进入复位模式
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_Reset(void)
{
    CAN->CFG_STAT |= BIT7;
}

/**
 * @brief    CAN模式使能
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_ModeConfig(u16 CAN_MODE_x, FuncState state)
{
    if (CAN_MODE_x & BIT8)
    {
        if (state == ENABLE)
        {
            CAN->TCMD |= BIT6;
        }
        else
        {
            CAN->TCMD &= ~BIT6;
        }
    }
    CAN_MODE_x = CAN_MODE_x & 0xFF;
    if (state == ENABLE)
    {
        CAN->CFG_STAT |= CAN_MODE_x;
    }
    else
    {
        CAN->CFG_STAT &= ~CAN_MODE_x;
    }
}

/**
 * @brief    CAN接收过滤器配置
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_FilterConfig(CAN_FilterTypeDef this[], u8 len)
{
    u8 i;

    for (i = 0u; i < len; i++)
    {
        CAN->ACFCTRL = this[i].ACFADR;
        CAN->ACF = this[i].ID_CODE;
        CAN->ACFCTRL |= BIT5;
        CAN->ACF = (u32)(this[i].ID_MASK) |
                   (u32)(this[i].IDMASK << 29) |
                   (u32)(this[i].IDCMP << 30);
    }
}

/**
 * @brief    CAN接收过滤器使能
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_FilterCmd(u8 enFilter, FuncState state)
{
    u8 u8FilterSel;

    u8FilterSel = (u8)(1ul << enFilter);

    if (state == ENABLE)
    {
        CAN->ACFEN |= u8FilterSel;
    }
    else
    {
        CAN->ACFEN &= (u8)(~u8FilterSel);
    }
}

/**
 * @brief    can传输帧配置
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
void CAN_SetFrame(CAN_TxFrameTypeDef *this)
{

    if (NULL != this)
    {
        if (this->enBufferSel)
        {
            CAN->TCMD |= BIT7;
        }
        else
        {
            CAN->TCMD &= ~BIT7;
        }
        CAN->TBUF[0] = this->TBUF_0;
        CAN->TBUF[1] = this->TBUF_1;
        CAN->TBUF[2] = this->TBUF_2;
        CAN->TBUF[3] = this->TBUF_3;

        if (this->enBufferSel == CAN_TBSEL_STB)
        {
            CAN->TCTRL |= BIT6;
        }
    }
}

/**
 * @brief    CAN Tx命令配置
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_TransmitCmd(u8 enTxCmd)
{
    CAN->TCMD |= enTxCmd;

    return (CAN->TCTRL & 3);
}

/**
 * @brief    can接收帧配置
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_Receive(CAN_RxFrameTypeDef *this)
{

    if (NULL != this)
    {
        this->RBUF_0 = CAN->RBUF[0];
        this->RBUF_1 = CAN->RBUF[1];
        this->RBUF_2 = CAN->RBUF[2];
        this->RBUF_3 = CAN->RBUF[3];

        CAN->RCTRL |= BIT4;
    }
    return (CAN->RCTRL & 3);
}
/**
 * @brief    获取can错误状态
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_ErrorStatusGet(void)
{
    u8 enRet = 0;

    if (6u > (CAN->EALCAP >> 5))
    {
        enRet = (u8)(CAN->EALCAP >> 5);
    }
    return enRet;
}

/**
 * @brief    获取can状态
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
bool CAN_StatusGet(u8 enCanStatus)
{
    bool bRet = false;

    if (CAN->CFG_STAT & enCanStatus)
    {
        bRet = true;
    }
    return bRet;
}

/**
 * @brief    获取can接收错误计数
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_RxErrorCntGet(void)
{
    return CAN->RECNT;
}

/**
 * @brief    获取can发送错误计数
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_TxErrorCntGet(void)
{
    return CAN->TECNT;
}

/**
 * @brief    获取can仲裁失败位置
 * @par 更新记录  V1.0  2022/09/19  YangZJ  创建
 */
u8 CAN_ArbitrationLostCap(void)
{
    return (CAN->EALCAP & 0x1f);
}

/*! @} */
