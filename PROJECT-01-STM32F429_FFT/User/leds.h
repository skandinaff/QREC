#ifndef LEDS_H
#define LEDS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "stm32f4xx_gpio.h"

#include "usart.h"
#include "init_periph_irqs.h"
#include "detect_whistle.h"
#include "init_periph_irqs.h"
#include "cup_detection.h"
#include "pulse_reading.h"

#define LED_SEG_GPIO	GPIOE
#define LATCH_PIN 		GPIO_Pin_5
#define CLOCK_PIN 		GPIO_Pin_6
#define DATA_PIN	  	GPIO_Pin_7
#define NUM_OF_REG		3

void addToBuffer(int digit);
void shiftOut(int myDataPin, int myClockPin, uint8_t myDataOut);
void clearBuffer(void);

void BlinkOnboardLED(uint8_t L);
void Control_12V_LEDs(void);
void Test_7Seg(void);
	
#endif
