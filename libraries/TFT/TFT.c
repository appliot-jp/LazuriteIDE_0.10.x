/* FILE NAME: TFT.c
 *
 * Modifided Copyright (c) 2017  Lapis Semiconductor Co.,Ltd.
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

/* Copyright (c) 2013, Enrico Gueli <enrico.gueli@gmail.com> All
rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "TFT.h"
#include "SPI.h"
#include "glcdfont.c"

//********************************************************************************
//   global parameters
//********************************************************************************
void begin(void);
void initR(uint8_t options);
void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void pushColor(uint16_t color);
void drawPixel(int16_t x, int16_t y, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void fillScreen(uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void setRotation(uint8_t m);
void invertDisplay(boolean i);
uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
size_t write(uint8_t c);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void setCursor(int16_t x, int16_t y);
void setTextSize(uint8_t s);
void setTextColor(uint16_t c);
void setTextWrap(boolean w);
uint8_t getRotation(void);
int16_t width(void);
int16_t height(void);
uint16_t newColor(uint8_t r, uint8_t g, uint8_t b);
void background(uint8_t red, uint8_t green, uint8_t blue);
void stroke(uint8_t red, uint8_t green, uint8_t blue);
void noStroke();
void noFill();
void fill(uint8_t red, uint8_t green, uint8_t blue);
void text(const char * text, int16_t x, int16_t y);
void textWrap(const char * text, int16_t x, int16_t y);
void textSize(uint8_t size);
void point(int16_t x, int16_t y);
void line(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void rect(int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius);
void circle(int16_t x, int16_t y, int16_t r);
void triangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3);

const t_TFT tft = {
	begin,
	initR,					// for ST7735R
	setAddrWindow,
	pushColor,
	fillScreen,
	drawPixel,
	drawFastVLine,
	drawFastHLine,
	fillRect,
	setRotation,
	invertDisplay,
	Color565,
	drawLine,
	drawCircle,
	drawCircleHelper,
	fillCircle,
	fillCircleHelper,
	drawTriangle,
	fillTriangle,
	drawRoundRect,
	fillRoundRect,
	drawBitmap,
	drawChar,
	setCursor,
	setTextColor,
	setTextSize,
	setTextWrap,
	write,
	height,
	width,
	getRotation,
	newColor,
	background,
	fill,
	noFill,
	stroke,
	noStroke,
	text,
	textWrap,
	textSize,
	circle,
	point,
	line,
	rect,
	triangle
};

//********************************************************************************
//   local definitions
//********************************************************************************
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

//********************************************************************************
//   local parameters
//********************************************************************************
uint8_t tabcolor;
uint8_t _cs, _rs, _rst, colstart, rowstart; // some displays need this changed
int16_t WIDTH, HEIGHT;		// this is the 'raw' display w/h - never changes
int16_t _width, _height;	// dependent on rotation
int16_t cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t textsize, rotation;
boolean wrap;				// If set, 'wrap' text at right edge of display
/* Processing-style graphics state */
color strokeColor;
bool useStroke;
color fillColor;
bool useFill;

//********************************************************************************
//   local function definitions
//********************************************************************************
uint16_t swapcolor(uint16_t x);
void Adafruit_ST7735(uint8_t cs, uint8_t rs, uint8_t rst);
void spiwrite(uint8_t c);
void writecommand(uint8_t c);
void writedata(uint8_t c);
void commandList(const uint8_t *addr);
void commonInit(const uint8_t *cmdList);
void Adafruit_GFX(int16_t w, int16_t h);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
size_t write_print(const uint8_t *buffer, size_t size);
void print_text(const char str[]);

//********************************************************************************
//   local functions
//********************************************************************************
t_TFT TFT(uint8_t CS, uint8_t RS, uint8_t RST)
{
	Adafruit_ST7735(CS, RS, RST);
	// as we already know the orientation (landscape, therefore rotated),
	// set default width and height without need to call begin() first.
	_width = ST7735_TFTHEIGHT;
	_height = ST7735_TFTWIDTH;
	
	return tft;
}

void begin(void) {
	initR(INITR_BLACKTAB);
	setRotation(1);
}

uint16_t swapcolor(uint16_t x) { 
	return (x << 11) | (x & 0x07E0) | (x >> 11);
}

void Adafruit_ST7735(uint8_t cs, uint8_t rs, uint8_t rst)
{
	Adafruit_GFX(ST7735_TFTWIDTH, ST7735_TFTHEIGHT);
	
	_cs   = cs;
	_rs   = rs;
	_rst  = rst;
}

