#ifndef __foc_h__
#define __foc_h__

#include "main.h"
#include "mymode.h"

#define	PI		3.1416f
#define _2PI	6.2832f
#define _2PI_3 	2.0944f		//2*PI/3 120度

#define	Sqrt2	1.4142f
#define	Sqrt3	1.7321f
#define Encoder_Constant	0.0000958753f		//编码器角度换算
#define	Clark_b				0.5774f		//Sqrt3/2*2/3
#define	Clark_a				0.6667f		//常数 (先用,不理原理)

#define TS_timer2	0.001f		//定时器的1ms中断
#define TS_V_get	0.001f		//速度获取周期
#define TS_PWM		0.00005f		//pwm触发ADC中断的周期 也就是pwm周期
#define timer2_V	(TS_V_get/TS_PWM)//获取速度需要延时的次数
#define KTH7823_ADC	0.000002f		//从触发PWM中断开始 编码器延时1us 通信延时7us ADC采集完成2us 相对于ADC采集时刻预估 大约会早于ADC时刻2us
#define KTH7823_PWM	0.0001f		//编码器延时1us 相对于PWM输出时刻预估一般需要向前预估一个PWM周期的时间 TS_PWM 这应该是固定值

#define timer2_CCR 	(192000000.0f *TS_timer2)
#define PWMCCR		(192000000/2.0f/1.0f*TS_PWM) //4800 //4800	//pwm的溢出值配置	pwm频率 = 192 000 000/2/PWMCCR
//#define PWMCCR_2	PWMCCR/2		//占空比的一半
#define PWMCCR_MAX_SET	(PWMCCR-30)	//可设置的最大占空比 由于上桥打开时间不是无限长,且下桥需要时间采样
#define Vdc			12.0f		//母线电压
#define Vdc_Half	(Vdc/2.0f)	//母线电压一半
#define Vmax		6.93f		//24V电压下,最大为13.86v
#define dead_time_ 	0.000000050f	//死区时间(s) 50ns  
#define Vdc_dead_time 	(PWMCCR*dead_time_/TS_PWM/PWMCCR*Vdc) 		//死区补偿所需电压  0.192
#define dead_time_threshold  0.03f		//电压判断的死区补偿阈值 是一个电压值 满电压是 Vdc  出厂默认值
#define K_dead_time_threshold (dead_time_threshold+Vdc_dead_time)/dead_time_threshold 	//死区补偿的过零线性过度 和阈值有关

#define constantADC_CURRENT_SCALE_A	0.002504f //	//相电流校准系数
#define constantADC_CURRENT_SCALE_B	0.002504f //	ADC最大值为+-8192
#define constantADC_CURRENT_SCALE_C	0.002504f //

#define constantADC_Voltage_SCALE_A	0.00434758f		//相电压校准系数
#define constantADC_Voltage_SCALE_B	0.00438542f
#define constantADC_Voltage_SCALE_C	0.00432451f
#define constantADC_Voltage_SCALE_POWER 0.00414941f	//母线电压校准系数
#define V_I_SCALE	0.003f	//高转速下的电流补偿系数,量级在0.03-0.001 越大,越稳定,电流虚报越大
#define DIV_enco 0	//编码器方向 旋转方向 0-1

//MOTO.state 状态标志
#define	state_offset				0x00001	//开机校准电流偏置
#define	state_Calibration_A			0x00002	//电角度校准模式
#define	state_FOCSVPWM				0x00004	//电机输出pwm使能
#define	state_FOCloopID				0x00008	//电流D闭环使能
#define	state_FOCloopIQ				0x00010	//电流Q闭环使能
#define	state_FOCloopspeed			0x00020	//速度闭环使能
#define	state_FOCloopsPOS			0x00040	//位置闭环使能
#define	state_encoder_measurement	0x00080	//编码器数据采集使能
#define	state_Current_measurement	0x00100	//电流测量计算使能
#define	state_getspeed				0x00200	//速度测量使能
#define state_F						0X00400	//电角度开环模式  配合VF,IF
//#define state_IF					0X00800	//电流开环	//用不上这个 可以由其他模式代替
#define	state_circuit_Brake			0X01000	//电机短路刹车
#define	state_Disconnect			0X02000	//断开电机
#define get_uABC					0x04000	//ABC电压采样使能
#define	state_Calibration_RL		0x08000	//电感参数辨识模式
#define	state_FOC_Current_protec	0x10000	//电流保护使能
#define state_sensorless			0x20000	//无感算法 在无传感器的时候获得电角度和机械速度 需要得到磁极对数

