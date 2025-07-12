/*
 * ILI9341.h
 *
 *  Created on: Jun 25, 2025
 *      Author: Semi Eren Gökyıldız
 */
#ifndef INC_ILI9341_H_
#define INC_ILI9341_H_
//-------------------------------------------------------------------
#include "main.h"
#include "fonts.h"
#include <stdio.h>   // vsnprintf için
#include <stdarg.h>  // va_list için
#include <string.h>  // strlen için
//-------------------------------------------------------------------
typedef struct{
	GPIO_TypeDef* port;
	uint16_t pin;
}Gpio_Pin;

typedef struct
{
	uint16_t TextColor;
	uint16_t BackColor;
	sFONT *pFont;
}LCD_DrawPropTypeDef;

typedef struct{
	Gpio_Pin RST;
	Gpio_Pin CS;
	Gpio_Pin DC;
	SPI_HandleTypeDef* hspi;
	uint16_t w_size;
	uint16_t h_size;
	LCD_DrawPropTypeDef lcdprop;
}ILI9341_HandleTypeDef;
//-------------------------------------------------------------------
#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF

#define ILI9341_WIDTH  		240
#define ILI9341_HEIGHT		320
//-------------------------------------------------------------------
void ILI9341_SetAddrWindow(ILI9341_HandleTypeDef* hili9341, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ILI9341_FillRect(ILI9341_HandleTypeDef* hili9341, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ILI9341_FillScreen(ILI9341_HandleTypeDef* hili9341, uint16_t color);
void ILI9341_DrawPixel(ILI9341_HandleTypeDef* hili9341, int x, int y, uint16_t color);
void ILI9341_DrawLine(ILI9341_HandleTypeDef* hili9341, uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ILI9341_DrawRect(ILI9341_HandleTypeDef* hili9341, uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ILI9341_DrawCircle(ILI9341_HandleTypeDef* hili9341, uint16_t x0, uint16_t y0, int r, uint16_t color);
void ILI9341_SetTextColor(ILI9341_HandleTypeDef* hili9341, uint16_t color);
void ILI9341_SetBackColor(ILI9341_HandleTypeDef* hili9341,uint16_t color);
void ILI9341_SetFont(ILI9341_HandleTypeDef* hili9341, sFONT *pFonts);
void ILI9341_DrawChar(ILI9341_HandleTypeDef* hili9341, uint16_t x, uint16_t y, uint8_t c);
void ILI9341_String(ILI9341_HandleTypeDef* hili9341, uint16_t x,uint16_t y, char *str);
void ILI9341_SetRotation(ILI9341_HandleTypeDef* hili9341, uint8_t r);
void ILI9341_FontsInit(ILI9341_HandleTypeDef* hili9341);
void ILI9341_Printf(ILI9341_HandleTypeDef* hili9341, uint16_t x, uint16_t y, const char *fmt, ...);
void ILI9341_init(ILI9341_HandleTypeDef* hili9341);
//-------------------------------------------------------------------
#endif /* INC_ILI9341_H_ */
