#ifndef DETECT_WHISTLE_H
#define DETECT_WHISTLE_H

#include "tm_stm32f4_delay.h"
//#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_disco.h"
//#include "tm_stm32f4_sdram.h"
#include "tm_stm32f4_dac_signal.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "arm_math.h"
#include "init_periph_irqs.h"
#include "cup_detection.h"
#include "leds.h"

/* FFT settings */
#define SAMPLES					    			512 			/* 256 real party and 256 imaginary parts */
#define FFT_SIZE				    			SAMPLES / 2		/* FFT size is always the same size as we have samples, so 256 in our case */
#define FFT_BAR_MAX_HEIGHT		    120 		    /* 120 px on the LCD */
/* Whistle settings */
#define WHISTLE_TIME							10
/* Clap settings */
#define CLAP_AMPLITUDE 						13
/* Silence settings */
#define SILENCE_AMPLITUDE					5						// 3 is the absolute minimum, barele above backround noise 
#define SILENCE_TIME							15
#define SIL_AVG_SAMPLES						12					// previously was 10
#define CORRECTION_VALUE					0 					// was 3
/* Common settings */
#define DELAY_VALUE								20

typedef struct {
		float32_t maxValue;							
		uint32_t maxIndex;	
		float32_t minValue;
		uint32_t minIndex;
		uint16_t i;	
} FFT_OUT_t;

void DetectWhistle(void);
void DrawBar(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t foreground, uint16_t background);
FFT_OUT_t ComputeFFT(void);
void DetectClap(void);
void SilenceDetection(void);
void setClaps(uint8_t c);
uint8_t getClaps(void);
void setSilenceThresh(float32_t st);
float32_t getSilenceThresh(void);
void resetSilenceThresh(void);
bool is_silence_thresh_set(void);

#endif