/*
模式搭配控制：
基础配置： state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC	
0X4384	17284 //DQ电压控制模式						state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM																		=0x0080+0x0200+0x0100+0x4000+0x0004=																		
0X438C	17292 //占空比控制模式(Q轴电压加D轴电流环)	state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID														=0X0080+0X0200+0X0100+0X4000+0X0004+0X0008=							
0X439C	17308 //电流环控制模式						state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ										=0X0080+0X0200+0X0100+0X4000+0X0004+0X0008+0X0010=					
0X43BC	17340 //速度控制+电流闭环					state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ+state_FOCloopspeed					=0X0080+0X0200+0X0100+0X4000+0X0004+0X0008+0X0010+0X0020=			
0X43AC	17324 //速度控制+占空比模式					state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopspeed									=0X0080+0X0200+0X0100+0X4000+0X0004+0X0008+0X0020=					
0X43FC	17404 //位置控制+速度控制+电流闭环			state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ+state_FOCloopspeed+state_FOCloopsPOS	=0X0080+0X0200+0X0100+0X4000+0X0004+0X0008+0X0010+0X0020+0X0040=	
0X43EC	17388 //位置控制+速度控制+占空比模式		state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopspeed+state_FOCloopsPOS					=0X0080+0X0200+0X0100+0X4000+0X0004+0X0008+0X0020+0X0040=			
0X4784	18308 //VF电压开环控制						state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_F																=0X0080+0X0200+0X0100+0X4000+0X0004+0X0400=							
0X479C	18332 //IF电流开环控制						state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ+state_F								=0X0080+0X0200+0X0100+0X4000+0X0004+0X0008+0X0010+0X0800=			
0X6380	25472 //断开电机							state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_Disconnect																	=0X0080+0X0200+0X0100+0X4000+0X2000=								
0X5380	21376 //电机短路刹车						state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_circuit_Brake																	=0X0080+0X0200+0X0100+0X4000+0X1000=								
*/

#define MODE_FOC_VOLTAGE    state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM																		//DQ电压控制模式						
#define MODE_FOC_VOLT_IQ    state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID														//占空比控制模式(Q轴电压加D轴电流环)	
#define MODE_FOC_CURRENT    state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ										//电流环控制模式						
#define MODE_FOC_SPEED      state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ+state_FOCloopspeed					//速度控制+电流闭环					
#define MODE_SPEED_DUTY     state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopspeed									//速度控制+占空比模式					
#define MODE_FOC_POSITION   state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ+state_FOCloopspeed+state_FOCloopsPOS	//位置控制+速度控制+电流闭环			
#define MODE_POS_DUTY       state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopspeed+state_FOCloopsPOS					//位置控制+速度控制+占空比模式		
#define MODE_VF_OPENLOOP    state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_F																//VF电压开环控制						
#define MODE_IF_OPENLOOP    state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_FOCSVPWM+state_FOCloopID+state_FOCloopIQ+state_F								//IF电流开环控制						
#define MODE_MOTOR_OFF      state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_Disconnect																	//断开电机							
#define MODE_BRAKE_SHORT    state_encoder_measurement+state_getspeed+state_Current_measurement+get_uABC+	state_circuit_Brake																	//电机短路刹车						



#define	REG_ERR_
#define err_voltage				0x01	//电压异常
#define err_Overcurrent			0x02	//过流
#define err_Overtemperature		0x04	//过温
#define err_Over_ctrl_time		0x08	//超时
#define err_currentLOOP_Failure	0x10	//电流环失效
#define err_SpeedLOOP_Failure	0x20	//速度环失效

//NVR寄存器 掉电保持内存/通信寄存器
#define		ERG_magic				123		//新片校验

