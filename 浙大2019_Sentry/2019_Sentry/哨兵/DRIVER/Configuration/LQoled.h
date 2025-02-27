#ifndef __LQOLED_H__
#define __LQOLED_H__
#include "stm32f4xx_gpio.h"
#include "stm32f4xx.h"
#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		  64
#define	Brightness	0xCF

#define X_WIDTH 128
#define Y_WIDTH 64

#define OLED_D0H   GPIO_SetBits(GPIOD,GPIO_Pin_4)
#define OLED_D0L   GPIO_ResetBits(GPIOD,GPIO_Pin_4)
#define OLED_D1H   GPIO_SetBits(GPIOD,GPIO_Pin_3)
#define OLED_D1L   GPIO_ResetBits(GPIOD,GPIO_Pin_3)
#define OLED_RSTH  GPIO_SetBits(GPIOD,GPIO_Pin_1)
#define OLED_RSTL  GPIO_ResetBits(GPIOD,GPIO_Pin_1)
#define OLED_DCH   GPIO_SetBits(GPIOD,GPIO_Pin_0)
#define OLED_DCL   GPIO_ResetBits(GPIOD,GPIO_Pin_0)

void LCD_Init(void);
void LCD_CLS(void);
void LCD_Set_Pos(u8, u8 );
void LCD_WrDat(u8 );
void LCD_P6x8Str(u8 x,u8 y,u8 ch[]);
void LCD_P8x16Str(u8 x,u8 y,u8 ch[]);
void LCD_P14x16Str(u8 x,u8 y,u8 ch[]);
void LCD_Print(u8 x, u8 y, u8 ch[]);
void LCD_PutPixel(u8,u8);
void LCD_Rectangle(u8,u8,u8,u8,u8 );
void Draw_LQLogo(void);
void Draw_LibLogo(void);
void Draw_BMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 bmp[]);
void LCD_Fill(u8 );
void LCD_write_char(u8 X, u8 Y,u16 c);
void LCD_Show_Number (u8 X,u8 Y,int number);
void Delaynop(void);
void Delayms(u8 xms);

#endif
