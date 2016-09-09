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

#include "tm_stm32f4_pwm.h"

//TM_PWM_TIM_t TIM2_Data;

void BlinkOnboardLED(uint8_t L);

#endif