#define		REG_NEW					0
#define   	REG_SYS_MODE			1		// 模式 写1 进入BOOT升级模式
#define		REG_addr				2		// 地址 设备ID
#define   	REG_Version				3		// 版本
#define		REG_Output_VoltageD		4		// 输出电压(D轴)
#define		REG_Output_VoltageQ		5		// 输出电压(Q轴)
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
#define		REG_L					18		// 电机电感
#define		REG_R					19		// 电机电阻
#define		REG_debug				20		// 调试模式	输入'0'发送FOC调试数据,输入"1"发送寄存器部分数据,输入"50"停止自动发送数据
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
#define		REG_NONE_43             43
#define		REG_NONE_44             44
#define		REG_NONE_45             45
#define		REG_NONE_46             46
#define		REG_NONE_47             47
#define		REG_NONE_48             48
#define		REG_NONE_49             49
#define		REG_NONE_50             50
#define		REG_NONE_51             51
#define		REG_NONE_52             52
#define		REG_NONE_53             53
#define		REG_NONE_54             54
#define		REG_NONE_55             55
#define		REG_NONE_56             56
#define		REG_NONE_57             57
#define		REG_NONE_58             58
#define		REG_NONE_59             59
#define		REG_NONE_60             60
#define		REG_NONE_61             61
#define		REG_NONE_62             62
#define		REG_NONE_63             63
#define		REG_NONE_64             64
#define		REG_NONE_65             65
#define		REG_NONE_66             66
#define		REG_NONE_67             67
#define		REG_NONE_68             68
#define		REG_NONE_69             69
#define		REG_NONE_70             70
#define		REG_NONE_71             71
#define		REG_NONE_72             72
#define		REG_NONE_73             73
#define		REG_NONE_74             74
#define		REG_NONE_75             75
#define		REG_NONE_76             76
#define		REG_NONE_77             77
#define		REG_NONE_78             78
#define		REG_NONE_79             79
#define		REG_NONE_80             80
#define		REG_NONE_81             81
#define		REG_NONE_82             82
#define		REG_NONE_83             83
#define		REG_NONE_84             84
#define		REG_NONE_85             85
#define		REG_NONE_86             86
#define		REG_NONE_87             87
#define		REG_NONE_88             88
#define		REG_NONE_89             89
#define		REG_NONE_90             90
#define		REG_NONE_91             91
#define		REG_NONE_92             92
#define		REG_NONE_93             93
#define		REG_NONE_94             94
#define		REG_NONE_95             95
#define		REG_NONE_96             96
#define		REG_NONE_97             97
#define		REG_NONE_98             98
#define		REG_NONE_99             99
#define		REG_NONE_100            100
#define		REG_NONE_101            101
#define		REG_NONE_102            102
#define		REG_NONE_103            103
#define		REG_NONE_104            104
#define		REG_NONE_105            105
#define		REG_NONE_106            106
#define		REG_NONE_107            107
#define		REG_NONE_108            108
#define		REG_NONE_109            109
#define		REG_NONE_110            110
#define		REG_NONE_111            111
#define		REG_NONE_112            112
#define		REG_NONE_113            113
#define		REG_NONE_114            114
#define		REG_NONE_115            115
#define		REG_NONE_116            116
#define		REG_NONE_117            117
#define		REG_NONE_118            118
#define		REG_NONE_119            119
#define		REG_NONE_120            120
#define		REG_NONE_121            121
#define		REG_NONE_122            122
#define		REG_NONE_123            123
#define		REG_NONE_124            124
#define		REG_NONE_125            125
#define		REG_NONE_126            126
#define		REG_NONE_127            127
#define		REG_NONE_128            128
#define		REG_NONE_129            129
#define		REG_NONE_130            130
#define		REG_NONE_131            131
#define		REG_NONE_132            132
#define		REG_NONE_133            133
#define		REG_NONE_134            134
#define		REG_NONE_135            135
#define		REG_NONE_136            136
#define		REG_NONE_137            137
#define		REG_NONE_138            138
#define		REG_NONE_139            139
#define		REG_NONE_140            140
#define		REG_NONE_141            141
#define		REG_NONE_142            142
#define		REG_NONE_143            143
#define		REG_NONE_144            144
#define		REG_NONE_145            145
#define		REG_NONE_146            146
#define		REG_NONE_147            147
#define		REG_NONE_148            148
#define		REG_NONE_149            149
#define		REG_NONE_150            150
#define		REG_NONE_151            151
#define		REG_NONE_152            152
#define		REG_NONE_153            153
#define		REG_NONE_154            154
#define		REG_NONE_155            155
#define		REG_NONE_156            156
#define		REG_NONE_157            157
#define		REG_NONE_158            158
#define		REG_NONE_159            159
#define		REG_NONE_160            160
#define		REG_NONE_161            161
#define		REG_NONE_162            162
#define		REG_NONE_163            163
#define		REG_NONE_164            164
#define		REG_NONE_165            165
#define		REG_NONE_166            166
#define		REG_NONE_167            167
#define		REG_NONE_168            168
#define		REG_NONE_169            169
#define		REG_NONE_170            170
#define		REG_NONE_171            171
#define		REG_NONE_172            172
#define		REG_NONE_173            173
#define		REG_NONE_174            174
#define		REG_NONE_175            175
#define		REG_NONE_176            176
#define		REG_NONE_177            177
#define		REG_NONE_178            178
#define		REG_NONE_179            179
#define		REG_NONE_180            180
#define		REG_NONE_181            181
#define		REG_NONE_182			182
#define		REG_A_offset0			183	  // 编码器校准起始地址
#define		REG_A_offset1			184    
#define		REG_A_offset2           185    
#define		REG_A_offset3           186    
#define		REG_A_offset4           187    
#define		REG_A_offset5           188    
#define		REG_A_offset6           199    
#define		REG_A_offset7           190    
#define		REG_A_offset8           191    
#define		REG_A_offset9           192    
#define		REG_A_offset10          193    
#define		REG_A_offset11          194    
#define		REG_A_offset12          195    
#define		REG_A_offset13          196    
#define		REG_A_offset14          197    
#define		REG_A_offset15          198    
#define		REG_A_offset16          209    
#define		REG_A_offset17          200    
#define		REG_A_offset18          201    
#define		REG_A_offset19          202    
#define		REG_A_offset20          203    
#define		REG_A_offset21          204    
#define		REG_A_offset22          205    
#define		REG_A_offset23          206    
#define		REG_A_offset24          207    
#define		REG_A_offset25          208    
#define		REG_A_offset26          219    
#define		REG_A_offset27          210    
#define		REG_A_offset28          211    
#define		REG_A_offset29          212    
#define		REG_A_offset30          213    
#define		REG_A_offset31          214    
#define		REG_A_offset32          215    
#define		REG_A_offset33          216    
#define		REG_A_offset34          217    
#define		REG_A_offset35          218    
#define		REG_A_offset36          229    
#define		REG_A_offset37          220    
#define		REG_A_offset38          221    
#define		REG_A_offset39          222    
#define		REG_A_offset40          223    
#define		REG_A_offset41          224    
#define		REG_A_offset42          225    
#define		REG_A_offset43          226    
#define		REG_A_offset44          227    
#define		REG_A_offset45          228    
#define		REG_A_offset46          239    
#define		REG_A_offset47          230    
#define		REG_A_offset48          231    
#define		REG_A_offset49          232    
#define		REG_A_offset50          233    
#define		REG_A_offset51          234    
#define		REG_A_offset52          235    
#define		REG_A_offset53          236    
#define		REG_A_offset54          237    
#define		REG_A_offset55          238    
#define		REG_A_offset56          249    
#define		REG_A_offset57          240    
#define		REG_A_offset58          241    
#define		REG_A_offset59          242    
#define		REG_A_offset60          243    
#define		REG_A_offset61          244    
#define		REG_A_offset62          245    
#define		REG_A_offset63          246    
#define		REG_A_offset64          247    
#define		REG_A_offset65          248    
#define		REG_A_offset66          259    
#define		REG_A_offset67          250    
#define		REG_A_offset68          251    
#define		REG_A_offset69          252    
#define		REG_A_offset70          253    
#define		REG_A_offset71          254






