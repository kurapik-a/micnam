
#include "OLED_Font.h"
#include "stdio.h"
#include "stdarg.h"
#include "oled.h"
#include "bsp.h"
#include "stdio.h"
#include "stdarg.h"
#include "math.h"

// 限幅函数:
static inline float limit_(float x, float min, float max)
{
	return x < min ? min : (x > max ? max : x);
}

/*引脚配置*/
// #define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)(x))
// #define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(x))

/*引脚初始化*/
void OLED_I2C_Init(void)
{
	//	MODE_GPIOPININIT_ALL(GPIOB,GPIO_Pin_12,GPIO_Mode_Out_OD);
	//	MODE_GPIOPININIT_ALL(GPIOB,GPIO_Pin_13,GPIO_Mode_Out_OD);
	//	OLED_W_SCL(1);
	//	OLED_W_SDA(1);
}

/**
 * @brief  I2C开始
 * @param  无
 * @retval 无
 */
void OLED_I2C_Start(void)
{
	//	MyI2C_Start();
}

/**
 * @brief  I2C停止
 * @param  无
 * @retval 无
 */
void OLED_I2C_Stop(void)
{
	//	MyI2C_Stop();
}

/**
 * @brief  I2C发送一个字节
 * @param  Byte 要发送的一个字节
 * @retval 无
 */
// void OLED_I2C_SendByte(uint8_t Byte)
//{
//	MyI2C_SendByte(Byte);
// }

/**
 * @brief  OLED写命令
 * @param  Command 要写入的命令
 * @retval 无
 */
#include "i2c_application.h"
void OLED_WriteCommand(uint8_t Command)
{
	//	OLED_I2C_Start();
	//	OLED_I2C_SendByte(0x78);		//从机地址
	//	MyI2C_ReceiveAck();
	//	OLED_I2C_SendByte(0x00);		//写命令
	//	MyI2C_ReceiveAck();
	//	OLED_I2C_SendByte(Command);
	//	MyI2C_ReceiveAck();
	// OLED_I2C_Stop();

	u8 a[2];
	a[0] = 0;
	a[1] = Command;
	I2C1_send_data(0x78, a, 2);
	while (i2c_wait_end(&hi2cx, 0xFFFFFFF) != I2C_OK)
		;
}

/**
 * @brief  OLED写数据
 * @param  Data 要写入的数据
 * @retval 无
 */
void OLED_WriteData(uint8_t Data)
{
	//	OLED_I2C_Start();
	//	OLED_I2C_SendByte(0x78);		//从机地址
	//	MyI2C_ReceiveAck();
	//	OLED_I2C_SendByte(0x40);		//写数据
	//	MyI2C_ReceiveAck();
	//	OLED_I2C_SendByte(Data);
	//	MyI2C_ReceiveAck();
	// OLED_I2C_Stop();

	u8 a[2];
	a[0] = 0x40;
	a[1] = Data;
	I2C1_send_data(0x78, a, 2);
	while (i2c_wait_end(&hi2cx, 0xFFFFFFF) != I2C_OK)
		;
	//	I2C_WriteNbyte(0x78,0x40,&Data,1);
}
void I2C_WriteNbyte(uint8_t slave_addr, uint8_t a, uint8_t *b, uint16_t len)
{
	u8 i[140];
	i[0] = a;
	for (int k = 0; k < len; k++)
	{
		i[k + 1] = b[k];
	}
	I2C1_send_data(slave_addr, i, len + 1);
	while (i2c_wait_end(&hi2cx, 0xFFFFFFF) != I2C_OK)
		;
}
/**
 * @brief  OLED设置光标位置
 * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
 * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
 * @retval 无
 */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	// OLED_WriteCommand(0xB0 | Y);				 // 设置Y位置
	// OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // 设置X位置低4位
	// OLED_WriteCommand(X & 0x0F);				 // 设置X位置高4位

	uint8_t cmd[7] = {0x00,						// 控制字节：命令流
					  0xB0 | Y,					// 页地址
					  0x00,						// 列高 4 位
					  0x10 | ((X & 0xF0) >> 4), // 列低 4 位
					  X & 0x0F};				// 列低 4 位
	/* 一次发出 3 条命令（共 7 字节） */
	I2C1_send_data(0x78, cmd, 7);
	while (i2c_wait_end(&hi2cx, 0xFFFFFFF) != I2C_OK)
		;
}

