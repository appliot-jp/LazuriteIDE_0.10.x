/*!
 *  @file Adafruit_PWMServoDriver.cpp
 *
 *  @mainpage Adafruit 16-channel PWM & Servo driver
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the 16-channel PWM & Servo driver.
 *
 *  Designed specifically to work with the Adafruit PWM & Servo driver.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/815
 *
 *  These displays use I2C to communicate, 2 pins are required to interface.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  @section license License
 *
 *  BSD license, all text above must be included in any redistribution
 */

#include "Adafruit_PWMServoDriver.h"
#include <Wire.h>

/*!
 *  @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on a
 * TwoWire interface
 *  @param  i2c  A pointer to a 'Wire' compatible object that we'll use to
 * communicate with
 *  @param  addr The 7-bit I2C address to locate this chip, default is 0x40
 */
static void Adafruit_PWMServoDriver_init(uint8_t addr);
static void Adafruit_PWMServoDriver_begin(uint8_t prescale);
static void Adafruit_PWMServoDriver_reset();
static void Adafruit_PWMServoDriver_sleep();
static void Adafruit_PWMServoDriver_wakeup();
static void	Adafruit_PWMServoDriver_setExtClk(uint8_t prescale);
static void	Adafruit_PWMServoDriver_setPWMFreq(float freq);
static uint8_t Adafruit_PWMServoDriver_getPWM(uint8_t num);
static void	Adafruit_PWMServoDriver_setPWM(uint8_t num, uint16_t on, uint16_t off);
static void	Adafruit_PWMServoDriver_setPin(uint8_t num, uint16_t val, bool invert);
static void Adafruit_PWMServoDriver_write8(uint8_t addr, uint8_t d);
static uint8_t Adafruit_PWMServoDriver_read8(uint8_t addr);
	
static uint8_t i2caddr;
void Adafruit_PWMServoDriver_init(uint8_t addr) {
  i2caddr = addr;
}

/*!
 *  @brief  Setups the I2C interface and hardware
 *  @param  prescale
 *          Sets External Clock (Optional)
 *          
 */
void Adafruit_PWMServoDriver_begin(uint8_t prescale) {
  Wire.begin();
  Adafruit_PWMServoDriver_reset();
  if (prescale) {
    Adafruit_PWMServoDriver_setExtClk(prescale);
  } else {
    // set a default frequency
    Adafruit_PWMServoDriver_setPWMFreq((float)1000);
  }
}

/*!
 *  @brief  Sends a reset command to the PCA9685 chip over I2C
 */
void Adafruit_PWMServoDriver_reset() {
  Adafruit_PWMServoDriver_write8(PCA9685_MODE1, 0x80);
  delay(10);
}

/*!
 *  @brief  Puts board into sleep mode
 */
void Adafruit_PWMServoDriver_sleep() {
  uint8_t awake = Adafruit_PWMServoDriver_read8(PCA9685_MODE1);
  uint8_t sleep = awake | 0x10; // set sleep bit high
  Adafruit_PWMServoDriver_write8(PCA9685_MODE1, sleep);
  delay(5); // wait until cycle ends for sleep to be active
}

/*!
 *  @brief  Wakes board from sleep
 */
void Adafruit_PWMServoDriver_wakeup() {
  uint8_t sleep = Adafruit_PWMServoDriver_read8(PCA9685_MODE1);
  uint8_t wakeup = sleep & ~0x10; // set sleep bit low
  Adafruit_PWMServoDriver_write8(PCA9685_MODE1, wakeup);
}