typedef struct
{
	float IA;	//电流参数
	float IB;
	float IC;
	float Ia;
	float Ib;
	float ID;
	float IQ;
	
	int  Current_offset[3];	//电流偏置
	int  Voltage_offset[3];	//电压偏置
	float UA;	//电压参数
	float UB;
	float UC;
	float Ua;
	float Ub;
	float UD;
	float UQ;
	float GET_UA;	//测量电压
	float GET_UB;
	float GET_UC;
	float GET_Ua;
	float GET_Ub;
	float GET_Ud;
	float GET_Uq;
	float OUT_U;	//直接电压输入的值,也是速度环输出的值
	float OUT_I;	//电流输出值
	float REF_I;	//目标电流,也是限制电流值
	
	float Mechanical_Angle;			//单圈机械角度 red
	float Last_Mechanical_Angle;	//上一次单圈机械角度 red
	int Laps;						//圈数 必须使用int,否者不能记录负速度 r
	float Theta_Absoluteangle;		//多圈绝对角度 red
	float Theta_lastAbsoluteangle;	//上一次多圈绝对角度 red
	float Aw;						//角速度 red/s
	float A_openloop;				//电压开环速度设置
	float Theta_adc;				//单圈电角度 red	对齐ADC采集时刻的电角度
	float Theta_pwm;				//单圈电角度 red	对齐PWM输出时刻的电角度
	float Theat_Offset;				//电角度偏移 red
	int	DIV	;		//线序
	int	PP	;		//磁极对数
	float L	;		//电机电感
	float R	;		//电机电阻
	float  F_PI ;	//电流环PI参数带宽
	u32 state;		//运行模式
	char Safety;	//运行使能 是否触发安全保护
	float DEADZONE_CUR_THR; //死区补偿电流判断 转换阈值 一般是堵转时电流噪声峰峰值的2-3倍
	float DEADZONE_VOL_THR; //死区补偿电压判断
	float K_dead_time;		//死区补偿斜率
	KalmanFilter AW_LPF;

}moto_;