/**
 * @brief  OLED清屏
 * @param  无
 * @retval 无
 */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for (i = 0; i < 128; i++) // 左到右
		{
			OLED_WriteData(0x00); // 上到下
		}
	}
}

/**
 * @brief  OLED显示一个字符
 * @param  Line 行位置，范围：1~4
 * @param  Column 列位置，范围：1~16
 * @param  Char 要显示的一个字符，范围：ASCII可见字符
 * @retval 无
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((u8)((Line - 1) * 2), (u8)((Column - 1) * 8)); // 设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]); // 显示上半部分内容
	}
	OLED_SetCursor((u8)((Line - 1) * 2 + 1), (u8)((Column - 1) * 8)); // 设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]); // 显示下半部分内容
	}
}
void Char_mini(u8 x, u8 y, u8 chr)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';
	if (x > 1 - 1)
	{
		x = 0;
	}
	OLED_SetCursor(x, y);
	for (i = 0; i < 6; i++)
		OLED_WriteData(F6x8[c][i]);
}

/**
 * @brief  OLED显示字符串
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  String 要显示的字符串，范围：ASCII可见字符
 * @retval 无
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, (u8)(Column + i), String[i]);
	}
}
void OLED_ShowString_mini(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		Char_mini((u8)(Line - 1), (u8)(Column - 1 + i * 6), String[i]);
	}
}

/**
 * @brief  OLED次方函数
 * @retval 返回值等于X的Y次方
 */
uint32_t OLED_Pow(uint32_t X, long Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}
void printf_oled(char a, char b, char *stringg, ...) // 格式化发送
{
	static char string[100]; // 一个足够长的字符串
	static va_list arg;
	va_start(arg, stringg);
	vsprintf(string, stringg, arg);
	va_end(arg);
	OLED_ShowString(a, b, string); // 发送函数 可以是oled usart spi 。。。。字符串格式
}
void printf_oled_mini(char a, char b, char *stringg, ...) // 格式化发送
{
	static char string[100]; // 一个足够长的字符串
	static va_list arg;
	va_start(arg, stringg);
	vsprintf(string, stringg, arg);
	va_end(arg);
	OLED_ShowString_mini(a, b, string); // 发送函数 可以是oled usart spi 。。。。字符串格式
}

// 区域填充(x1,y2)(x2,y2)左上->右下(坐标)
void AreaFill(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
	short i, j;
	short dx;
	dx = x2 - x1;
	for (i = y1; i < y2; i++)
	{
		OLED_SetCursor((u8)i, x1);
		for (j = 0; j < dx; j++)  // 竖直方向填充
			OLED_WriteData(0x00); // 上到下
	}
}
// F6x8[str-'']
// 针对目前代码 oled像素位置
// 两条竖线(x1,y2)(x2,y2)左上->右下(坐标)
void huakuang(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
	char i;
	for (i = y1; i < y2; i++)
	{
		OLED_SetCursor(i, x1);
		OLED_WriteData(0xff);
	}
	for (i = y1; i < y2; i++)
	{
		OLED_SetCursor(i, x2);
		OLED_WriteData(0xff);
	}
}

/**
 * @brief  OLED初始化
 * @param  无
 * @retval 无
 */
