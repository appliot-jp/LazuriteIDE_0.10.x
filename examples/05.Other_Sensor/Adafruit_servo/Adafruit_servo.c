#include "Adafruit_servo_ide.h"		// Additional Header
/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815
  
  These drivers use I2C to communicate, 2 pins are required to  
  interface.
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


// called this way, it uses the default address 0x40
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(&Wire, 0x40);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("8 channel Servo test!");

  Adafruit_PWMServoDriver.init(0x40);
  Adafruit_PWMServoDriver.begin(0);
  
  Adafruit_PWMServoDriver.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  delay(10);
}

// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print_double(pulselength,2); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print_double(pulselength,2); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;
  Serial.println_double(pulse,2);
  Adafruit_PWMServoDriver.setPWM(n, 0, pulse);
}

void loop() {
	uint16_t pulselen;
  // Drive each servo one at a time
  Serial.println_long(servonum,DEC);
  for ( pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
    Adafruit_PWMServoDriver.setPWM(servonum, 0, pulselen);
  }

  delay(500); 
  for (pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
    Adafruit_PWMServoDriver.setPWM(servonum, 0, pulselen);
  }

  delay(500);

  servonum ++;
  if (servonum > 7) servonum = 0;
}
