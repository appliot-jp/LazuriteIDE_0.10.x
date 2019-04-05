/* FILE NAME: TFT.h
 *
 * Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _LAZURITE_TFT_H
#define _LAZURITE_TFT_H

#include "lazurite.h"

//********************************************************************************
//   global parameters
//********************************************************************************
typedef uint16_t color;
typedef struct {
	/* TFT public */
	void (*begin)(void);
	void (*initR)(uint8_t options); // for ST7735R
	void (*setAddrWindow)(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void (*pushColor)(uint16_t color);
	void (*fillScreen)(uint16_t color);
	void (*drawPixel)(int16_t x, int16_t y, uint16_t color);
	void (*drawFastVLine)(int16_t x, int16_t y, int16_t h, uint16_t color);
	void (*drawFastHLine)(int16_t x, int16_t y, int16_t w, uint16_t color);
	void (*fillRect)(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void (*setRotation)(uint8_t r);
	void (*invertDisplay)(boolean i);
	uint16_t (*Color565)(uint8_t r, uint8_t g, uint8_t b);
	void (*drawLine)(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void (*drawCircle)(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void (*drawCircleHelper)(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
	void (*fillCircle)(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void (*fillCircleHelper)(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
	void (*drawTriangle)(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void (*fillTriangle)(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void (*drawRoundRect)(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void (*fillRoundRect)(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void (*drawBitmap)(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
	void (*drawChar)(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
	void (*setCursor)(int16_t x, int16_t y);
	void (*setTextColor)(uint16_t c);
	void (*setTextSize)(uint8_t s);
	void (*setTextWrap)(boolean w);
	size_t (*write)(uint8_t);
	int16_t (*height)(void);
	int16_t (*width)(void);
	uint8_t (*getRotation)(void);
	/* Processing-like graphics primitives   */
	uint16_t (*newColor)(uint8_t red, uint8_t green, uint8_t blue);
	void (*background)(uint8_t red, uint8_t green, uint8_t blue);
	void (*fill)(uint8_t red, uint8_t green, uint8_t blue);
	void (*noFill)();
	void (*stroke)(uint8_t red, uint8_t green, uint8_t blue);
	void (*noStroke)();
	void (*text)(const char * text, int16_t x, int16_t y);
	void (*textWrap)(const char * text, int16_t x, int16_t y);
	void (*textSize)(uint8_t size);
	void (*circle)(int16_t x, int16_t y, int16_t r);
	void (*point)(int16_t x, int16_t y);
	void (*line)(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	void (*rect)(int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius);
	void (*triangle)(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3);
} t_TFT;

t_TFT TFT(uint8_t CS, uint8_t RS, uint8_t RST);

//********************************************************************************
//   extern function definitions
//********************************************************************************
extern const t_TFT tft;

//********************************************************************************
//   global definitions
//********************************************************************************
#define swap(a, b) { int16_t t = a; a = b; b = t; }

// some flags for initR()
#define INITR_GREENTAB   0x0
#define INITR_REDTAB     0x1
#define INITR_BLACKTAB   0x2

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

#endif // _LAZURITE_TFT_H