void spiwrite(uint8_t c)
{
	SPI.transfer(c);
}

void writecommand(uint8_t c) 
{
	digitalWrite(_rs, LOW);
	digitalWrite(_cs, LOW);
	spiwrite(c);
	digitalWrite(_cs, HIGH);
}

void writedata(uint8_t c) 
{
	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);
	spiwrite(c);
	digitalWrite(_cs, HIGH);
}

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
//PROGMEM const static unsigned char
const static unsigned char
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay

  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 },      //     XEND = 159
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F },           //     XEND = 159

  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 },                  //     100 ms delay
  Gcmd[] = {                  // Initialization commands for 7735B screens
    19,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      100,                    //     255 = 500 ms delay
    0x26 , 1,                 // 3: Set default gamma
      0x04,                   //     16-bit color
    0xb1, 2,                  // 4: Frame Rate
      0x0b,
      0x14,
    0xc0, 2,                  // 5: VRH1[4:0] & VC[2:0]
      0x08,
      0x00,
    0xc1, 1,                  // 6: BT[2:0]
      0x05,
    0xc5, 2,                  // 7: VMH[6:0] & VML[6:0]
      0x41,
      0x30,
    0xc7, 1,                  // 8: LCD Driving control
      0xc1,
    0xEC, 1,                  // 9: Set pumping color freq
      0x1b,
    0x3a , 1 + DELAY,         // 10: Set color format
      0x55,                   //     16-bit color
      100,
    0x2a, 4,                  // 11: Set Column Address
      0x00,
      0x00,
      0x00,
      0x7f,
    0x2b, 4,                  // 12: Set Page Address
      0x00,
      0x00,
      0x00,
      0x9f,
    0x36, 1,                  // 12+1: Set Scanning Direction
      0xc8,
    0xb7, 1,                  // 14: Set Source Output Direciton
      0x00,
    0xf2, 1,                  // 15: Enable Gamma bit
      0x00,
    0xe0, 15 + DELAY,         // 16: magic
      0x28, 0x24, 0x22, 0x31,
      0x2b, 0x0e, 0x53, 0xa5,
      0x42, 0x16, 0x18, 0x12,
      0x1a, 0x14, 0x03,
      50,
    0xe1, 15 + DELAY,         // 17: more magic
      0x17, 0x1b, 0x1d, 0x0e,
      0x14, 0x11, 0x2c, 0xa5,
      0x3d, 0x09, 0x27, 0x2d,
      0x25, 0x2b, 0x3c, 
      50, 
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 };                  //     255 = 500 ms delay


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void commandList(const uint8_t *addr) 
{
	uint8_t  numCommands, numArgs;
	uint16_t ms;

	numCommands = pgm_read_byte(addr++);   // Number of commands to follow
	while(numCommands--) {                 // For each command...
		writecommand(pgm_read_byte(addr++)); //   Read, issue command
		numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
		ms       = numArgs & DELAY;          //   If hibit set, delay follows args
		numArgs &= ~DELAY;                   //   Mask out delay bit
		while(numArgs--) {                   //   For each argument...
			writedata(pgm_read_byte(addr++));  //     Read, issue argument
		}
		if(ms) {
			ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
			if(ms == 255) ms = 500;     // If 255, delay for 500 ms
			delay(ms);
		}
	}
}

// Initialization code common to both 'B' and 'R' type displays
void commonInit(const uint8_t *cmdList) 
{
	colstart  = rowstart = 0; // May be overridden in init func

	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setBitOrder(MSBFIRST);
	SPI.setBitOrder(SPI_MSBFIRST);
	SPI.setDataMode(SPI_MODE0);

	// toggle RST low to reset; CS low so it'll listen to us
	digitalWrite(_cs, LOW);
	if (_rst) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(500);
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
		delay(500);
	}
	if(cmdList) commandList(cmdList);
}

// Initialization for ST7735R screens (green or red tabs)
void initR(uint8_t options) {
	commonInit(Rcmd1);
	if(options == INITR_GREENTAB) {
		commandList(Rcmd2green);
		colstart = 2;
		rowstart = 1;
	} else {
		// colstart, rowstart left at default '0' values
		commandList(Rcmd2red);
	}
	commandList(Rcmd3);
	tabcolor = options;
}

