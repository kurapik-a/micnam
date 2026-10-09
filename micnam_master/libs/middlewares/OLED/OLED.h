#ifndef __OLED_H
#define __OLED_H
#include "bsp.h"

//∑∂Œß…Ë÷√
#define zuoshang 0,0
#define miny 0
#define minx 0
#define youxia 63,127
#define maxy 63
#define maxx 127
//…Í√˜œ‘¥Ê
extern u8 bufdis[(maxy+1)*(maxx+1)/8];

void OLED_I2C_Init(void);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t Data);
void I2C_WriteNbyte(uint8_t slave_addr, uint8_t a, uint8_t *b, uint16_t len);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void Char_mini(u8 x,u8 y,u8 chr);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowString_mini(uint8_t Line, uint8_t Column, char *String);
uint32_t OLED_Pow(uint32_t X, long Y);
void printf_oled(char a,char b,char *stringg, ...);
void printf_oled_mini(char a,char b,char *stringg, ...);
void AreaFill(uint8_t y1, uint8_t x1,uint8_t y2, uint8_t x2);
void huakuang(uint8_t y1, uint8_t x1,uint8_t y2, uint8_t x2);
void OLED_Init(void);
void Placedashed_line(char y);
void PlaceVerticalline(char x);
void Clearbuffer(void);
void Clearbuffer_bc(u8 y);
void OLED_Display(void);
void PlacePixel(uint8_t y, uint8_t x);
void PlacePixel_back(uint8_t y, uint8_t x);
void Placecharbit(uint8_t y, uint8_t x,uint8_t cha);
void Placechar(uint8_t y, uint8_t x,uint8_t chr);
void PlaceString(uint8_t Line, uint8_t Column, char *String);
void Placechar_F8x16(uint8_t x, uint8_t y,uint8_t chr);
void Placechar_F8x16_back(uint8_t x, uint8_t y,uint8_t chr);
void PlaceString_F8x16(uint8_t Line, uint8_t Column, char *String);
void PlaceString_F8x16_back(uint8_t Line, uint8_t Column, char *String);
void printf_Place(char a,char b,char *stringg, ...);
void printf_Place_F8x16(char a,char b,char *stringg, ...);
void printf_Place_F8x16_back(char a,char b,char *stringg, ...);
void PlaceChinese_Fc32(uint8_t x, uint8_t y,uint8_t chr);
void PlaceChinese_Fc32_back(uint8_t x, uint8_t y,uint8_t chr);
short absx(short x) ;
void Placeline(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2) ;
void Placeline_back(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2) ;
void rectangle(uint8_t y1, uint8_t x1,uint8_t y2, uint8_t  x2);
void solid_rectangle(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2);
void rectangle_back(uint8_t y1, uint8_t x1,uint8_t y2, uint8_t  x2);
void PlaceCyuan(short y1, short x1,float R);
void DrawGaugeNeedle(uint8_t CENTER_X, uint8_t CENTER_Y, uint8_t RADIUS, uint16_t P, uint16_t M) ;
void FillCircle(short x, short y, short R) ;


#endif
