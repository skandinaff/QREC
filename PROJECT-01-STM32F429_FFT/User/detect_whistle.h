#ifndef DETECT_WHISTLE_H
#define DETECT_WHISTLE_H

#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_sdram.h"
#include "tm_stm32f4_dac_signal.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "arm_math.h"
#include "init_periph_irqs.h"
#include "cup_detection.h"

/* FFT settings */
#define SAMPLES					    			512 			/* 256 real party and 256 imaginary parts */
#define FFT_SIZE				    			SAMPLES / 2		/* FFT size is always the same size as we have samples, so 256 in our case */
#define FFT_BAR_MAX_HEIGHT		    120 		    /* 120 px on the LCD */
#define CLAP_AMPLITUDE 						33
#define SILENCE_AMPLITUDE					5.5						// 4 is the absolute minimum, barele above backround noise

typedef struct {
		float32_t maxValue;							
		uint32_t maxIndex;	
		uint16_t i;	
} FFT_OUT_t;

void DetectWhistle(void);
void DrawBar(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t foreground, uint16_t background);
FFT_OUT_t ComputeFFT(void);
void DetectClap(void);
void SilenceDetection(void);

#endif
