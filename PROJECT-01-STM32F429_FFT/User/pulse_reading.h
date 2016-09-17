#ifndef PULSE_READING_H
#define PULSE_READING_H

#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_general.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "init_periph_irqs.h"
#include "detect_whistle.h"
#include "init_periph_irqs.h"
#include "cup_detection.h"
#include "pulse_reading.h"
#include "leds.h"
#include "usart.h"

#define TARGET_BPM 100
#define TARGET_TIME 10

void INTTIM5_Config(void);
void ReadPulse(void);
uint16_t getQS(void);
void setQS(uint16_t s);

#endif
