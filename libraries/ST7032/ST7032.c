/* FILE NAME: ST7032.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

/*
	ST7032 LCD panel driver
	Original source is Arduino LiquidCrystal liblary

 ------------------------
  Lazurite(3V)        ST7032i
 ------------------------
  3.3V    --+-- VDD
            +-- -RES
  A4(SDA) --*-- SDA
  A5(SCL) --*-- SCL
  GND     ----- GND
  
  *... 10Kohm pull-up
 ------------------------
*/

#include "Lazurite.h"
#include "ST7032.h"
#include <Wire.h>

// public methods
void st7032_init();
void st7032_begin(uint8_t cols, uint8_t lines, uint8_t dotsize);
void st7032_setContrast(uint8_t cont);
void st7032_setIcon(uint8_t addr, uint8_t bit);
/********** high level commands, for the user! */
void st7032_clear();
void st7032_home();
void st7032_setCursor(uint8_t col, uint8_t row);
// Turn the display on/off (quickly)
void st7032_noDisplay(void);
void st7032_display();
// Turns the underline cursor on/off
void st7032_noCursor();
void st7032_cursor();
// Turn on and off the blinking cursor
void st7032_noBlink();
void st7032_blink();
// These commands scroll the display without changing the RAM
void st7032_scrollDisplayLeft(void);
void st7032_scrollDisplayRight(void);

// This is for text that flows Left to Right
void st7032_leftToRight(void);
// This is for text that flows Right to Left
void st7032_rightToLeft(void);

// This will 'right justify' text from the cursor
void st7032_autoscroll(void);

// This will 'left justify' text from the cursor
void st7032_noAutoscroll(void);

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void st7032_createChar(uint8_t location, uint8_t charmap[]);
/*********** mid level commands, for sending data/cmds */

void st7032_command(uint8_t value);
size_t st7032_write(uint8_t value);
size_t st7032_print(uint8_t *value);


const ST7032 lcd={
	st7032_init,
	st7032_begin,

    st7032_setContrast,
    st7032_setIcon,
    st7032_clear,
    st7032_home,

    st7032_noDisplay,
    st7032_display,
    st7032_noBlink,
    st7032_blink,
    st7032_noCursor,
    st7032_cursor,
    st7032_scrollDisplayLeft,
    st7032_scrollDisplayRight,
    st7032_leftToRight,
    st7032_rightToLeft,
    st7032_autoscroll,
    st7032_noAutoscroll,

    st7032_createChar,
    st7032_setCursor,
    st7032_write,
    st7032_command,
	st7032_print
};
static void st7032_setDisplayControl(uint8_t setBit);
static void st7032_resetDisplayControl(uint8_t resetBit);
static void st7032_setEntryMode(uint8_t setBit);
static void st7032_resetEntryMode(uint8_t resetBit);
static void st7032_normalFunctionSet();
static void st7032_extendFunctionSet();

//  void send(uint8_t, uint8_t);
/*
    uint8_t _rs_pin; // LOW: command.   HIGH: character.
    uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
    uint8_t _enable_pin; // activated by a HIGH pulse.
    uint8_t _data_pins[8];
*/
static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;
//  uint8_t _iconfunction;

static uint8_t _initialized;

static uint8_t _numlines;
static uint8_t _currline;
	
static uint8_t _i2c_addr;

// private methods