void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) 
{
	writecommand(ST7735_CASET);				// Column addr set
	writedata(0x00);
	writedata((uint8_t)(x0+colstart));		// XSTART 
	writedata(0x00);
	writedata((uint8_t)(x1+colstart));		// XEND

	writecommand(ST7735_RASET);				// Row addr set
	writedata(0x00);
	writedata((uint8_t)(y0+rowstart));		// YSTART
	writedata(0x00);
	writedata((uint8_t)(y1+rowstart));		// YEND

	writecommand(ST7735_RAMWR);				// write to RAM
}

void pushColor(uint16_t color) 
{
	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);

	if (tabcolor == INITR_BLACKTAB)   color = swapcolor(color);
	spiwrite((uint8_t)(color >> 8));
	spiwrite((uint8_t)(color));

	digitalWrite(_cs, HIGH);
}

void drawPixel(int16_t x, int16_t y, uint16_t color) 
{
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) {
		return;
	}
	setAddrWindow((uint8_t)x, (uint8_t)y, (uint8_t)(x+1), (uint8_t)(y+1));
	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);
	if (tabcolor == INITR_BLACKTAB) {
		color = swapcolor(color);
	}
	spiwrite((uint8_t)(color >> 8));
	spiwrite((uint8_t)color);
	digitalWrite(_cs, HIGH);
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	uint8_t hi,lo;
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) {
		return;
	}
	if((y+h-1) >= _height) {
		h = _height-y;
	}
	setAddrWindow((uint8_t)x, (uint8_t)y, (uint8_t)x, (uint8_t)(y+h-1));
	
	if (tabcolor == INITR_BLACKTAB) {
		color = swapcolor(color);
	}
	hi = (uint8_t)(color >> 8);
	lo = (uint8_t)color;
	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);
	while (h--) {
		spiwrite(hi);
		spiwrite(lo);
	}
	digitalWrite(_cs, HIGH);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	uint8_t hi, lo;
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) {
		return;
	}
	if((x+w-1) >= _width)  {
		w = _width-x;
	}
	setAddrWindow((uint8_t)x, (uint8_t)y, (uint8_t)(x+w-1), (uint8_t)y);
	if (tabcolor == INITR_BLACKTAB) {
		color = swapcolor(color);
	}
	hi = (uint8_t)(color >> 8);
	lo = (uint8_t)color;

	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);
	while (w--) {
		spiwrite(hi);
		spiwrite(lo);
	}
	digitalWrite(_cs, HIGH);
}

void fillScreen(uint16_t color) {
	fillRect(0, 0,  _width, _height, color);
}

// fill a rectangle
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
{
	uint8_t hi, lo;
	// rudimentary clipping (drawChar w/big text requires this)
	if((x >= _width) || (y >= _height)) {
		return;
	}
	if((x + w - 1) >= _width) {
		w = _width  - x;
	}
	if((y + h - 1) >= _height) {
		h = _height - y;
	}
	if (tabcolor == INITR_BLACKTAB) {
		color = swapcolor(color);
	}
	setAddrWindow((uint8_t)x, (uint8_t)y, (uint8_t)(x+w-1), (uint8_t)(y+h-1));

	hi = (uint8_t)(color >> 8);
	lo = (uint8_t)color;

	digitalWrite(_rs, HIGH);
	digitalWrite(_cs, LOW);
  
	for(y=h; y>0; y--) {
		for(x=w; x>0; x--) {
			spiwrite(hi);
			spiwrite(lo);
		}
	}
	digitalWrite(_cs, HIGH);
}

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_MH  0x04

void setRotation(uint8_t m)
{
	writecommand(ST7735_MADCTL);
	rotation = (uint8_t)(m % 4); // can't be higher than 3
	switch (rotation) {
	case 0:
		writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
		_width  = ST7735_TFTWIDTH;
		_height = ST7735_TFTHEIGHT;
		break;
	case 1:
		writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
		_width  = ST7735_TFTHEIGHT;
		_height = ST7735_TFTWIDTH;
		break;
	case 2:
		writedata(MADCTL_RGB);
		_width  = ST7735_TFTWIDTH;
		_height = ST7735_TFTHEIGHT;
		break;
	case 3:
		writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
		_width  = ST7735_TFTHEIGHT;
		_height = ST7735_TFTWIDTH;
		break;
	default:
		break;
	}
}

void invertDisplay(boolean i)
{
	writecommand((uint8_t)(i ? ST7735_INVON : ST7735_INVOFF));
}

uint16_t Color565(uint8_t r, uint8_t g, uint8_t b)
{
	return newColor(r, g, b);
}