void OLED_Init(void)
{
	uint32_t i, j;

	for (i = 0; i < 3000; i++) // 上电延时
	{
		for (j = 0; j < 1000; j++)
			;
	}
	OLED_I2C_Start();
	//	OLED_I2C_Init();			//端口初始化

	OLED_WriteCommand(0xAE); // 关闭显示

	OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0xA8); // 设置多路复用率
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3); // 设置显示偏移
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40); // 设置显示开始行

	OLED_WriteCommand(0xA1); // 设置左右方向，0xA1正常 0xA0左右反置

	OLED_WriteCommand(0xC8); // 设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA); // 设置COM引脚硬件配置
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81); // 设置对比度控制
	OLED_WriteCommand(0xff);

	OLED_WriteCommand(0xD9); // 设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB); // 设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4); // 设置整个显示打开/关闭

	OLED_WriteCommand(0xA6); // 设置正常/倒转显示

	OLED_WriteCommand(0x8D); // 设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF); // 开启显示

	OLED_Clear(); // OLED清屏
}

// ------------------------------------ 显存操作 ------------------------------------
u8 bufdis[(maxy + 1) / 8 * (maxx + 1)]; // 申明显存 一个字节代表8个像素,所以/8
// 水平虚线
void Placedashed_line(char y)
{
	uint8_t x, i;
	y = limit_(y, miny, maxy);
	for (x = minx; x < maxx + 1; x++) // 遍历横向页
	{
		i++;
		if (i > 4)
		{
			bufdis[x + (y / 8) * maxx] |= 1 << (y % 8);
		} // 非覆盖
		if (i == 8)
			i = 0;
	}
}
// 竖直虚线
void PlaceVerticalline(char x)
{
	uint8_t i;
	x = limit_(x, minx, maxx);
	for (i = miny; i < (maxy + 1) / 8; i++) // 遍历纵向页
		bufdis[x + i * (maxx + 1)] = 0xf0;	// 覆盖
}
// 缓存归零
void Clearbuffer(void)
{
	static unsigned short i, B = (maxy + 1) / 8 * (maxx + 1);
	i = 0;
	for (; i < B; i++)
		bufdis[i] = 0;
}
// 显存页归零 (部分)
void Clearbuffer_bc(u8 y)
{
	static unsigned short i, B;
	i = y * (maxx + 1);
	B = (y + 1) * (maxx + 1);
	for (; i < B; i++)
		bufdis[i] = 0;
}
// 显示缓存区域
void OLED_Display(void)
{
	OLED_SetCursor(0, 0);											   // 目标页
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 0 * (maxx + 1)), (maxx + 1)); // 刷新行
	OLED_SetCursor(1, 0);
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 1 * (maxx + 1)), (maxx + 1)); // 刷新行
	OLED_SetCursor(2, 0);
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 2 * (maxx + 1)), (maxx + 1)); // 刷新行
	OLED_SetCursor(3, 0);
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 3 * (maxx + 1)), (maxx + 1)); // 刷新行
	OLED_SetCursor(4, 0);
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 4 * (maxx + 1)), (maxx + 1)); // 刷新行
	OLED_SetCursor(5, 0);
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 5 * (maxx + 1)), (maxx + 1)); // 刷新行
	OLED_SetCursor(6, 0);
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 6 * (maxx + 1)), (maxx + 1)); // 刷新行
	OLED_SetCursor(7, 0);
	I2C_WriteNbyte(0x78, 0x40, (bufdis + 7 * (maxx + 1)), (maxx + 1)); // 刷新行
}
// 画点到缓存
void PlacePixel(uint8_t y, uint8_t x)
{
	y = limit_(y, miny, maxy); // 对点的位置限幅,在屏幕范围内
	x = limit_(x, minx, maxx);
	bufdis[x + (y / 8) * (maxx + 1)] |= 1 << (y % 8); // 非覆盖画点
}
// 画黑点到缓存
void PlacePixel_back(uint8_t y, uint8_t x)
{
	y = limit_(y, miny, maxy); // 对点的位置限幅,在屏幕范围内
	x = limit_(x, minx, maxx);
	bufdis[x + (y / 8) * (maxx + 1)] &= ~(1 << (y % 8)); // 非覆盖画点
}
// 为了利用F6x8或F8x16数组来显示字符 写字节
void Placecharbit(uint8_t y, uint8_t x, uint8_t cha)
{
	char i = 0;
	for (; i < 8; i++)
	{
		if (cha & (1 << i))
			PlacePixel((u8)(y + i), x); // 列扫描 画一列
	}
}
// 写小字符
void Placechar(uint8_t y, uint8_t x, uint8_t chr)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';
	for (i = 0; i < 6; i++)
		Placecharbit(y, (u8)(x + i), F6x8[c][i]);
}
// 写小字符串
void PlaceString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
		Placechar(Line, (u8)(Column + i * 6), String[i]);
}
// 写大字符
void Placechar_F8x16(uint8_t x, uint8_t y, uint8_t chr)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';
	for (i = 0; i < 8; i++) // 上行
		Placecharbit(x, (u8)(y + i), OLED_F8x16[c][i]);
	for (i = 0; i < 8; i++) // 下行
		Placecharbit((u8)(x + 8), (u8)(y + i), OLED_F8x16[c][i + 8]);
}
// 反向写大字符
void Placechar_F8x16_back(uint8_t x, uint8_t y, uint8_t chr)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';
	for (i = 0; i < 8; i++) // 上行
		Placecharbit(x, (u8)(y + i), ~OLED_F8x16[c][i]);
	for (i = 0; i < 8; i++) // 下行
		Placecharbit((u8)(x + 8), (u8)(y + i), ~OLED_F8x16[c][i + 8]);
}
// 写大字符串
void PlaceString_F8x16(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
		Placechar_F8x16(Line, (u8)(Column + i * 8), String[i]);
}
// 反向写大字符串
void PlaceString_F8x16_back(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
		Placechar_F8x16_back(Line, (u8)(Column + i * 8), String[i]);
}
// 格式化缓存
void printf_Place(char a, char b, char *stringg, ...)
{
	static char string[100]; // 一个足够长的字符串
	static va_list arg;
	va_start(arg, stringg);
	vsprintf(string, stringg, arg);
	va_end(arg);
	PlaceString(a, b, string); // 发送函数 可以是oled usart spi 。。。。字符串格式
}
// 大字符格式化缓存
void printf_Place_F8x16(char a, char b, char *stringg, ...) // 格式化发送
{
	static char string[100]; // 一个足够长的字符串
	va_list arg;
	va_start(arg, stringg);
	vsprintf(string, stringg, arg);
	va_end(arg);
	PlaceString_F8x16(a, b, string); // 发送函数 可以是oled usart spi 。。。。字符串格式
}

