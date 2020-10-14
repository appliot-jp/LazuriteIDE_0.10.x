/* FILE NAME: buttonDetect.c
 *
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <lazurite.h>
#include <driver_extirq.h>
#include "buttonDetect.h"
#include "MsTimer2.h"
extern const unsigned char digital_pin_to_port[MAX_PIN_NO+1];
bool msTimer2Enb = false;

static struct button_config* btn_cfg;


static void irq_btn(int mode);
static void extirq_btn() {
	irq_btn(0);
}
static void timerirq_btn() {
	irq_btn(1);
}

static void irq_btn(int mode) {
	uint32_t now;
	uint8_t needTimer = false;
	int i;
	now = millis();
	for(i=0;i<btn_cfg->num;i++) {
		uint8_t pin = (uint8_t)digitalRead(btn_cfg->btn[i].pin);
		switch(btn_cfg->btn[i].type) {
			case PUSH:
				if(pin == LOW) {
					needTimer = true;
					switch(btn_cfg->btn[i].state) {
						case 0:
							btn_cfg->btn[i].state = 1;
							break;
						case 1:
							if(mode == 1) {
								btn_cfg->btn[i].count++;
								if(btn_cfg->btn[i].count >= btn_cfg->chat) {
									btn_cfg->btn[i].count=0;
									btn_cfg->btn[i].state = 2;
									bitSet(btn_cfg->event,btn_cfg->btn[i].extirq);
									btn_cfg->wait_flag = true;
								}
							}
							break;
						case 2:
							if((mode == 1) && (btn_cfg->cont != 0)) {
								btn_cfg->btn[i].count++;
								if(btn_cfg->btn[i].count >= btn_cfg->cont) {
									btn_cfg->btn[i].count=0;
									btn_cfg->btn[i].state = 3;
								}
							}
							break;
						case 3:
							if(mode == 1) {
								btn_cfg->btn[i].count++;
								if(btn_cfg->btn[i].count >= btn_cfg->cont_interval) {
									btn_cfg->btn[i].count=0;
									bitSet(btn_cfg->event,btn_cfg->btn[i].extirq);
									btn_cfg->wait_flag = true;
								}
							}
							break;
						default:
							btn_cfg->btn[i].state=0;
							break;
					}
				} else {
					if(btn_cfg->btn[i].state>=2) {
						btn_cfg->wait_flag = true;
					}
					btn_cfg->btn[i].state = 0;
				}
				break;
			case RELEASE:
				if(pin == LOW) {
					needTimer = true;
					switch(btn_cfg->btn[i].state) {
						case 0:
							btn_cfg->btn[i].state = 1;
							break;
						case 1:
							if(mode == 1) {
								btn_cfg->btn[i].count++;
								if(btn_cfg->btn[i].count >= btn_cfg->chat) {
									btn_cfg->btn[i].count=0;
									btn_cfg->btn[i].state = 2;
								}
							}
							break;
						case 2:
						case 3:
							break;
						default:
							btn_cfg->btn[i].state=0;
							break;
					}
				} else {
					if(btn_cfg->btn[i].state >= 2) {
						bitSet(btn_cfg->event,btn_cfg->btn[i].extirq);
						btn_cfg->wait_flag = true;
					}
					btn_cfg->btn[i].state = 0;
					btn_cfg->btn[i].count = 0;
				}
				break;
			default:
				break;
		}
	}
	if((needTimer == true) && (msTimer2Enb == false)) {
		msTimer2Enb = true;
		timer2.set(btn_cfg->timer_unit,timerirq_btn);
		timer2.start();
	} else if((needTimer == false) && (msTimer2Enb == true)) {
		msTimer2Enb = false;
		timer2.stop();
	}
}

void buttonDetect(struct button_config* params) {
	int i;
	btn_cfg = params;
	btn_cfg->event = 0;
	btn_cfg->wait_flag = false;
	for(i=0;i<btn_cfg->num;i++){
		pinMode(btn_cfg->btn[i].pin,INPUT_PULLUP);
		drv_attachInterrupt(digital_pin_to_port[btn_cfg->btn[i].pin],btn_cfg->btn[i].extirq,extirq_btn,CHANGE,false,false);
		btn_cfg->btn[i].state = 0;
		btn_cfg->btn[i].count = 0;
	}
}