void Adafruit_GFX(int16_t w, int16_t h)
{
	WIDTH = w;
	HEIGHT = h;
	
	_width    = WIDTH;
	_height   = HEIGHT;
	rotation  = 0;
	cursor_y  = cursor_x    = 0;
	textsize  = 1;
	textcolor = textbgcolor = 0xFFFF;
	wrap = true;
}

// draw a circle outline
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) 
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawPixel(x0, y0+r, color);
	drawPixel(x0, y0-r, color);
	drawPixel(x0+r, y0, color);
	drawPixel(x0-r, y0, color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);
	}
}

void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			drawPixel(x0 + x, y0 + y, color);
			drawPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			drawPixel(x0 + x, y0 - y, color);
			drawPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			drawPixel(x0 - y, y0 + x, color);
			drawPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			drawPixel(x0 - y, y0 - x, color);
			drawPixel(x0 - x, y0 - y, color);
		}
	}
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	drawFastVLine(x0, y0-r, 2*r+1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

// used to do circles and roundrects
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
				    uint8_t cornername, int16_t delta, uint16_t color)
{	
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x1) {
			drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2) {
			drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}

// Bresenham's algorithm - thx wikpedia
void drawLine(int16_t x0, int16_t y0, 
			    int16_t x1, int16_t y1, 
			    uint16_t color) 
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	int16_t dx, dy;
	int16_t err;
	int16_t ystep;

	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	dx = x1 - x0;
	dy = abs(y1 - y0);
	err = dx / 2;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}
	for (; x0<=x1; x0++) {
		if (steep) {
			drawPixel(y0, x0, color);
		} else {
			drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}


// Draw a rectangle
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	drawFastHLine(x, y, w, color);
	drawFastHLine(x, y+h-1, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x+w-1, y, h, color);
}

// Draw a rounded rectangle
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	drawFastHLine(x+r  , y    , w-2*r, color); // Top
	drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
	drawFastVLine(x    , y+r  , h-2*r, color); // Left
	drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
	// draw four corners
	drawCircleHelper(x+r    , y+r    , r, 1, color);
	drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
	drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	fillRect(x+r, y, w-2*r, h, color);
	// draw four corners
	fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
void drawTriangle(int16_t x0, int16_t y0,
				int16_t x1, int16_t y1, 
				int16_t x2, int16_t y2, uint16_t color) 
{
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void fillTriangle ( int16_t x0, int16_t y0,
				  int16_t x1, int16_t y1,
				  int16_t x2, int16_t y2, uint16_t color)
{
	int16_t a, b, y, last;
	int16_t dx01, dy01, dx02, dy02, dx12, dy12, sa, sb;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1); swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}

	if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if(x1 < a)      a = x1;
		else if(x1 > b) b = x1;
		if(x2 < a)      a = x2;
		else if(x2 > b) b = x2;
		drawFastHLine(a, y0, b-a+1, color);
		return;
	}

	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
	if(y1 == y2) last = y1;   // Include y1 scanline
	else         last = y1-1; // Skip it

	for(y=y0; y<=last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b) swap(a,b);
		drawFastHLine(a, y, b-a+1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if(a > b) swap(a,b);
		drawFastHLine(a, y, b-a+1, color);
	}
}

void drawBitmap(int16_t x, int16_t y, 
				const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) 
{
	int16_t i, j, byteWidth = (w + 7) / 8;
	for(j=0; j<h; j++) {
		for(i=0; i<w; i++ ) {
			if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x+i, y+j, color);
			}
		}
	}
}

size_t write(uint8_t c) {
	if (c == '\n') {
		cursor_y += textsize*8;
		cursor_x = 0;
	} else if (c == '\r') {
		// skip em
	} else {
		drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
		cursor_x += textsize*6;
		if (wrap && (cursor_x > (_width - textsize*6))) {
			cursor_y += textsize*8;
			cursor_x = 0;
		}
	}
	return 1;
}

