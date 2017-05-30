#include "CT_Sensor_serial_ide.h"		// Additional Header

#define CHB	2
#define MEAS 3
#define MEAS_LED	26

bool event = false;
void callback(void)
{
	event = true;
}

void ct_init() {
	pinMode(CHB,OUTPUT);
	pinMode(MEAS,OUTPUT);
	digitalWrite(CHB,LOW);
	digitalWrite(MEAS,LOW);
	analogReadResolution(12);
}


double ct_meas() {
	unsigned long  st_time,en_time;
	double amps;
	volatile int st_voltage,en_voltage,dif_vol;
	digitalWrite(MEAS,HIGH);
	digitalWrite(CHB,HIGH);
	
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	delayMicroseconds(1000);
	st_voltage = analogRead(A0);
	st_time = millis();
	do {
		delayMicroseconds(1000);
		en_voltage = analogRead(A0);
		en_time = millis();
	} while(((en_time - st_time) < 1000) && (en_voltage < 1024));
	digitalWrite(CHB,LOW);
	digitalWrite(MEAS,LOW);
	
	amps = 3.3*1.1;
	dif_vol = en_voltage-st_voltage;
	dif_vol = (dif_vol < 0) ? 0 : dif_vol;
	amps = amps * dif_vol /4.096/(en_time - st_time);  
	return amps;
}

void setup() {
	// put your setup code here, to run once
	ct_init();
	digitalWrite(MEAS_LED,HIGH);
	pinMode(MEAS_LED,OUTPUT);
	
	timer2.set(5000L,callback);
	event = true;
	Serial.begin(115200);
	analogReadResolution(12);
	timer2.start();
	Serial.println("CT Sensor Start");
}

void loop() {
	// put your main code here, to run repeatedly:
	double amps;
	digitalWrite(MEAS_LED,LOW);
	amps = ct_meas();
	digitalWrite(MEAS_LED,HIGH);

	wait_event(&event);
	Serial.print_double(amps,2);
	Serial.println("mA");
}
