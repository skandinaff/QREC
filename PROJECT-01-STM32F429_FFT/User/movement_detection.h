#ifndef MOVEMENT_DETECTION_H
#define MOVEMENT_DETECTION_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "tm_stm32f4_ili9341_ltdc.h"
#include "stm32f4xx_gpio.h"

#include "usart.h"
#include "init_periph_irqs.h"
#include "detect_whistle.h"
#include "init_periph_irqs.h"
#include "cup_detection.h"
#include "pulse_reading.h"

void MotionDetection(void);

#endif