// draw a character
void drawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t color, uint16_t bg, uint8_t size) 
{
	int8_t i,j;
	if((x >= _width)				|| // Clip right
		(y >= _height)				|| // Clip bottom
		((x + 6 * size - 1) < 0)	|| // Clip left
		((y + 8 * size - 1) < 0))     // Clip top
		return;

	for (i=0; i<6; i++ ) {
		uint8_t line;
		if (i == 5) {
			line = 0x0;
		} else {
			line = pgm_read_byte(font+(c*5)+i);
		}
		for (j = 0; j<8; j++) {
			if (line & 0x1) {
				if (size == 1) { // default size
					drawPixel(x+i, y+j, color);
				} else {  // big size
					fillRect(x+(i*size), y+(j*size), size, size, color);
				} 
			} else if (bg != color) {
				if (size == 1) {// default size
					drawPixel(x+i, y+j, bg);
				} else {  // big size
					fillRect(x+i*size, y+j*size, size, size, bg);
				}
			}
			line >>= 1;
		}
	}
}

void setCursor(int16_t x, int16_t y) 
{
	cursor_x = x;
	cursor_y = y;
}


void setTextSize(uint8_t s) 
{
	textsize = (uint8_t)((s > 0) ? s : 1);
}


void setTextColor(uint16_t c) {
	textcolor = c;
	textbgcolor = c; 
	// for 'transparent' background, we'll set the bg 
	// to the same as fg instead of using a flag
}

void setTextWrap(boolean w)
{
	wrap = w;
}

uint8_t getRotation(void)
{
	return rotation;
}

// return the size of the display (per current rotation)
int16_t width(void) 
{ 
	return _width; 
}
 
int16_t height(void) 
{ 
	return _height; 
}

uint16_t newColor(uint8_t r, uint8_t g, uint8_t b) 
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void background(uint8_t red, uint8_t green, uint8_t blue)
{
	fillScreen(newColor(red, green, blue));
}

void stroke(uint8_t red, uint8_t green, uint8_t blue)
{
	useStroke = true;
	strokeColor = newColor(red, green, blue);
	setTextColor(strokeColor);
}

void noStroke()
{
	useStroke = false;
}

void noFill(void)
{
	useFill = false;
}

void fill(uint8_t red, uint8_t green, uint8_t blue)
{
	useFill = true;
	fillColor = newColor(red, green, blue);
}

void text(const char * text, int16_t x, int16_t y)
{
	if (!useStroke){
		return;
	}
	setTextWrap(false);
	setTextColor(strokeColor);
	setCursor(x, y);
	print_text(text);
}

void textWrap(const char * text, int16_t x, int16_t y) 
{
	if (!useStroke){
		return;
	}
	setTextWrap(true);
	setTextColor(strokeColor);
	setCursor(x, y);
	print_text(text);
}

void textSize(uint8_t size)
{
	setTextSize(size);
}

void point(int16_t x, int16_t y)
{
	if (!useStroke){
		return;
	}
	drawPixel(x, y, strokeColor);
}

void line(int16_t x1, int16_t y1, int16_t x2, int16_t y2) 
{
	if (!useStroke) {
		return;
	}
	if (x1 == x2) {
		if (y1 < y2) {
			drawFastVLine(x1, y1, y2 - y1, strokeColor);
		} else {
			drawFastVLine(x1, y2, y1 - y2, strokeColor);
		}
	} else if (y1 == y2) {
		if (x1 < x2) {
			drawFastHLine(x1, y1, x2 - x1, strokeColor);
		} else {
			drawFastHLine(x2, y1, x1 - x2, strokeColor);
		}
	} else {
		drawLine(x1, y1, x2, y2, strokeColor);
	}
}

void rect(int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius) 
{
	if (radius == 0) {
		if (useFill) {
			fillRect(x, y, width, height, fillColor);
		}
		if (useStroke) {
			drawRect(x, y, width, height, strokeColor);
		}
	}
	if (useFill) {
		fillRoundRect(x, y, width, height, radius, fillColor);
	}
	if (useStroke) {
		drawRoundRect(x, y, width, height, radius, strokeColor);
	}
}

void circle(int16_t x, int16_t y, int16_t r) 
{
	if (r == 0) {
		return;
	}
	if (useFill) {
		fillCircle(x, y, r, fillColor);
	}
	if (useStroke) {
		drawCircle(x, y, r, strokeColor);
	}
}

void triangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3) 
{
	if (useFill) {
		fillTriangle(x1, y1, x2, y2, x3, y3, fillColor);
	}
	if (useStroke) {
		drawTriangle(x1, y1, x2, y2, x3, y3, strokeColor);
	}
}

size_t write_print(const uint8_t *buffer, size_t size)
{
	size_t n = 0;
	while (size--) {
		if (write(*buffer++)) {
			n++;
		} else {
			break;
		}
	}
	return n;
}

void print_text(const char str[])
{
	write_print(str, strlen(str));
}