// 反向大字显示
void printf_Place_F8x16_back(char a, char b, char *stringg, ...)
{
	static char string[100]; // 一个足够长的字符串
	static va_list arg;
	va_start(arg, stringg);
	vsprintf(string, stringg, arg);
	va_end(arg);
	PlaceString_F8x16_back(a, b, string); // 发送函数 可以是oled usart spi 。。。。字符串格式
}

// 写大字
void PlaceChinese_Fc32(uint8_t x, uint8_t y, uint8_t chr)
{
	unsigned char i = 0;
	for (i = 0; i < 16; i++) // 上行
		Placecharbit(x, (u8)(y + i), Fc32[chr][i]);
	for (i = 0; i < 16; i++) // 下行
		Placecharbit((u8)(x + 8), (u8)(y + i), Fc32[chr][i + 16]);
}
// 反向写大字
void PlaceChinese_Fc32_back(uint8_t x, uint8_t y, uint8_t chr)
{
	unsigned char i = 0;
	for (i = 0; i < 16; i++) // 上行
		Placecharbit(x, (u8)(y + i), ~Fc32[chr][i]);
	for (i = 0; i < 16; i++) // 下行
		Placecharbit((u8)(x + 8), (u8)(y + i), ~Fc32[chr][i + 16]);
}
short absx(short x)
{
	return x < 0 ? -x : x;
}

