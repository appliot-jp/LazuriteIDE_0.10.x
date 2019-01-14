#include "ArduCAM_Mini_5MP_OV5642_Plus_Functions_ide.h"		// Additional Header

// This program is a demo of how to use most of the functions
// of the library with ArduCAM Mini camera, and can run on Lazurite platform.
// This demo was made for ArduCAM_Mini_5MP_Plus.
// It needs to be used in combination with PC software.
// It can test OV5642 functions
// This program requires the ArduCAM library and ArduCAM_Mini_5MP_Plus
// and use Lazurite IDE

#define OV5642_CHIPID_HIGH 0x300a
#define OV5642_CHIPID_LOW 0x300b

#define BMPIMAGEOFFSET 66
const char bmp_header[BMPIMAGEOFFSET] =
{
  0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00
};
// set pin 7 as the slave select for the digital pot:
#define CS_PIN			7
#define BLUE_LED		26			// pin number of Blue LED
#define SUBGHZ_CH		36			// channel number (frequency)
#define SUBGHZ_PANID	0xabcd		// panid
#define HOST_ADDRESS	0x4577

bool is_header = false;
int mode = 0;
uint8_t start_capture = 0;
uint8_t read_fifo_burst(void);

#define RAW_SIZE		128
typedef struct {
	long length;
	uint32_t addr;
	uint8_t raw[RAW_SIZE+10];
} SEND_DATA;
SEND_DATA send_data;
	
void setup()
{
	// put your setup code here, to run once:
	uint8_t vid, pid;
	uint8_t temp;

	SubGHz.init();
	pinMode(BLUE_LED, OUTPUT);
	digitalWrite(BLUE_LED, HIGH);
	Wire.begin();
	Serial.begin(115200);
	Serial.println("ACK CMD ArduCAM Start! END");
	// set the CS as an output:
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);
	// initialize SPI:
	SPI.begin();
	ArduCAM.init(OV5642, CS_PIN);
	// set Normal Operation
	ArduCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
	//Reset the CPLD
	ArduCAM.write_reg(0x07, 0x80);
	delay(100);
	ArduCAM.write_reg(0x07, 0x00);
	delay(100);
	while(1){
		//Check if the ArduCAM SPI bus is OK
		ArduCAM.write_reg(ARDUCHIP_TEST1, 0x55);
		temp = ArduCAM.read_reg(ARDUCHIP_TEST1);
		if (temp != 0x55){
			Serial.println("ACK CMD SPI interface Error! END");
			delay(1000);
			continue;
		} else {
			Serial.println("ACK CMD SPI interface OK. END");
			break;
		}
	}
	while(1){
		//Check if the camera module type is OV5642
		ArduCAM.wrSensorReg16_8(0xff, 0x01);
		ArduCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
		ArduCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
		if((vid != 0x56) || (pid != 0x42)){
			Serial.println("ACK CMD Can't find OV5642 module! END");
			delay(1000);
			continue;
		} else {
			Serial.println("ACK CMD OV5642 detected. END");
			break;
		} 
	}
	//Change to JPEG capture mode and initialize the OV5642 module
	ArduCAM.set_format(JPEG);
	ArduCAM.InitCAM();

	ArduCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
	ArduCAM.OV5642_set_JPEG_size(OV5642_640x480);
	delay(1000);
	ArduCAM.clear_fifo_flag();
	ArduCAM.write_reg(ARDUCHIP_FRAMES,0x00);
	ArduCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);

	start_capture = 1;
	Serial.println("ACK CMD CAM start single shoot. END");
}

void loop()
{
	// put your main code here, to run repeatedly:
//	if (mode == 1) {
		if (start_capture == 1) {
			ArduCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
			Serial.println("ArduCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK)");
			delay(1000);
			//Flush the FIFO
			ArduCAM.flush_fifo();
			Serial.println("ArduCAM.flush_fifo()");
			ArduCAM.clear_fifo_flag();
			Serial.println("ArduCAM.clear_fifo_flag()");
			//Start capture
			ArduCAM.start_capture();
			Serial.println("ArduCAM.start_capture()");
			start_capture = 0;
		}
		if (ArduCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
			Serial.println("ACK CMD CAM Capture Done. END");
			delay(50);
			read_fifo_burst();
			//Clear the capture done flag
			ArduCAM.clear_fifo_flag();
			ArduCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
			sleep(3600000);
			start_capture = 1;
			Serial.println("ACK CMD CAM start single shoot. END");
		}
//	}
}

uint8_t read_fifo_burst(void)
{
	uint8_t temp = 0, temp_last = 0;
	uint32_t length = 0;
	uint32_t i = 0, j = 0;
	bool eof = false;
	SUBGHZ_MSG msg;

	length = ArduCAM.read_fifo_length();
	Serial.println_long(length, DEC);
	if (length >= MAX_FIFO_SIZE) { //512 kb
		Serial.println("ACK CMD Over size. END");
		return 0;
	}
	if (length == 0 ) { //0 kb
		Serial.println("ACK CMD Size is 0. END");
		return 0;
	}
	send_data.length = length;
	send_data.addr = 0L;
	ArduCAM.CS_LOW();
	ArduCAM.set_fifo_burst();//Set fifo burst mode
	length --;
	while ( length-- ) {
		temp_last = temp;
		temp = SPI.transfer(0x00);
		if (is_header == true) {
//			Serial.write_byte(temp);
			send_data.raw[i++] = temp;
		} else if ((temp == 0xD8) & (temp_last == 0xFF)) {
			is_header = true;
			Serial.println("ACK IMG END");
			Serial.write_byte(temp_last);
			Serial.write_byte(temp);
			send_data.raw[i++] = temp_last;
			send_data.raw[i++] = temp;
		}
		if ( (temp == 0xD9) && (temp_last == 0xFF) ) { //If find the end ,break while,
			eof = true;
		}
		if (eof || (i >= RAW_SIZE)) {
			send_data.addr = RAW_SIZE * j++;
			SubGHz.begin(SUBGHZ_CH, SUBGHZ_PANID, SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);
			digitalWrite(BLUE_LED,LOW);
			msg = SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, (uint8_t *)&send_data, (uint16_t)(8+i), NULL);	// 4+4+i
			if (msg != SUBGHZ_OK) {
				// send retry
				SubGHz.send(SUBGHZ_PANID, HOST_ADDRESS, (uint8_t *)&send_data, (uint16_t)(8+i), NULL);
			}
			digitalWrite(BLUE_LED,HIGH);
			SubGHz.close();
			i = 0;
			if (eof) break;
		}
		delayMicroseconds(100);
	}
	ArduCAM.CS_HIGH();
	is_header = false;
	return 1;
}
