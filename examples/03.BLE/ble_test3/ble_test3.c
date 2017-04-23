#include "ble_test3_ide.h"		// Additional Header

#define BLUE_LED	26
#define ORANGE_LED	25
#define SENSOR_BUF_SIZE  	( 100 )

uint8_t accel_gyro[50][50] = {
	{"-0.49,0.85,-0.91,-227.01,-187.26,-9.43,"},
	{"-0.41,1.02,-0.81,-214.27,-219.64,-10.88,"},
	{"-0.32,1.07,-0.70,-226.39,-253.82,-6.36,"},
	{"-0.28,0.94,-0.66,-243.32,-256.00,3.75,"},
	{"-0.37,0.78,-0.56,-250.46,-256.00,20.50,"},
	{"-0.50,0.81,-0.44,-246.35,-254.23,19.88,"},
	{"-0.50,0.87,-0.28,-231.35,-237.93,-6.19,"},
	{"-0.28,0.96,-0.10,-223.35,-242.65,-36.08,"},
	{"-0.22,0.99,0.03,-221.00,-249.84,-58.57,"},
	{"-0.28,0.92,0.13,-213.57,-232.28,-61.79,"},
	{"-0.36,0.77,0.21,-205.36,-195.07,-43.80,"},
	{"-0.39,0.66,0.30,-198.14,-161.74,-26.57,"},
	{"-0.42,0.66,0.36,-192.09,-148.48,-32.41,"},
	{"-0.45,0.67,0.45,-177.07,-140.57,-62.75,"},
	{"-0.43,0.71,0.50,-158.32,-135.21,-91.42,"},
	{"-0.41,0.73,0.54,-143.97,-138.81,-89.29,"},
	{"-0.35,0.63,0.61,-137.82,-142.90,-52.33,"},
	{"-0.28,0.60,0.67,-144.35,-144.12,-22.06,"},
	{"-0.35,0.55,0.72,-147.84,-138.35,-22.33,"},
	{"-0.43,0.57,0.78,-143.78,-128.69,-37.32,"},
	{"-0.47,0.60,0.80,-137.39,-125.49,-51.98,"},
	{"-0.35,0.56,0.81,-131.28,-132.32,-60.02,"},
	{"-0.19,0.50,0.86,-134.75,-143.82,-57.10,"},
	{"-0.16,0.46,0.92,-141.76,-149.64,-43.03,"},
	{"-0.18,0.38,0.93,-143.71,-143.80,-33.09,"},
	{"-0.18,0.27,0.92,-141.48,-133.96,-29.72,"},
	{"-0.15,0.27,0.95,-134.80,-130.47,-29.82,"},
	{"-0.02,0.27,0.94,-128.99,-131.53,-43.50,"},
	{"0.16,0.10,0.87,-129.99,-133.75,-58.67,"},
	{"0.17,-0.07,0.87,-128.55,-131.31,-56.92,"},
	{"0.04,-0.07,0.89,-111.60,-117.88,-50.93,"},
	{"0.03,-0.02,0.89,-85.81,-101.91,-67.78,"},
	{"0.18,0.00,0.89,-64.60,-95.38,-105.75,"},
	{"0.23,-0.02,0.85,-53.31,-98.84,-135.62,"},
	{"0.10,-0.06,0.81,-40.22,-99.69,-142.68,"},
	{"0.12,0.02,0.86,-17.05,-87.45,-153.23,"},
	{"0.34,0.13,0.85,4.85,-78.42,-186.73,"},
	{"0.50,0.14,0.79,14.57,-84.06,-207.92"},
	{"0.58,0.10,0.77,22.10,-79.82,-209.19,"},
	{"0.42,0.33,0.80,32.10,-54.14,-228.35,"},
	{"0.48,0.44,0.83,28.03,-19.72,-256.00,"},
	{"0.60,0.32,0.79,8.24,5.05,-256.00,"},
	{"0.43,0.20,0.71,-8.82,25.25,-256.00,"},
	{"0.05,0.27,0.71,-12.07,59.50,-256.00,"},
	{"-0.19,0.61,0.89,-10.98,95.88,-256.00,"},
	{"0.13,0.49,0.80,-21.02,93.36,-256.00,"},
	{"0.17,0.28,0.75,-25.42,62.64,-256.00,"},
	{"0.00,0.28,0.75,-19.35,50.08,-256.00,"},
	{"-0.14,0.36,0.80,-7.89,51.48,-256.00,"},
	{"-0.19,0.45,0.86,-0.29,46.36,-256.00,"}};

