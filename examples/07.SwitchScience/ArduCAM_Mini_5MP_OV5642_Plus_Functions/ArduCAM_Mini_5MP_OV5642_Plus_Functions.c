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
	ArduCAM.OV5642_set_JPEG_size(OV5642_320x240);
	delay(1000);
	ArduCAM.clear_fifo_flag();
	ArduCAM.write_reg(ARDUCHIP_FRAMES,0x00);
}

void loop()
{
	// put your main code here, to run repeatedly:
	uint8_t temp = 0xff, temp_last = 0;
	if (Serial.available()) {
		temp = (uint8_t)Serial.read();
		switch (temp) {
			case 0x00:
				ArduCAM.OV5642_set_JPEG_size(OV5642_320x240);
				delay(1000);
   				Serial.println("ACK CMD switch to OV5642_320x240 END");
				break;
			case 0x01:
				ArduCAM.OV5642_set_JPEG_size(OV5642_640x480);
				delay(1000);
   				Serial.println("ACK CMD switch to OV5642_640x480 END");
				break;
			case 0x02: 
				ArduCAM.OV5642_set_JPEG_size(OV5642_1024x768);
				delay(1000);
   				Serial.println("ACK CMD switch to OV5642_1024x768 END");
				break;
			case 0x03:
				ArduCAM.OV5642_set_JPEG_size(OV5642_1280x960);
				delay(1000);
   				Serial.println("ACK CMD switch to OV5642_1280x960 END");
				break;
			case 0x04:
				ArduCAM.OV5642_set_JPEG_size(OV5642_1600x1200);
				delay(1000);
   				Serial.println("ACK CMD switch to OV5642_1600x1200 END");
				break;
			case 0x05:
				ArduCAM.OV5642_set_JPEG_size(OV5642_2048x1536);
				delay(1000);
   				Serial.println("ACK CMD switch to OV5642_2048x1536 END");
				break;
			case 0x06:
				ArduCAM.OV5642_set_JPEG_size(OV5642_2592x1944);
				delay(1000);
   				Serial.println("ACK CMD switch to OV5642_2592x1944 END");
				break;
			case 0x10:
				mode = 1;
				start_capture = 1;
 				Serial.println("ACK CMD CAM start single shoot. END");
				break;
			case 0x11: 
				ArduCAM.set_format(JPEG);
				ArduCAM.InitCAM();
#if !(defined (OV2640_MINI_2MP))
				ArduCAM.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
#endif
				break;
			case 0x20:
				mode = 2;
				start_capture = 2;
 				Serial.println("ACK CMD CAM start video streaming. END");
				break;
			case 0x30:
				mode = 3;
				start_capture = 3;
 				Serial.println("ACK CMD CAM start single shoot. END");
				break;
			case 0x31:
				ArduCAM.set_format(BMP);
				ArduCAM.InitCAM();     
				ArduCAM.clear_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
				ArduCAM.wrSensorReg16_8(0x3818, 0x81);
				ArduCAM.wrSensorReg16_8(0x3621, 0xA7);
				break;
			case 0x40:
				ArduCAM.OV5642_set_Light_Mode(Advanced_AWB);
  				Serial.println("ACK CMD Set to Advanced_AWB END");
				break;
			case 0x41:
				ArduCAM.OV5642_set_Light_Mode(Simple_AWB);
  				Serial.println("ACK CMD Set to Simple_AWB END");
				break;
 			case 0x42:
				ArduCAM.OV5642_set_Light_Mode(Manual_day);
  				Serial.println("ACK CMD Set to Manual_day END");
				break;
 			case 0x43:
				ArduCAM.OV5642_set_Light_Mode(Manual_A);
  				Serial.println("ACK CMD Set to Manual_A END");
				break;
 			case 0x44:
				ArduCAM.OV5642_set_Light_Mode(Manual_cwf);
  				Serial.println("ACK CMD Set to Manual_cwf END");
				break;
 			case 0x45:
				ArduCAM.OV5642_set_Light_Mode(Manual_cloudy);
  				Serial.println("ACK CMD Set to Manual_cloudy END");
				break;
  			case 0x50:
				ArduCAM.OV5642_set_Color_Saturation(Saturation4);
  				Serial.println("ACK CMD Set to Saturation+4 END");
				break;
			case 0x51:
				ArduCAM.OV5642_set_Color_Saturation(Saturation3);
  				Serial.println("ACK CMD Set to Saturation+3 END");
				break;
			case 0x52:
				ArduCAM.OV5642_set_Color_Saturation(Saturation2);
				Serial.println("ACK CMD Set to Saturation+2 END");
				break;
			case 0x53:
				ArduCAM.OV5642_set_Color_Saturation(Saturation1);
				Serial.println("ACK CMD Set to Saturation+1 END");
				break;
			case 0x54:
				ArduCAM.OV5642_set_Color_Saturation(Saturation0);
				Serial.println("ACK CMD Set to Saturation+0 END");
				break;
			case 0x55:
				ArduCAM.OV5642_set_Color_Saturation(Saturation_1);
				Serial.println("ACK CMD Set to Saturation-1 END");
				break;
			case 0x56:
				ArduCAM.OV5642_set_Color_Saturation(Saturation_2);
				Serial.println("ACK CMD Set to Saturation-2");
				break;
			case 0x57:
				ArduCAM.OV5642_set_Color_Saturation(Saturation_3);
				Serial.println("ACK CMD Set to Saturation-3 END");
				break;
			case 0x58:
				ArduCAM.OV5642_set_Light_Mode(Saturation_4);
				Serial.println("ACK CMD Set to Saturation-4 END");
				break; 
			case 0x60:
				ArduCAM.OV5642_set_Brightness(Brightness4);
				Serial.println("ACK CMD Set to Brightness+4 END");
				break;
			case 0x61:
				ArduCAM.OV5642_set_Brightness(Brightness3);
				Serial.println("ACK CMD Set to Brightness+3 END");
				break; 
			case 0x62:
				ArduCAM.OV5642_set_Brightness(Brightness2);
				Serial.println("ACK CMD Set to Brightness+2 END");
				break; 
			case 0x63:
				ArduCAM.OV5642_set_Brightness(Brightness1);
				Serial.println("ACK CMD Set to Brightness+1 END");
				break; 
			case 0x64:
				ArduCAM.OV5642_set_Brightness(Brightness0);
				Serial.println("ACK CMD Set to Brightness+0 END");
				break; 
			case 0x65:
				ArduCAM.OV5642_set_Brightness(Brightness_1);
				Serial.println("ACK CMD Set to Brightness-1 END");
				break; 
 			case 0x66:
				ArduCAM.OV5642_set_Brightness(Brightness_2);
				Serial.println("ACK CMD Set to Brightness-2 END");
				break; 
			case 0x67:
				ArduCAM.OV5642_set_Brightness(Brightness_3);
				Serial.println("ACK CMD Set to Brightness-3 END");
				break; 
			case 0x68:
				ArduCAM.OV5642_set_Brightness(Brightness_4);
				Serial.println("ACK CMD Set to Brightness-4 END");
				break;
			case 0x70:
				ArduCAM.OV5642_set_Contrast(Contrast4);
				Serial.println("ACK CMD Set to Contrast+4 END");
				break;
			case 0x71:
				ArduCAM.OV5642_set_Contrast(Contrast3);
				Serial.println("ACK CMD Set to Contrast+3 END");
				break; 
			case 0x72:
				ArduCAM.OV5642_set_Contrast(Contrast2);
				Serial.println("ACK CMD Set to Contrast+2 END");
				break; 
			case 0x73:
				ArduCAM.OV5642_set_Contrast(Contrast1);
				Serial.println("ACK CMD Set to Contrast+1 END");
				break; 
			case 0x74:
				ArduCAM.OV5642_set_Contrast(Contrast0);
				Serial.println("ACK CMD Set to Contrast+0 END");
				break; 
			case 0x75:
				ArduCAM.OV5642_set_Contrast(Contrast_1);
				Serial.println("ACK CMD Set to Contrast-1 END");
				break; 
 			case 0x76:
				ArduCAM.OV5642_set_Contrast(Contrast_2);
				Serial.println("ACK CMD Set to Contrast-2 END");
				break; 
			case 0x77:
				ArduCAM.OV5642_set_Contrast(Contrast_3);
				Serial.println("ACK CMD Set to Contrast-3 END");
				break; 
			case 0x78:
				ArduCAM.OV5642_set_Contrast(Contrast_4);
				Serial.println("ACK CMD Set to Contrast-4 END");
				break;
			case 0x80: 
				ArduCAM.OV5642_set_hue(degree_180);
  				Serial.println("ACK CMD Set to -180 degree END");
				break;   
			case 0x81: 
 				ArduCAM.OV5642_set_hue(degree_150);
  				Serial.println("ACK CMD Set to -150 degree END");
				break;  
			case 0x82: 
 				ArduCAM.OV5642_set_hue(degree_120);
  				Serial.println("ACK CMD Set to -120 degree END");
				break;  
			case 0x83: 
 				ArduCAM.OV5642_set_hue(degree_90);
  				Serial.println("ACK CMD Set to -90 degree END");
				break;   
			case 0x84: 
 				ArduCAM.OV5642_set_hue(degree_60);
  				Serial.println("ACK CMD Set to -60 degree END");
				break;   
			case 0x85: 
 				ArduCAM.OV5642_set_hue(degree_30);
  				Serial.println("ACK CMD Set to -30 degree END");
				break;  
 			case 0x86: 
 				ArduCAM.OV5642_set_hue(degree_0);
  				Serial.println("ACK CMD Set to 0 degree END");
				break; 
			case 0x87: 
 				ArduCAM.OV5642_set_hue(degree30);
  				Serial.println("ACK CMD Set to 30 degree END");
				break;
			case 0x88: 
 				ArduCAM.OV5642_set_hue(degree60);
  				Serial.println("ACK CMD Set to 60 degree END");
				break;
			case 0x89: 
 				ArduCAM.OV5642_set_hue(degree90);
  				Serial.println("ACK CMD Set to 90 degree END");
				break;
 			case 0x8a: 
 				ArduCAM.OV5642_set_hue(degree120);
  				Serial.println("ACK CMD Set to 120 degree END");
				break; 
			case 0x8b: 
 				ArduCAM.OV5642_set_hue(degree150);
  				Serial.println("ACK CMD Set to 150 degree END");
				break;
			case 0x90: 
 				ArduCAM.OV5642_set_Special_effects(Normal);
  				Serial.println("ACK CMD Set to Normal END");
				break;
  			case 0x91: 
 				ArduCAM.OV5642_set_Special_effects(BW);
  				Serial.println("ACK CMD Set to BW END");
				break;
			case 0x92: 
 				ArduCAM.OV5642_set_Special_effects(Bluish);
  				Serial.println("ACK CMD Set to Bluish END");
				break;
  			case 0x93: 
 				ArduCAM.OV5642_set_Special_effects(Sepia);
  				Serial.println("ACK CMD Set to Sepia END");
				break;
			case 0x94: 
 				ArduCAM.OV5642_set_Special_effects(Reddish);
  				Serial.println("ACK CMD Set to Reddish END");
				break;
			case 0x95: 
 				ArduCAM.OV5642_set_Special_effects(Greenish);
  				Serial.println("ACK CMD Set to Greenish END");
				break;
			case 0x96: 
 				ArduCAM.OV5642_set_Special_effects(Negative);
  				Serial.println("ACK CMD Set to Negative END");
				break;
			case 0xA0: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure_17_EV);
  				Serial.println("ACK CMD Set to -1.7EV");
				break;  
 			case 0xA1: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure_13_EV);
  				Serial.println("ACK CMD Set to -1.3EV END");
				break;
  			case 0xA2: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure_10_EV);
  				Serial.println("ACK CMD Set to -1.0EV END");
				break; 
			case 0xA3: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure_07_EV);
  				Serial.println("ACK CMD Set to -0.7EV END");
				break;
 			case 0xA4: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure_03_EV);
  				Serial.println("ACK CMD Set to -0.3EV END");
				break;
			case 0xA5: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure_default);
  				Serial.println("ACK CMD Set to -Exposure_default END");
				break;
			case 0xA6: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure07_EV);
  				Serial.println("ACK CMD Set to 0.7EV END");
				break;  
			case 0xA7: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure10_EV);
  				Serial.println("ACK CMD Set to 1.0EV END");
				break;
			case 0xA8: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure13_EV);
  				Serial.println("ACK CMD Set to 1.3EV END");
				break; 
			case 0xA9: 
 				ArduCAM.OV5642_set_Exposure_level(Exposure17_EV);
  				Serial.println("ACK CMD Set to 1.7EV END");
				break; 
			case 0xB0: 
 				ArduCAM.OV5642_set_Sharpness(Auto_Sharpness_default);
  				Serial.println("ACK CMD Set to Auto Sharpness default END");
				break; 
			case 0xB1: 
 				ArduCAM.OV5642_set_Sharpness(Auto_Sharpness1);
  				Serial.println("ACK CMD Set to Auto Sharpness +1 END");
				break; 
			case 0xB2: 
 				ArduCAM.OV5642_set_Sharpness(Auto_Sharpness2);
  				Serial.println("ACK CMD Set to Auto Sharpness +2 END");
				break; 
  			case 0xB3: 
 				ArduCAM.OV5642_set_Sharpness(Manual_Sharpnessoff);
  				Serial.println("ACK CMD Set to Auto Manual Sharpness off END");
				break; 
 			case 0xB4: 
 				ArduCAM.OV5642_set_Sharpness(Manual_Sharpness1);
  				Serial.println("ACK CMD Set to Auto Manual Sharpness +1 END");
				break;
 			case 0xB5: 
 				ArduCAM.OV5642_set_Sharpness(Manual_Sharpness2);
  				Serial.println("ACK CMD Set to Auto Manual Sharpness +2 END");
				break; 
 			case 0xB6: 
 				ArduCAM.OV5642_set_Sharpness(Manual_Sharpness3);
  				Serial.println("ACK CMD Set to Auto Manual Sharpness +3 END");
				break;
 			case 0xB7: 
 				ArduCAM.OV5642_set_Sharpness(Manual_Sharpness4);
  				Serial.println("ACK CMD Set to Auto Manual Sharpness +4 END");
				break;
			case 0xB8: 
 				ArduCAM.OV5642_set_Sharpness(Manual_Sharpness5);
  				Serial.println("ACK CMD Set to Auto Manual Sharpness +5 END");
				break;  
			case 0xC0: 
 				ArduCAM.OV5642_set_Mirror_Flip(MIRROR);
  				Serial.println("ACK CMD Set to MIRROR END");
				break;  
			case 0xC1: 
 				ArduCAM.OV5642_set_Mirror_Flip(FLIP);
  				Serial.println("ACK CMD Set to FLIP END");
				break; 
			case 0xC2: 
 				ArduCAM.OV5642_set_Mirror_Flip(MIRROR_FLIP);
  				Serial.println("ACK CMD Set to MIRROR&FLIP END");
				break;
			case 0xC3: 
 				ArduCAM.OV5642_set_Mirror_Flip(Normal);
  				Serial.println("ACK CMD Set to Normal END");
				break;
 			case 0xD0: 
 				ArduCAM.OV5642_set_Compress_quality(high_quality);
  				Serial.println("ACK CMD Set to high quality END");
				break;
  			case 0xD1: 
 				ArduCAM.OV5642_set_Compress_quality(default_quality);
  				Serial.println("ACK CMD Set to default quality END");
				break;
  			case 0xD2: 
 				ArduCAM.OV5642_set_Compress_quality(low_quality);
  				Serial.println("ACK CMD Set to low quality END");
				break;
  			case 0xE0: 
 				ArduCAM.OV5642_Test_Pattern(Color_bar);
  				Serial.println("ACK CMD Set to Color bar END");
				break;
  			case 0xE1: 
 				ArduCAM.OV5642_Test_Pattern(Color_square);
  				Serial.println("ACK CMD Set to Color square END");
				break;
  			case 0xE2: 
 				ArduCAM.OV5642_Test_Pattern(BW_square);
  				Serial.println("ACK CMD Set to BW square END");
				break;
 			case 0xE3: 
 				ArduCAM.OV5642_Test_Pattern(DLI);
  				Serial.println("ACK CMD Set to DLI END");
				break;
			default:
  				break;
		}
		temp = 0xff;
	}
	if (mode == 1) {
		if (start_capture == 1) {
		    //Flush the FIFO
			ArduCAM.flush_fifo();
			ArduCAM.clear_fifo_flag();
		    //Start capture
			ArduCAM.start_capture();
			start_capture = 0;
		}
		if (ArduCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
			Serial.println("ACK CMD CAM Capture Done. END");
			delay(50);
			read_fifo_burst();
			//Clear the capture done flag
			ArduCAM.clear_fifo_flag();
			mode = 0;
		}
	} else if (mode == 2) {
		mode = 0;
		while (1) {
			if (Serial.available()) {
				temp = (uint8_t)Serial.read();
				if (temp == 0x21) {
					start_capture = 0;
					mode = 0;
					Serial.println("ACK CMD CAM stop video streaming. END");
					break;
				}
				switch (temp) {
					case 0x40:
						ArduCAM.OV5642_set_Light_Mode(Advanced_AWB);
						Serial.println("ACK CMD Set to Advanced_AWB END");
						break;
					case 0x41:
						ArduCAM.OV5642_set_Light_Mode(Simple_AWB);
						Serial.println("ACK CMD Set to Simple_AWB END");
						break;
					case 0x42:
						ArduCAM.OV5642_set_Light_Mode(Manual_day);
						Serial.println("ACK CMD Set to Manual_day END");
						break;
					case 0x43:
						ArduCAM.OV5642_set_Light_Mode(Manual_A);
						Serial.println("ACK CMD Set to Manual_A END");
						break;
					case 0x44:
						ArduCAM.OV5642_set_Light_Mode(Manual_cwf);
						Serial.println("ACK CMD Set to Manual_cwf END");
						break;
					case 0x45:
						ArduCAM.OV5642_set_Light_Mode(Manual_cloudy);
						Serial.println("ACK CMD Set to Manual_cloudy END");
						break;
					case 0x50:
						ArduCAM.OV5642_set_Color_Saturation(Saturation4);
						Serial.println("ACK CMD Set to Saturation+4 END");
						break;
					case 0x51:
						ArduCAM.OV5642_set_Color_Saturation(Saturation3);
						Serial.println("ACK CMD Set to Saturation+3 END");
						break;
					case 0x52:
						ArduCAM.OV5642_set_Color_Saturation(Saturation2);
						Serial.println("ACK CMD Set to Saturation+2 END");
						break;
					case 0x53:
						ArduCAM.OV5642_set_Color_Saturation(Saturation1);
						Serial.println("ACK CMD Set to Saturation+1 END");
						break;
					case 0x54:
						ArduCAM.OV5642_set_Color_Saturation(Saturation0);
						Serial.println("ACK CMD Set to Saturation+0 END");
						break;
					case 0x55:
						ArduCAM.OV5642_set_Color_Saturation(Saturation_1);
						Serial.println("ACK CMD Set to Saturation-1 END");
						break;
					case 0x56:
						ArduCAM.OV5642_set_Color_Saturation(Saturation_2);
						Serial.println("ACK CMD Set to Saturation-2 END");
						break;
					case 0x57:
						ArduCAM.OV5642_set_Color_Saturation(Saturation_3);
						Serial.println("ACK CMD Set to Saturation-3 END");
						break;
					case 0x58:
						ArduCAM.OV5642_set_Light_Mode(Saturation_4);
						Serial.println("ACK CMD Set to Saturation-4 END");
						break; 
					case 0x60:
						ArduCAM.OV5642_set_Brightness(Brightness4);
						Serial.println("ACK CMD Set to Brightness+4 END");
						break;
					case 0x61:
						ArduCAM.OV5642_set_Brightness(Brightness3);
						Serial.println("ACK CMD Set to Brightness+3 END");
						break; 
					case 0x62:
						ArduCAM.OV5642_set_Brightness(Brightness2);
						Serial.println("ACK CMD Set to Brightness+2 END");
						break; 
					case 0x63:
						ArduCAM.OV5642_set_Brightness(Brightness1);
						Serial.println("ACK CMD Set to Brightness+1 END");
						break; 
					case 0x64:
						ArduCAM.OV5642_set_Brightness(Brightness0);
						Serial.println("ACK CMD Set to Brightness+0 END");
						break; 
					case 0x65:
						ArduCAM.OV5642_set_Brightness(Brightness_1);
						Serial.println("ACK CMD Set to Brightness-1 END");
						break; 
					case 0x66:
						ArduCAM.OV5642_set_Brightness(Brightness_2);
						Serial.println("ACK CMD Set to Brightness-2 END");
						break; 
					case 0x67:
						ArduCAM.OV5642_set_Brightness(Brightness_3);
						Serial.println("ACK CMD Set to Brightness-3 END");
						break; 
					case 0x68:
						ArduCAM.OV5642_set_Brightness(Brightness_4);
						Serial.println("ACK CMD Set to Brightness-4 END");
						break;
					case 0x70:
						ArduCAM.OV5642_set_Contrast(Contrast4);
						Serial.println("ACK CMD Set to Contrast+4 END");
						break;
					case 0x71:
						ArduCAM.OV5642_set_Contrast(Contrast3);
						Serial.println("ACK CMD Set to Contrast+3 END");
						break; 
					case 0x72:
						ArduCAM.OV5642_set_Contrast(Contrast2);
						Serial.println("ACK CMD Set to Contrast+2 END");
						break; 
					case 0x73:
						ArduCAM.OV5642_set_Contrast(Contrast1);
						Serial.println("ACK CMD Set to Contrast+1 END");
						break; 
					case 0x74:
						ArduCAM.OV5642_set_Contrast(Contrast0);
						Serial.println("ACK CMD Set to Contrast+0 END");
						break; 
					case 0x75:
						ArduCAM.OV5642_set_Contrast(Contrast_1);
						Serial.println("ACK CMD Set to Contrast-1 END");
						break; 
					case 0x76:
						ArduCAM.OV5642_set_Contrast(Contrast_2);
						Serial.println("ACK CMD Set to Contrast-2 END");
						break; 
					case 0x77:
						ArduCAM.OV5642_set_Contrast(Contrast_3);
						Serial.println("ACK CMD Set to Contrast-3 END");
						break; 
					case 0x78:
						ArduCAM.OV5642_set_Contrast(Contrast_4);
						Serial.println("ACK CMD Set to Contrast-4 END");
						break;
					case 0x80: 
						ArduCAM.OV5642_set_hue(degree_180);
						Serial.println("ACK CMD Set to -180 degree END");
						break;   
					case 0x81: 
						ArduCAM.OV5642_set_hue(degree_150);
						Serial.println("ACK CMD Set to -150 degree END");
						break;  
					case 0x82: 
						ArduCAM.OV5642_set_hue(degree_120);
						Serial.println("ACK CMD Set to -120 degree END");
						break;  
					case 0x83: 
						ArduCAM.OV5642_set_hue(degree_90);
						Serial.println("ACK CMD Set to -90 degree END");
						break;   
					case 0x84: 
						ArduCAM.OV5642_set_hue(degree_60);
						Serial.println("ACK CMD Set to -60 degree END");
						break;   
					case 0x85: 
						ArduCAM.OV5642_set_hue(degree_30);
						Serial.println("ACK CMD Set to -30 degree END");
						break;  
					case 0x86: 
						ArduCAM.OV5642_set_hue(degree_0);
						Serial.println("ACK CMD Set to 0 degree END");
						break; 
					case 0x87: 
						ArduCAM.OV5642_set_hue(degree30);
						Serial.println("ACK CMD Set to 30 degree END");
						break;
					case 0x88: 
						ArduCAM.OV5642_set_hue(degree60);
						Serial.println("ACK CMD Set to 60 degree END");
						break;
					case 0x89: 
						ArduCAM.OV5642_set_hue(degree90);
						Serial.println("ACK CMD Set to 90 degree END");
						break;
					case 0x8a: 
						ArduCAM.OV5642_set_hue(degree120);
						Serial.println("ACK CMD Set to 120 degree END");
						break; 
					case 0x8b: 
						ArduCAM.OV5642_set_hue(degree150);
						Serial.println("ACK CMD Set to 150 degree END");
						break;
					case 0x90:
						ArduCAM.OV5642_set_Special_effects(Normal);
						Serial.println("ACK CMD Set to Normal END");
						break;
					case 0x91: 
						ArduCAM.OV5642_set_Special_effects(BW);
						Serial.println("ACK CMD Set to BW END");
						break;
					case 0x92: 
						ArduCAM.OV5642_set_Special_effects(Bluish);
						Serial.println("ACK CMD Set to Bluish END");
						break;
					case 0x93: 
						ArduCAM.OV5642_set_Special_effects(Sepia);
						Serial.println("ACK CMD Set to Sepia END");
						break;
					case 0x94: 
						ArduCAM.OV5642_set_Special_effects(Reddish);
						Serial.println("ACK CMD Set to Reddish END");
						break;
					case 0x95: 
						ArduCAM.OV5642_set_Special_effects(Greenish);
						Serial.println("ACK CMD Set to Greenish END");
						break;
					case 0x96: 
						ArduCAM.OV5642_set_Special_effects(Negative);
						Serial.println("ACK CMD Set to Negative END");
						break;
					case 0xA0: 
						ArduCAM.OV5642_set_Exposure_level(Exposure_17_EV);
						Serial.println("ACK CMD Set to -1.7EV END");
						break;  
					case 0xA1: 
						ArduCAM.OV5642_set_Exposure_level(Exposure_13_EV);
						Serial.println("ACK CMD Set to -1.3EV END");
						break;
					case 0xA2: 
						ArduCAM.OV5642_set_Exposure_level(Exposure_10_EV);
						Serial.println("ACK CMD Set to -1.0EV END");
						break; 
					case 0xA3: 
						ArduCAM.OV5642_set_Exposure_level(Exposure_07_EV);
						Serial.println("ACK CMD Set to -0.7EV END");
						break;
					case 0xA4: 
						ArduCAM.OV5642_set_Exposure_level(Exposure_03_EV);
						Serial.println("ACK CMD Set to -0.3EV END");
						break;
					case 0xA5: 
						ArduCAM.OV5642_set_Exposure_level(Exposure_default);
						Serial.println("ACK CMD Set to -Exposure_default END");
						break;
					case 0xA6: 
						ArduCAM.OV5642_set_Exposure_level(Exposure07_EV);
						Serial.println("ACK CMD Set to 0.7EV END");
						break;  
					case 0xA7: 
						ArduCAM.OV5642_set_Exposure_level(Exposure10_EV);
						Serial.println("ACK CMD Set to 1.0EV END");
						break;
					case 0xA8: 
						ArduCAM.OV5642_set_Exposure_level(Exposure13_EV);
						Serial.println("ACK CMD Set to 1.3EV END");
						break; 
					case 0xA9: 
						ArduCAM.OV5642_set_Exposure_level(Exposure17_EV);
						Serial.println("ACK CMD Set to 1.7EV END");
						break; 
					case 0xB0: 
						ArduCAM.OV5642_set_Sharpness(Auto_Sharpness_default);
						Serial.println("ACK CMD Set to Auto Sharpness default END");
						break; 
					case 0xB1: 
						ArduCAM.OV5642_set_Sharpness(Auto_Sharpness1);
						Serial.println("ACK CMD Set to Auto Sharpness +1 END");
						break; 
					case 0xB2: 
						ArduCAM.OV5642_set_Sharpness(Auto_Sharpness2);
						Serial.println("ACK CMD Set to Auto Sharpness +2 END");
						break; 
					case 0xB3: 
						ArduCAM.OV5642_set_Sharpness(Manual_Sharpnessoff);
						Serial.println("ACK CMD Set to Auto Manual Sharpness off END");
						break; 
					case 0xB4: 
						ArduCAM.OV5642_set_Sharpness(Manual_Sharpness1);
						Serial.println("ACK CMD Set to Auto Manual Sharpness +1 END");
						break;
					case 0xB5: 
						ArduCAM.OV5642_set_Sharpness(Manual_Sharpness2);
						Serial.println("ACK CMD Set to Auto Manual Sharpness +2 END");
						break; 
					case 0xB6: 
						ArduCAM.OV5642_set_Sharpness(Manual_Sharpness3);
						Serial.println("ACK CMD Set to Auto Manual Sharpness +3 END");
						break;
					case 0xB7: 
						ArduCAM.OV5642_set_Sharpness(Manual_Sharpness4);
						Serial.println("ACK CMD Set to Auto Manual Sharpness +4 END");
						break;
					case 0xB8: 
						ArduCAM.OV5642_set_Sharpness(Manual_Sharpness5);
						Serial.println("ACK CMD Set to Auto Manual Sharpness +5 END");
						break;  
					case 0xC0: 
						ArduCAM.OV5642_set_Mirror_Flip(MIRROR);
						Serial.println("ACK CMD Set to MIRROR END");
						break;  
					case 0xC1: 
						ArduCAM.OV5642_set_Mirror_Flip(FLIP);
						Serial.println("ACK CMD Set to FLIP END");
						break; 
					case 0xC2: 
						ArduCAM.OV5642_set_Mirror_Flip(MIRROR_FLIP);
						Serial.println("ACK CMD Set to MIRROR&FLIP END");
						break;
					case 0xC3: 
						ArduCAM.OV5642_set_Mirror_Flip(Normal);
						Serial.println("ACK CMD Set to Normal END");
						break;
					case 0xD0: 
						ArduCAM.OV5642_set_Compress_quality(high_quality);
						Serial.println("ACK CMD Set to high quality END");
						break;
					case 0xD1: 
						ArduCAM.OV5642_set_Compress_quality(default_quality);
						Serial.println("ACK CMD Set to default quality END");
						break;
					case 0xD2: 
						ArduCAM.OV5642_set_Compress_quality(low_quality);
						Serial.println("ACK CMD Set to low quality END");
						break;
					case 0xE0: 
						ArduCAM.OV5642_Test_Pattern(Color_bar);
						Serial.println("ACK CMD Set to Color bar END");
						break;
					case 0xE1: 
						ArduCAM.OV5642_Test_Pattern(Color_square);
						Serial.println("ACK CMD Set to Color square END");
						break;
					case 0xE2: 
						ArduCAM.OV5642_Test_Pattern(BW_square);
						Serial.println("ACK CMD Set to BW square END");
						break;
					case 0xE3: 
						ArduCAM.OV5642_Test_Pattern(DLI);
						Serial.println("ACK CMD Set to DLI END");
						break;
					default:
						break;
				}
				temp = 0xff;
				if (start_capture == 2) {
					//Flush the FIFO
					ArduCAM.flush_fifo();
					ArduCAM.clear_fifo_flag();
					//Start capture
					ArduCAM.start_capture();
					start_capture = 0;
				}
				if (ArduCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
					uint32_t length = 0;
					length = ArduCAM.read_fifo_length();
					if ((length >= MAX_FIFO_SIZE) | (length == 0)) {
						ArduCAM.clear_fifo_flag();
						start_capture = 2;
						continue;
					}
					ArduCAM.CS_LOW();
					ArduCAM.set_fifo_burst();//Set fifo burst mode
					temp = SPI.transfer(0x00);
					length --;
					while ( length-- ) {
						temp_last = temp;
						temp =  SPI.transfer(0x00);
						if (is_header == true) {
							Serial.write_byte(temp);
						} else if ((temp == 0xD8) & (temp_last == 0xFF)) {
							is_header = true;
							Serial.println("ACK IMG END");
							Serial.write_byte(temp_last);
							Serial.write_byte(temp);
						}
						if ( (temp == 0xD9) && (temp_last == 0xFF) ) break; //If find the end ,break while,
						delayMicroseconds(100);
					}
					ArduCAM.CS_HIGH();
					ArduCAM.clear_fifo_flag();
					start_capture = 2;
					is_header = false;
				}
			}
		}
	} else if (mode == 3) {
		if (start_capture == 3) {
		    //Flush the FIFO
			ArduCAM.flush_fifo();
			ArduCAM.clear_fifo_flag();
		    //Start capture
			ArduCAM.start_capture();
			start_capture = 0;
		}
		if (ArduCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
			uint32_t length = 0;
 			Serial.println("ACK CMD CAM Capture Done. END");
			delay(50);
			length = ArduCAM.read_fifo_length();
			if (length >= MAX_FIFO_SIZE) {
				Serial.println("ACK CMD Over size. END");
				ArduCAM.clear_fifo_flag();
				return;
			}
			if (length == 0 ) { //0 kb
   				Serial.println("ACK CMD Size is 0. END");
				ArduCAM.clear_fifo_flag();
				return;
			}
			ArduCAM.CS_LOW();
			ArduCAM.set_fifo_burst();//Set fifo burst mode

			Serial.write_byte(0xFF);
			Serial.write_byte(0xAA);
			for (temp = 0; temp < BMPIMAGEOFFSET; temp++) {
   				Serial.write_byte(bmp_header[temp]);
			}
			//SPI.transfer(0x00);
			{
				char VH, VL;
				int i = 0, j = 0;
				for (i = 0; i < 240; i++) {
					for (j = 0; j < 320; j++) {
						VH = SPI.transfer(0x00);;
						VL = SPI.transfer(0x00);;
						Serial.write_byte(VL);
						delayMicroseconds(100);
						Serial.write_byte(VH);
						delayMicroseconds(100);
					}
				}
			}
 			Serial.write_byte(0xBB);
 			Serial.write_byte(0xCC);
			ArduCAM.CS_HIGH();
			//Clear the capture done flag
			ArduCAM.clear_fifo_flag();
			mode = 0;
		}
	}
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
			Serial.write_byte(temp);
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
