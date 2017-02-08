#include "voltage_check_ide.h"		// Additional Header

void print_voltage(uint8_t level) {
	Serial.print("Current VDD voltage : ");
	switch (level) {
	case VLS_UNDER_1_898:
		Serial.println("< 1.898V");
		break;
	case VLS_1_898:
		Serial.println("1.898 ~ 2.000V");
		break;
	case VLS_2_000:
		Serial.println("2.000 ~ 2.093V");
		break;
	case VLS_2_093:
		Serial.println("2.093 ~ 2.196V");
		break;
	case VLS_2_196:
		Serial.println("2.196 ~ 2.309V");
		break;
	case VLS_2_309:
		Serial.println("2.309 ~ 2.409V");
		break;
	case VLS_2_409:
		Serial.println("2.409 ~ 2.605V");
		break;
	case VLS_2_605:
		Serial.println("2.605 ~ 2.800V");
		break;
	case VLS_2_800:
		Serial.println("2.800 ~ 3.068V");
		break;
	case VLS_3_068:
		Serial.println("3.068 ~ 3.394V");
		break;
	case VLS_3_394:
		Serial.println("3.394 ~ 3.797V");
		break;
	case VLS_3_797:
		Serial.println("3.797 ~ 4.226V");
		break;
	case VLS_4_226:
		Serial.println("4.226 ~ 4.667V");
		break;
	case VLS_4_667:
		Serial.println("> 4.667V");
		break;
	default:
		Serial.println("unknown");
		break;
	}
}

void setup() {
	Serial.begin(115200);
}

void loop() {
	uint8_t level;

	level = voltage_check(VLS_4_667);		// start checking from 4.667V
	print_voltage(level);
	sleep(3000);
}

