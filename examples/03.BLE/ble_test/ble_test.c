#include "BLE_test_ide.h"		// Additional Header


#define BLUE_LED	26
#define ORANGE_LED	25

static char advertise_data[64];
static const char advertise_test_data[] = {
	0x00,0x01,0x02,0x03,
	0x04,0x05,0x06,0x07,
	0x08,0x09,0x0a,0x0b,
	0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,
	0x14,0x15,0x16,0x17,
//	0x18,0x19,0x1a,0x1b,
//	0x1c,0x1d,0x1e,
};
static char advertise_len;
static const char test_data[] ="智恵子は東京に空が無いといふ。\n"
"ほんとの空が見たいといふ。\n"
"私は驚いて空を見る。\n"
"桜若葉の間（あいだ）に在るのは、\n"
"切っても切れない\n"
"むかしなじみのきれいな空だ。\n"
"どんよりけむる地平のぼかしは\n"
"うすもも色の朝のしめりだ。\n"
"智恵子は遠くを見ながら言ふ。\n"
"阿多多羅山（あたたらやま）の山の上に\n"
"毎日出ている青い空が\n"
"智恵子のほんとうの空だといふ。\n"
"あどけない空の話である。\n";


void BLE_callback(int mode) {
	Serial3.print("callback:: ");
	Serial3.print("\t");
	Serial3.println_long(mode,DEC);
}
void setup() {
  // put your setup code here, to run once:
	pinMode(BLUE_LED,OUTPUT);
	pinMode(ORANGE_LED,OUTPUT);

	BLE.begin(BLE_callback);
	Serial3.begin(115200);
//	BLE.advertising(true);
	
}

char hello[] = "hello world\r\n";
void loop() {
	// put your main code here, to run repeatedly:
	volatile unsigned long current_time;
	volatile static unsigned long last_monitor_time = 0x8000;
	int data;
	int i;
	uint16_t fast,slow, time;
	data = Serial3.read();
	
	switch(data)
	{
	case '1':
		Serial3.println_long(BLE.advertise(true),DEC);
		break;
	case '2':
		Serial3.println_long(BLE.advertise(false),DEC);
		break;
	case '3':
		BLE.print(test_data);
		break;
	case '4':
		BLE.println("println");
		break;
	case '5':
		BLE.print_long(123,DEC);
		break;
	case '6':
		BLE.println_long(0xFFFF,HEX);
		break;
	case '7':
		BLE.print_double(1.23456,2);
		break;
	case '8':
		BLE.println_double(3.141592,3);
		break;
	case '9':
		BLE.write_byte('w');
		break;
	case 'a':
		BLE.write(hello,sizeof(hello));
		break;
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
		break;
	case 'v':
		Serial3.print_long(BLE.getPairingMode(&advertise_len),DEC);
		Serial3.print("\t");
		Serial3.println_long(advertise_len,DEC);
		break;
	case 'V':
		Serial3.println_long(BLE.setPairingMode(1),DEC);
		break;
	case 'w':
		Serial3.print_long(BLE.getTxPower(&advertise_len),DEC);
		Serial3.print("\t");
		Serial3.println_long(advertise_len,DEC);
		break;
	case 'W':
		BLE.setTxPower(1);
		break;
	case 'x':
		Serial3.print_long(BLE.getAdvertiseInterval(&fast, &slow, &time),DEC);
		Serial3.print("\t");
		Serial3.print_long(fast,DEC);
		Serial3.print("\t");
		Serial3.print_long(slow,DEC);
		Serial3.print("\t");
		Serial3.println_long(time,DEC);
		break;
	case 'X':
		Serial3.print_long(BLE.setAdvertiseInterval(20,1000,3000),DEC);
		break;
	case 'y':
		Serial3.print_long(BLE.getConnectInterval(&fast, &slow, &time),DEC);
		Serial3.print("\t");
		Serial3.print_long(fast,DEC);
		Serial3.print("\t");
		Serial3.print_long(slow,DEC);
		Serial3.print("\t");
		Serial3.println_long(time,DEC);
		break;
	case 'Y':
		Serial3.println_long(BLE.setConnectInterval(8,100,3000),DEC);
		break;
	case 'z':
		Serial3.print_long(BLE.getAdvertiseData(advertise_data,&advertise_len),DEC);
		Serial3.print("\t");
		for(i=0;i<advertise_len;i++) {
			if(advertise_data[i] < 16) Serial3.print("0");
			Serial3.print_long(advertise_data[i],HEX);
		}
		Serial3.println("");
		break;
	case 'Z':
		Serial3.println_long(BLE.setAdvertiseData(advertise_test_data,sizeof(advertise_test_data)),DEC);
		break;
	}
	if(BLE.available()>0) {
		Serial3.write_byte(BLE.read());
	}
	current_time = millis();
	if((current_time-last_monitor_time)>1000) {
		Serial3.print("BLE status::\t");
		Serial3.print_long(BLE.getStatus(),DEC);
		Serial3.print("\t");
		Serial3.println_long(drv_digitalRead(32),DEC);
		last_monitor_time = current_time;
	}
}