extern moto_ MOTO;
typedef struct
{
	float 	TS					;//执行周期(单位s)(根据定时器中断或者ADC中断来配置)
	float	VIN					;//校准时的运行速度(单位red/s(极对数为1时))
	float	UIN					;//校准时设置电压(需要设置)
	float	DIV					;//电机线序(用户需要读取该值,根据传感器检测到的电机旋转方向得出的结果,这里需要设置编码器逆时钟为正向,若电机旋转为顺时钟DIV=1(反向))
	u16 	PP					;//测量出的磁极对数(用户读取)
	float	Zero_position_offset;//测出的零位电角度偏移量(用户读取,减去测量值即可获得转子零位)
	u32 	Zero_I[3]			;//电流校准值(用于校准3相电流零位,直接与测量电流测量值相减)
	char 	over				;//校准结束标志(当该值为1时说明校准完成,用户读取)
	u32 	time_delay[3]		;//校准时的时钟		(不需要设置)
	float 	po					;//运行过程中的电角度			(不需要设置)
	float 	po_last				;//运行过程中的上一次电角度	(不需要设置)
	float 	po_enco				;//运行过程中测量的角度	(不需要设置)
	float 	po_enco_last		;//上一次角度			(不需要设置)
	short 	laps				;//圈数						(不需要设置)
	float 	Absolute_po			;//绝对位置			(不需要设置)
	u32 	steps				;//校准函数在switch运行中的步骤
	float 	A_offset[72]		;//每极对数校准3个值(每极对有3个绕组) 最高校准24极对数电机
} Calibration;	//校准
typedef struct
{
	float 	TS					;//执行周期(单位s)(根据定时器中断或者ADC中断来配置)
	float	w					;//校准时的运行速度(单位red/s(极对数为1时))
	float	UIN					;//校准时设置电压(需要设置)
	float	R					;//测出的电机电阻
	float	L					;//测出的电机电感
	float	UD					;//测出的D电压
	float	UQ					;//测出的Q电压
	float	ID					;//测出的D电流
	float	IQ					;//测出的Q电流
	char 	over				;//校准结束标志(当该值为1时说明校准完成,用户读取)
	u32 	time_delay[3]		;//校准时的时钟		(不需要设置)
	float 	Absolute_po			;//绝对位置			(不需要设置)
	u32 	steps				;//校准函数在switch运行中的步骤
	float 	X;	//抗性
} LR_get;	//校准
extern LR_get moto_Calibration_LR;	//电机电阻电感自动获取结构体
extern Calibration moto_Calibration;//电机校准函数结构体
typedef struct 
{
	float temp;		//PCB温度
	float Voltage;	//母线电压
	float Overcurrent;	//过流保护值
	float Overtemperature;	//过流保护值
	u32 Over_ctrl_timer;	//通信超时时间
	u8 flag_over_Voltage;	//过压信号
}PCB_;
extern PCB_ PCB;