/**************************************************************************/
/*!
    @brief  Sets EXTCLK pin to use the external clock
       @param  prescale Configures the prescale value to be used by the external
   clock
*/
/**************************************************************************/
void Adafruit_PWMServoDriver_setExtClk(uint8_t prescale) {
  uint8_t oldmode;
	uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep
	Adafruit_PWMServoDriver_write8(PCA9685_MODE1, newmode); // go to sleep, turn off internal oscillator
	oldmode = Adafruit_PWMServoDriver_read8(PCA9685_MODE1);

	// This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
	// use the external clock.
	Adafruit_PWMServoDriver_write8(PCA9685_MODE1, (newmode |= 0x40));

	Adafruit_PWMServoDriver_write8(PCA9685_PRESCALE, prescale); // set the prescaler

	delay(5);
	Adafruit_PWMServoDriver_write8(PCA9685_MODE1,
			(newmode & ~(0x10)) | 0xa0); // clear the SLEEP bit to start

#ifdef ENABLE_DEBUG_OUTPUT
	Serial.print("Mode now 0x");
	Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

/*!
 *  @brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
 *  @param  freq Floating point frequency that we will attempt to match
 */
void Adafruit_PWMServoDriver_setPWMFreq(float freq) {
	float prescaleval;
	uint8_t prescale;
	uint8_t oldmode;
	uint8_t newmode; // sleep

#ifdef ENABLE_DEBUG_OUTPUT
	Serial.print("Attempting to set freq ");
	Serial.println(freq);
#endif

	freq *=
		0.9; // Correct for overshoot in the frequency setting (see issue #11).
	prescaleval = 25000000;
	prescaleval /= 4096;
	prescaleval /= freq;
	prescaleval -= 1;

#ifdef ENABLE_DEBUG_OUTPUT
	Serial.print("Estimated pre-scale: ");
	Serial.println(prescaleval);
#endif

	prescale = floor(prescaleval + 0.5);
#ifdef ENABLE_DEBUG_OUTPUT
	Serial.print("Final pre-scale: ");
	Serial.println_long(prescale,DEC);
#endif

	oldmode = Adafruit_PWMServoDriver_read8(PCA9685_MODE1);
	newmode = (oldmode & 0x7F) | 0x10; // sleep
	Adafruit_PWMServoDriver_write8(PCA9685_MODE1, newmode);            // go to sleep
	Adafruit_PWMServoDriver_write8(PCA9685_PRESCALE, prescale);        // set the prescaler
	Adafruit_PWMServoDriver_write8(PCA9685_MODE1, oldmode);
	delay(5);
	Adafruit_PWMServoDriver_write8(PCA9685_MODE1,
			oldmode |
			0xa0); //  This sets the MODE1 register to turn on auto increment.

#ifdef ENABLE_DEBUG_OUTPUT
	Serial.print("Mode now 0x");
	Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

/*!
 *  @brief  Gets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @return requested PWM output value
 */
uint8_t Adafruit_PWMServoDriver_getPWM(uint8_t num) {
	//Wire.requestFrom((uint8_t)i2caddr, LED0_ON_L + 4 * num, (uint8_t)4);
	Wire.requestFrom((uint8_t)i2caddr, LED0_ON_L + 4 * num, true);
	return Wire.read();
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  on At what point in the 4096-part cycle to turn the PWM output ON
 *  @param  off At what point in the 4096-part cycle to turn the PWM output OFF
 */
void Adafruit_PWMServoDriver_setPWM(uint8_t num, uint16_t on, uint16_t off) {
#ifdef ENABLE_DEBUG_OUTPUT
	Serial.print("Setting PWM ");
	Serial.print(num);
	Serial.print(": ");
	Serial.print(on);
	Serial.print("->");
	Serial.println(off);
#endif

	Wire.beginTransmission(i2caddr);
	Wire.write_byte(LED0_ON_L + 4 * num);
	Wire.write_byte(on);
	Wire.write_byte(on >> 8);
	Wire.write_byte(off);
	Wire.write_byte(off >> 8);
	Wire.endTransmission(true);
}

/*!
 *   @brief  Helper to set pin PWM output. Sets pin without having to deal with
 * on/off tick placement and properly handles a zero value as completely off and
 * 4095 as completely on.  Optional invert parameter supports inverting the
 * pulse for sinking to ground.
 *   @param  num One of the PWM output pins, from 0 to 15
 *   @param  val The number of ticks out of 4096 to be active, should be a value
 * from 0 to 4095 inclusive.
 *   @param  invert If true, inverts the output, defaults to 'false'
 */
void Adafruit_PWMServoDriver_setPin(uint8_t num, uint16_t val, bool invert) {
	// Clamp value between 0 and 4095 inclusive.
	val = min(val, (uint16_t)4095);
	if (invert) {
		if (val == 0) {
			// Special value for signal fully on.
			Adafruit_PWMServoDriver_setPWM(num, 4096, 0);
		} else if (val == 4095) {
			// Special value for signal fully off.
			Adafruit_PWMServoDriver_setPWM(num, 0, 4096);
		} else {
			Adafruit_PWMServoDriver_setPWM(num, 0, 4095 - val);
		}
	} else {
		if (val == 4095) {
			// Special value for signal fully on.
			Adafruit_PWMServoDriver_setPWM(num, 4096, 0);
		} else if (val == 0) {
			// Special value for signal fully off.
			Adafruit_PWMServoDriver_setPWM(num, 0, 4096);
		} else {
			Adafruit_PWMServoDriver_setPWM(num, 0, val);
		}
	}
}

uint8_t Adafruit_PWMServoDriver_read8(uint8_t addr) {
	Wire.beginTransmission(i2caddr);
	Wire.write_byte(addr);
	Wire.endTransmission(false);

	Wire.requestFrom((uint8_t)i2caddr, (uint8_t)1,true);
	return Wire.read();
}

void Adafruit_PWMServoDriver_write8(uint8_t addr, uint8_t d) {
	Wire.beginTransmission(i2caddr);
	Wire.write_byte(addr);
	Wire.write_byte(d);
	Wire.endTransmission(true);
}


ADAFUIT_PWMSERVO_DRIVER Adafruit_PWMServoDriver ={
	Adafruit_PWMServoDriver_init,
	//	void (*init)(TwoWire *I2C = &Wire, uint8_t addr = 0x40);
	//	void (*begin)(uint8_t prescale = 0);
	Adafruit_PWMServoDriver_begin,
	Adafruit_PWMServoDriver_reset,
	Adafruit_PWMServoDriver_sleep,
	Adafruit_PWMServoDriver_wakeup,
	Adafruit_PWMServoDriver_setExtClk,
	Adafruit_PWMServoDriver_setPWMFreq,
	Adafruit_PWMServoDriver_getPWM,
	Adafruit_PWMServoDriver_setPWM,
	//  void (*setPin)(uint8_t num, uint16_t val, bool invert=false);
	Adafruit_PWMServoDriver_setPin
};

