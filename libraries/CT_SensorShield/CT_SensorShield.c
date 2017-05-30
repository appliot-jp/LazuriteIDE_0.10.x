
#include <common.h>

double ct_meas() {
	unsigned long  st_time,en_time;
	double amps;
	volatile int st_voltage,en_voltage;
	double amps;
	digitalWrite(LED,LOW);
	digitalWrite(MEAS,HIGH);
	digitalWrite(CHB,HIGH);
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
	digitalWrite(LED,HIGH);
	amps = 3.3*1.1;
	amps = amps * (en_voltage-st_voltage) /4.096/(en_time - st_time);  
	return amps;
}