void timer2irq_init(u32 timer);
void ABC_OUT(float *UA,float *UB,float *UC);
void Seven_SVPWM (int *Direct,float *theta,float *UD,float *UQ,float *Ua,float *Ub,float *UA,float *UB,float *UC);
float Encoder(void);
void Angle_get(float IN, float *Theta_ADC,float *Theta_PWM, float *Last_Mechanical_Angle_, float *Mechanical_Angle_, float *Theta_Absoluteangle_, int *Laps_, float *Aw_, int *PP_);
void Clark_Park(float *theta,float*I_A,float*I_B,float*I_C,float*I_a,float*I_b,float*I_d,float*I_q);
void set_Calibration_Calibration(void);
void Moto_Calibration_Program(Calibration *stru);
void MC_Speed_GetActualSpeed(float*Theta_Absoluteangle_,float*Theta_lastAbsoluteangle_,float*Mechanical_Angle_,int*Laps_,float*speed_);
void set_Calibration_LR_get(void);
void Moto_Calibration_RL(LR_get *stru);
void stop(void);
void Data_preprocessing(void);
float enc2mech_fpu(float x);
void Power_on_moto(void);

#define WIN   100          // 窗口长度（可调）
#define WIN_M1 (WIN-1)
/* float 版 */
typedef struct {
    float fifo[WIN];
    float sum;
    uint8_t idx;
} MAF_t;
float maf_f32(MAF_t *m, float x);
void set_const_init(void);
void reg_Parse(u16 reg_addr,float Value);
///* ================================================================
// * Sensorless_SMO 参数结构体(SMO/PLL/相位补偿可调参数集中管理):
// *   可调参数通过指针传入 Sensorless_SMO, 不在函数内重复定义;
// *   填好电机参数 n_max_rpm/Ke_Vs/I_rated 并置 param_auto=1 后,
// *   第一次切入无感时自动覆盖可调参数; 之后转速误差过大只重播种不重算参数
// * ================================================================ */
//typedef struct
//{
//	/* ---- 可调参数(手动整定默认值, 见 FOC.c 中 SMO_Param 初始化) ---- */
//	float LPF_FC;		// 反电动势LPF截止频率(Hz): #26实测(PP=10)取250Hz为稳定区(<300Hz自起+稳定+方向纠偏全正常; ≥300Hz抖振滤不净→err打满→引导/锁定循环, 原推荐(3~8)*fe_max被钳到2kHz即此例). 高速限制: fe_max=1kHz@6000rpm时250Hz一阶低通使BEMF幅值衰减≈0.28/相位滞后≈76°, 电压开环phi_comp=0无补偿→全速段需动态LPF或电流闭环方案
//	float K_RATIO;		// 滑模增益系数: K=K_RATIO*|E|, 取1.1~1.5(略大于1保证收敛)
//	float K_MIN;		// 滑模增益下限(V): 约1.2*Ke*we_min(最低稳定转速处BEMF幅值)
//	float DELTA;		// 饱和函数边界(A): 约(1~3)%*额定电流幅值
//	float KP_PLL;		// PLL比例: 约2*zeta*wn(zeta=0.707, f_pll=50~150Hz)
//	float KI_PLL;		// PLL积分: 约wn^2=(2PI*f_pll)^2
//	float EPS;			// 防除零
//	float Aw_err_limit;	// PLL误差限幅(rad): 大惯量电机取0.1~0.3防震荡
//	float PHI_COMP;		// 相位超前补偿系数(0~1.0): 补偿LPF滞后角, 从0.3保守起步
//	float PHI_COMP_MAX;	// 补偿角限幅(rad,约45度): 防高速动态过补偿正反馈失控
//	float DPHI_MAX;		// 补偿角每周期变化率限幅(rad): 防转速突变时角度跳变
//	float OMEGA_S_FC;	// 补偿用速度平滑截止频率(Hz): 滤PLL抖振, 防补偿角跳动
//	float OMEGA_MAX;	// PLL电气速度钳位(rad/s): 防跑飞硬保护, 推荐1.2*2PI*PP*n_max/60
//	float PHI_ERR;		// 补偿角-误差负反馈系数: phi+=PHI_ERR*err, err<0自动减小补偿角
//	/* ---- 低速引导/过零穿越(bootstrap)参数: 0速启动+正反转穿越BEMF不可观区 ---- */
//	float E_boot_thr;	// BEMF不可观门限(V): |E|<此值判为低速/过零→进入开环同步拖动; #27 0.06(配Omega_boot=150rpm, 真Ke≈0.0008@157rad/s→0.126V可切出, 失锁判0.5*此=0.03V)
//	float Omega_boot;	// 引导目标电角速度(rad/s): ramp拖动到该速度、BEMF可测后切回PLL; #27 ≈2PI*PP*150rpm/60=157rad/s(原300rpm→314太快易失步/带载差, 0X24104低磁链小惯量)
//	float Boot_accel;	// 引导电角加速度(rad/s^2): ramp缓升/缓降, 防电流冲击与拖失步; #27 1200(原3000)拉长平缓速度上升阶段
//	float Boot_switch_N;// 引导→PLL切换确认: BEMF达标且|err|收敛需连续周期数(防噪声误切)
//	float Boot_retry_N;	// PLL失锁→回引导重试: err打满连续周期数(锁相失败自动重新同步)
//	float Boot_timeout_N;// 引导超时周期数: 长时间未切出(拖不动/堵转)→降速归零重新爬升
//	float K_smooth;		// 滑模增益K一阶平滑系数(0~1): 抑制小L放大导致的抖振, 越大跟随越快
//	char  Dir_ref_src;	// 期望方向参考源: 0=auto(跟随MOTO.UQ符号, 支持±UQ双向启动/反转); 1=固定正转(测试)
//	/* ---- 自动计算用电机参数(需按电机填写) ---- */
//	float n_max_rpm;	// 最高机械转速(rpm)   (电机铭牌)
//	float Ke_Vs;		// 相反电动势系数(V*s/rad)=磁链(Wb); 反拖测线反电动势换算   旋转电机,得到反电动势,幅值/转速就是反电动势系数
//	float I_rated;		// 额定电流幅值(A)
//	char  param_auto;	// 1=第一次切入无感时用电机参数自动覆盖可调参数
//	/* ---- 失锁接管增强参数(param_auto=1时由自动块覆盖; 默认值见 FOC.c) ---- */
//	float Dir_flip_omega_thr;	// 反转过零接管速度阈值(rad/s): dir_ref翻号后|Omega_pll|跌入此值才回引导拖过零, 保守防误触发, 默认0.5*Omega_boot
//	float Err_fake_N;		// 假平衡确认拍数: err打满但|Omega_pll|≈0连续此拍数判锁相失败→回引导(默认50拍=2.5ms)
//	float Lock_hold_N;		// #25 lock-hold window ticks: after guide->PLL only a(BEMF drop)/e(reverse) may re-boot, mask b/c/d to stop guide<->lock cycle (default 300~15ms, 0=off)
//	float Dir_opp_N;		// #26 方向权威确认拍数: UQ方向明确(|UQ|>0.02)且Omega_pll*dir_ref<0且|Omega_pll|>Dir_opp_omega_thr, 连续此拍数判"外力拖反不回正"→回引导按dir_ref拖过零(默认30≈1.5ms)
//	float Dir_opp_omega_thr;	// #26 方向相悖判定速度下限(rad/s): 防0速/过零抖动误判, 默认0.15*Omega_boot≈47rad/s(≈45rpm机械), 与BEMF判据a盲区无缝
//} SMO_Param_t;
//extern SMO_Param_t SMO_Param;	// SMO参数实例(在FOC.c中初始化, 上电时可按电机修改)

//void Sensorless_SMO(SMO_Param_t *smo, float R, float L, int PP, float *Ia, float *Ib, float *Ua, float *Ub, float *Theta_pred, float *Omega_pred);
//typedef struct
//{
//	float Rs;				//电机电阻
//	float Ls;				//电机电感
//	float Fi;				//电机磁链
//	float ua_est;			//ua
//	float ub_est;			//ub
//						//ia
//						//ib
//	float ia_est;		//ia估计
//	float ib_est;		//ib估计
//	float ia_err;		//估计误差
//	float ib_err;		//ib估计误差
//						//角度估计
//						//角速度估计
//	float Kp;			//电流估计误差比例增益K
//	float Ki;			//积分增益
//						//A矩阵
//						//B矩阵
//						//C矩阵
//} sm_Param_t;
//extern sm_Param_t sm_GET;
//void SMO_Sensorless(sm_Param_t * EKF_,float ia,float ib,float ua,float ub,float *angle,float *angular);


#endif