// 起点终点 使用的算法：bresenham 画直线
void Placeline(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
	// 限制坐标在有效范围内
	y1 = limit_(y1, miny, maxy);
	x1 = limit_(x1, minx, maxx);
	y2 = limit_(y2, miny, maxy);
	x2 = limit_(x2, minx, maxx);

	short dx = absx(x2 - x1);
	short dy = absx(y2 - y1);

	// 计算x和y方向的步进方向
	short sx = (x1 < x2) ? 1 : -1;
	short sy = (y1 < y2) ? 1 : -1;

	short err = dx - dy;
	short e2;

	while (1)
	{
		PlacePixel(y1, x1);

		// 到达终点时退出循环
		if (x1 == x2 && y1 == y2)
			break;

		e2 = 2 * err;
		if (e2 > -dy)
		{
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y1 += sy;
		}
	}
}
// 画黑线
void Placeline_back(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
	// 限制坐标在有效范围内
	y1 = limit_(y1, miny, maxy);
	x1 = limit_(x1, minx, maxx);
	y2 = limit_(y2, miny, maxy);
	x2 = limit_(x2, minx, maxx);

	short dx = absx(x2 - x1);
	short dy = absx(y2 - y1);

	// 计算x和y方向的步进方向
	short sx = (x1 < x2) ? 1 : -1;
	short sy = (y1 < y2) ? 1 : -1;

	short err = dx - dy;
	short e2;

	while (1)
	{
		PlacePixel_back(y1, x1);
		// 到达终点时退出循环
		if (x1 == x2 && y1 == y2)
			break;

		e2 = 2 * err;
		if (e2 > -dy)
		{
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y1 += sy;
		}
	}
}
// 画矩形
void rectangle(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
	Placeline(y1, x1, y2, x1);
	Placeline(y1, x1, y1, x2);
	Placeline(y2, x1, y2, x2);
	Placeline(y1, x2, y2, x2);
}

// 画实心矩形（单函数实现，内部使用静态全局变量优化效率）
void solid_rectangle(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
	static uint8_t g_start_y, g_end_y, g_start_x, g_end_x, y;
	g_start_y = (y1 < y2) ? y1 : y2;
	g_end_y = (y1 < y2) ? y2 : y1;
	g_start_x = (x1 < x2) ? x1 : x2;
	g_end_x = (x1 < x2) ? x2 : x1;
	// 利用预计算的坐标范围高效绘制
	for (y = g_start_y; y <= g_end_y; y++)
		Placeline(y, g_start_x, y, g_end_x);
}

// 画黑色矩形
void rectangle_back(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
	Placeline_back(y1, x1, y2, x1);
	Placeline_back(y1, x1, y1, x2);
	Placeline_back(y2, x1, y2, x2);
	Placeline_back(y1, x2, y2, x2);
}

