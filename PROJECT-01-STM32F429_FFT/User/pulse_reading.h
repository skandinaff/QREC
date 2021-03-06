#ifndef PULSE_READING_H
#define PULSE_READING_H

#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_disco.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "init_periph_irqs.h"
#include "detect_whistle.h"
#include "init_periph_irqs.h"
#include "cup_detection.h"
#include "pulse_reading.h"

#define TARGET_BPM 120

void ReadPulse(void);
uint16_t getQS(void);
void setQS(uint16_t s);

#endif
