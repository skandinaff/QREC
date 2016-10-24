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

#define LED_RCC RCC_AHB1Periph_GPIOE
#define LED_GPIO GPIOE
#define LED_1 GPIO_Pin_8
#define LED_2 GPIO_Pin_9
#define LED_3 GPIO_Pin_10
#define LED_4 GPIO_Pin_11
#define LED_5 GPIO_Pin_12

#define LED_SEG_GPIO	GPIOE
#define LATCH_PIN 		GPIO_Pin_5
#define CLOCK_PIN 		GPIO_Pin_6
#define DATA_PIN	  	GPIO_Pin_7
#define STATE_LED			GPIO_Pin_4
#define NUM_OF_REG		3

void addToBuffer(int digit, bool dot, bool dot2);
void shiftOut(int myDataPin, int myClockPin, uint8_t myDataOut);
void clearBuffer(void);
void Configure_12V_LEDS(void);
void Configure_Onboard_LEDS(void);
void Configure_LED_indicator(void);
void BlinkOnboardLED(uint8_t L);
void Control_12V_LEDs(void);
void Test_7Seg(void);
void ClearOnboardLEDS(void);
void set_dot2_always_on(void);

#endif