void setup() {
	Serial3.begin(115200);
  // put your setup code here, to run once:

	pinMode(BLUE_LED,OUTPUT);
	digitalWrite(BLUE_LED,HIGH);
	pinMode(ORANGE_LED,OUTPUT);
	digitalWrite(ORANGE_LED,HIGH);
	
	ble.begin();
}


void loop() {
  // put your main code here, to run repeatedly:
  
	uint32_t tmp, deltaT;
	static uint32_t p_sensor_ts;
	static uint16_t seq_num = 0;
	static uint8_t txdata[SENSOR_BUF_SIZE];
	volatile unsigned long current_time;
	volatile static unsigned long last_monitor_time = 0x8000;

/*
	current_time = millis();
	if((current_time - last_monitor_time)>1000) {
		digitalWrite(ORANGE_LED,LOW);
		digitalWrite(BLUE_LED,HIGH);
		if(ble.getStatus() == 1) {
			ble.advertising(true);
		}
		digitalWrite(ORANGE_LED,HIGH);
		digitalWrite(BLUE_LED,LOW);
		if(ble.getStatus() == 3) {
			tmp = millis();
			deltaT = tmp - p_sensor_ts;
			p_sensor_ts = tmp;
			
			Print.init(txdata,sizeof(txdata));
			Print.p("$MOTION,,");
			Print.l((uint32_t)seq_num++, DEC);				// sequence number
			Print.p(",");
			Print.l((uint32_t)deltaT, DEC);					// delta t
			Print.p(",");
			Print.p(accel_gyro[seq_num%50]);
			Print.p(",-0.402,-0.309,-0.914,1019.456");
	//		digitalWrite(BLUE_LED, LOW);
			ble.println(txdata);
	//Serial.println(txdata);
	//		digitalWrite(BLUE_LED, HIGH);
		}
		last_monitor_time = current_time;
		digitalWrite(ORANGE_LED,HIGH);
		digitalWrite(BLUE_LED,HIGH);
		Serial3.print("ble status::\t");
		Serial3.print_long(ble.getStatus(),DEC);
	//	Serial3.print("\t");
	//	Serial3.print_long(current_time,DEC);
		Serial3.print("\t");
		Serial3.println_long(millis(),DEC);
		digitalWrite(ORANGE_LED,HIGH);
		digitalWrite(BLUE_LED,HIGH);
	}
*/

	if(ble.getStatus() == DEEPSLEEP) {
		ble.advertise(true);
	}
	if(ble.getStatus() == SERIAL_CONNECT) {
		tmp = millis();
		deltaT = tmp - p_sensor_ts;
		p_sensor_ts = tmp;
		
		Print.init(txdata,sizeof(txdata));
		Print.p("$MOTION,,");
		Print.l((uint32_t)seq_num++, DEC);				// sequence number
		Print.p(",");
		Print.l((uint32_t)deltaT, DEC);					// delta t
		Print.p(",");
		Print.p(accel_gyro[seq_num%50]);
		Print.p(",-0.402,-0.309,-0.914,1019.456");
		digitalWrite(BLUE_LED, LOW);
		ble.println(txdata);
//Serial.println(txdata);
		digitalWrite(BLUE_LED, HIGH);
	}

	Serial3.print("ble status::\t");
	Serial3.print_long(ble.getStatus(),DEC);
//	Serial3.print("\t");
//	Serial3.print_long(current_time,DEC);
	Serial3.print("\t");
	Serial3.println_long(millis(),DEC);
	delay(200);

}
