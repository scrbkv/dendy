#ifndef __LCD_H
#define __LCD_H

#include "main.h"

#define TFT_STRING_MODE_NO_BACKGROUND		0x01
#define TFT_STRING_MODE_BACKGROUND			0x00

void LcdInit();
void DrawPixel(uint16_t x,uint16_t y,uint16_t color);
void SetWindow(uint16_t xStart,uint16_t yStart,uint16_t xEnd, uint16_t yEnd);
void ClearScreen(uint16_t color);

void Puts26x48(uint16_t x, uint16_t y, uint8_t *string, uint8_t TFT_STRING_MODE);
void Puts18x32(uint16_t x, uint16_t y, uint8_t *string, uint8_t TFT_STRING_MODE);
void Puts14x24(uint16_t x, uint16_t y, uint8_t *string,uint8_t TFT_STRING_MODE);
void Puts8x16(uint16_t x, uint16_t y, uint8_t *string, uint8_t TFT_STRING_MODE);

#endif