void st7032_setDisplayControl(uint8_t setBit) {
  _displaycontrol |= setBit;
  st7032_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void st7032_resetDisplayControl(uint8_t resetBit) {
  _displaycontrol &= ~resetBit;
  st7032_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void st7032_setEntryMode(uint8_t setBit) {
  _displaymode |= setBit;
  st7032_command(LCD_ENTRYMODESET | _displaymode);
}

void st7032_resetEntryMode(uint8_t resetBit) {
  _displaymode &= ~resetBit;
  st7032_command(LCD_ENTRYMODESET | _displaymode);
}

void st7032_normalFunctionSet(void) {
  st7032_command(LCD_FUNCTIONSET | _displayfunction);
}

void st7032_extendFunctionSet() {
  st7032_command(LCD_FUNCTIONSET | _displayfunction | LCD_EX_INSTRUCTION);
}


// public methods


void st7032_init()
{
//  begin(16, 1);
	_displaycontrol = 0x00;
	_displaymode = 0x00;
	_i2c_addr = (uint8_t)0x3E;
}

void st7032_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {

  _displayfunction  = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }
  
  Wire.begin();
  delay(40);               // Wait time >40ms After VDD stable

  // finally, set # lines, font size, etc.
  st7032_normalFunctionSet();

  st7032_extendFunctionSet();
  st7032_command(LCD_EX_SETBIASOSC | LCD_BIAS_1_5 | LCD_OSC_183HZ);            // 1/5bias, OSC=183Hz@3.0V
  st7032_command(LCD_EX_FOLLOWERCONTROL | LCD_FOLLOWER_ON | LCD_RAB_2_00);     // internal follower circuit is turn on
  delay(200);                                       // Wait time >200ms (for power stable)
  st7032_normalFunctionSet();

  // turn the display on with no cursor or blinking default
//  display();
  _displaycontrol   = 0x00;//LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  st7032_setDisplayControl(LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);

  // clear it off
  st7032_clear();

  // Initialize to default text direction (for romance languages)
//  st7032_command(LCD_ENTRYMODESET | _displaymode);
  _displaymode      = 0x00;//LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  st7032_setEntryMode(LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
}

void st7032_setContrast(uint8_t cont)
{
  st7032_extendFunctionSet();
  st7032_command(LCD_EX_CONTRASTSETL | (cont & 0x0f));                     // Contrast set
  st7032_command(LCD_EX_POWICONCONTRASTH | LCD_ICON_ON | LCD_BOOST_ON | ((cont >> 4) & 0x03)); // Power, ICON, Contrast control
  st7032_normalFunctionSet();
}

void st7032_setIcon(uint8_t addr, uint8_t bit) {
  st7032_extendFunctionSet();
  st7032_command(LCD_EX_SETICONRAMADDR | (addr & 0x0f));                       // ICON address
  st7032_write(bit);
  st7032_normalFunctionSet();
}

/********** high level commands, for the user! */
void st7032_clear()
{
  st7032_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void st7032_home()
{
  st7032_command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void st7032_setCursor(uint8_t col, uint8_t row)
{
  const int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
	st7032_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void st7032_noDisplay() {
  st7032_resetDisplayControl(LCD_DISPLAYON);
}
void st7032_display() {
  st7032_setDisplayControl(LCD_DISPLAYON);
}

// Turns the underline cursor on/off
void st7032_noCursor() {
  st7032_resetDisplayControl(LCD_CURSORON);
}
void st7032_cursor() {
  st7032_setDisplayControl(LCD_CURSORON);
}

// Turn on and off the blinking cursor
void st7032_noBlink() {
  st7032_resetDisplayControl(LCD_BLINKON);
}
void st7032_blink() {
  st7032_setDisplayControl(LCD_BLINKON);
}

// These commands scroll the display without changing the RAM
void st7032_scrollDisplayLeft(void) {
  st7032_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void st7032_scrollDisplayRight(void) {
  st7032_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void st7032_leftToRight(void) {
  st7032_setEntryMode(LCD_ENTRYLEFT);
}

// This is for text that flows Right to Left
void st7032_rightToLeft(void) {
  st7032_resetEntryMode(LCD_ENTRYLEFT);
}

// This will 'right justify' text from the cursor
void st7032_autoscroll(void) {
  st7032_setEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// This will 'left justify' text from the cursor
void st7032_noAutoscroll(void) {
  st7032_resetEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void st7032_createChar(uint8_t location, uint8_t charmap[]) {
	int i;
  location &= 0x7; // we only have 8 locations 0-7
  st7032_command(LCD_SETCGRAMADDR | (location << 3));
  for (i=0; i<8; i++) {
    st7032_write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

void st7032_command(uint8_t value) {
  Wire.beginTransmission(_i2c_addr);
  Wire.write_byte((uint8_t)0x00);
  Wire.write_byte(value);
  Wire.endTransmission(true);
  delayMicroseconds(27);    // >26.3us
}

size_t st7032_write(uint8_t value) {
  Wire.beginTransmission(_i2c_addr);
  Wire.write_byte((uint8_t)0x40);
  Wire.write_byte(value);
  Wire.endTransmission(true);
  delayMicroseconds(27);    // >26.3us

  return 1;
}
size_t st7032_print(uint8_t *value) {
	int i=0;
	while(value[i]!=0)
	{
		Wire.beginTransmission(_i2c_addr);
		Wire.write_byte((uint8_t)0x40);
		Wire.write_byte(value[i]);
		Wire.endTransmission(true);
		delayMicroseconds(27);    // >26.3us
		i++;
	}

  return 1;
}