// 画圆
void PlaceCyuan(short y1, short x1, float R)
{
	short p, k, d, e, f, g, h, j, q;
	float err, r = R * R, b; //,dx,dy
	short x = x1, y = y1;
	y1 = limit_(y1, miny, maxy);
	x1 = limit_(x1, minx, maxx);
	b = x1 + R / 1.414f;
	y1 -= R;
	p = x + x;
	k = y + y;
	d = x + y;
	e = y - x;
	f = x - y;
	g = 2 * x;
	h = x * x;
	j = (y - 0.5f) * (y - 0.5f);
	q = 2 * (y - 0.5f);
	while (x1 < b)
	{
		PlacePixel((u8)(y1), (u8)(x1));
		PlacePixel((u8)(y1), (u8)(-x1 + p));
		PlacePixel((u8)(-y1 + k), (u8)(x1));
		PlacePixel((u8)(-y1 + k), (u8)(-x1 + p));
		PlacePixel((u8)(x1 + e), (u8)(y1 + f));
		PlacePixel((u8)(x1 + e), (u8)(-y1 + d));
		PlacePixel((u8)(-x1 + d), (u8)(y1 + f));
		PlacePixel((u8)(-x1 + d), (u8)(-y1 + d)); // 圆的8个部分
		x1++;
		err = x1 * x1 - g * x1 + h + j - q * y1 + y1 * y1 - r;
		if (err > 0)
			y1 += 1;
	}
}
// 绘制仪表指针
// 位置: (CENTER_X, CENTER_Y)  半径: RADIUS  占比: P/M
// 适用于各种仪表，比例范围0-1对应圆周360度
// 举例:DrawGaugeNeedle(20,20,10,50,100);
void DrawGaugeNeedle(uint8_t CENTER_X, uint8_t CENTER_Y, uint8_t RADIUS, uint16_t P, uint16_t M)
{
	uint8_t endX;
	uint8_t endY;
	float angle_rad;
	float ratio; // 使用更具描述性的变量名
	// 防止除零错误
	if (M == 0)
		return;
	// 计算比例（0.0 - 1.0），使用浮点除法
	ratio = (float)P / M;
	// 限制比例在0.0-1.0范围内，防止指针超出仪表范围
	if (ratio < 0.0f)
		ratio = 0.0f;
	if (ratio > 1.0f)
		ratio = 1.0f;
	// 计算角度（弧度）：从12点位置开始顺时针计算
	// 0对应0弧度（12点位置），1对应2π弧度（回到12点位置）
	angle_rad = ratio * 2.0f * 3.14159265358979f;
	angle_rad += 3.14159265358979f / 2.0f;
	// 计算终点坐标
	endX = CENTER_X + (int)((float)RADIUS * cos(angle_rad));
	endY = CENTER_Y - (int)((float)RADIUS * sin(angle_rad)); // 减号因为屏幕Y轴通常向下递增
	// 使用Bresenham算法绘制指针
	Placeline(CENTER_Y, CENTER_X, endY, endX);
}

// 填充圆算法
void FillCircle(short x, short y, short R)
{
	int radius = R;
	int radius2 = R * R, dx, dy, i; // 半径的平方

	// 从圆心开始，沿着Y轴向上和向下扫描
	for (dy = 0; dy <= radius; dy++)
	{
		// 计算当前Y坐标下的X范围
		dx = (int)sqrt(radius2 - dy * dy);

		// 填充当前行的像素点
		for (i = x - dx; i <= x + dx; i++)
		{
			PlacePixel((u8)i, (u8)(y + dy)); // 上半部分
			PlacePixel((u8)i, (u8)(y - dy)); // 下半部分
		}
	}
}

// 绘制波形
// plot 1点图 0折线图    tongdao 通道选择：0-A0 1-A1 2-A2 3-A3
// pianyi 扫描数据位置偏移 可偏移的范围： 0-302 (时间轴) 大数接近现在，小数接近以前
// chufa 触发电平： 若
// void PlotWave(char plot,char tongdao,unsigned short pianyi,short chufa)
//{//AD_Value中的数据是 4个引脚电压为一组 A0 A1 A2 A3  A0 A1 A2 A3......
//     //Vector Mode 折线
//	unsigned short i;
//	for(i=0;i<400;i++)
//	{ADbuf[i]=AD_Value[i*4+tongdao];}
//     if (plot == 0)
//     {
//         for (i=0; i < 98; i++)//98是画框长度
//         {//86是使4096刚好在画框宽度范围内
//             Placeline ( ADbuf[(i+pianyi)]/86,i , ADbuf[(i+1+pianyi)]/86,i+1);
//         }
//     }
//     //Dots Mode 点图
//     if (plot == 1)
//     {
//         for (i = 0; i <= 98; i++)
//         {
//             PlacePixel(  ADbuf[(i+pianyi)]/86 , i);
//         }
//     }
// }
