

#include "detect_whistle.h"


/* Global variables */
float32_t Input[SAMPLES];
float32_t Output[FFT_SIZE];
uint8_t claps = 0;	


void DetectWhistle(void){

		flags_t cflags;
	
		cflags = get_flags();
	
		TM_ILI9341_Puts(150, 10,"           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

		
		FFT_OUT_t in;
		
		in = ComputeFFT();
	
		/* This is me trying to output frequency as a number */
		TM_ILI9341_Puts(10, 10, "Peak Freq:",&TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		
			
		
		
		float32_t freq;
	

   /*** Thus code takes frequency at a maximum amplitude and detects if it's in range of whistle ***/
		freq = in.maxIndex * (45000 / 256);  // If a condition is added here, that cut's off low frequencies, just reaching 
																											// certain frequency is enough to trigger the event
		char str[64];	
		sprintf(str, "%.0f Hz", freq);
		

		
		if(in.maxIndex > 0) TM_ILI9341_Puts(150, 10, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				


		if (freq >= 1100) {
			TIM_Cmd(TIM2, ENABLE);
			TM_DISCO_LedOff(LED_RED);
			TM_DISCO_LedOn(LED_GREEN);
		}
		if(freq < 900) {
			TM_DISCO_LedOn(LED_RED);
			TM_DISCO_LedOff(LED_GREEN);
			TM_ILI9341_Puts(10, 25, "                       ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			TIM_Cmd(TIM2, DISABLE);
		}

		if (getSecondCount() > 5) {
			TM_ILI9341_Puts(10, 25, "You Whistled for 5 sec!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			Delayms(2000);
			cflags.detect_whistle = true;
			setSecondsCount(0);
		}

		CheckIfAllCupsPresent();
		
		/* Display data on LCD */
		for (in.i = 0; in.i < FFT_SIZE / 2; in.i++) {
			/* Draw FFT results */
			DrawBar(30 + 2 * in.i,
					220,
					FFT_BAR_MAX_HEIGHT,
					(uint16_t)in.maxValue,
					(float32_t)Output[(uint16_t)in.i],
					0x1234,
					0xFFFF
			);
		}
		set_flags(cflags);

}

FFT_OUT_t ComputeFFT(void){
		
		FFT_OUT_t out;
	
		arm_cfft_radix4_instance_f32 S;	/* ARM CFFT module */
		//float32_t maxValue;							/* Max FFT value is stored here */
		//uint32_t maxIndex;							/* Index in Output array where max value is */
		//uint16_t i;
		
		/* This part should be done with DMA and timer for ADC treshold */
		/* Actually, best solution is double buffered DMA with timer for ADC treshold */
		/* But this is only for show principle on how FFT works */
		for (out.i = 0; out.i < SAMPLES; out.i += 2) {
			/* Each 22us ~ 45kHz sample rate */
			Delay(21);
			
			/* We assume that sampling and other stuff will take about 1us */
			
			/* Real part, must be between -1 and 1 */
			Input[(uint16_t)out.i] = (float32_t)((float32_t)TM_ADC_Read(ADC1, ADC_Channel_3) - (float32_t)2048.0) / (float32_t)2048.0;
			/* Imaginary part */
			Input[(uint16_t)(out.i + 1)] = 0;
		}
		
		/* Initialize the CFFT/CIFFT module, intFlag = 0, doBitReverse = 1 */
		arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1);
		
		/* Process the data through the CFFT/CIFFT module */
		arm_cfft_radix4_f32(&S, Input);
		
		/* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
		arm_cmplx_mag_f32(Input, Output, FFT_SIZE);
		
		/* Calculates maxValue and returns corresponding value */
		arm_max_f32(Output, FFT_SIZE, &out.maxValue, &out.maxIndex);
		
		return out;
}

void DetectClap(void){
	
	flags_t cflags;
	cflags = get_flags();
	
//	TM_ILI9341_Puts(180, 10,"           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	FFT_OUT_t in;
	in = ComputeFFT();

	
	TM_ILI9341_Puts(10, 10, "Peak Ampl:",&TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	TM_ILI9341_Puts(10, 25, "Claps detected:",&TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	TM_ILI9341_Puts(10, 40, "Seconds passed:",&TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	
	char str[16];
	char str2[8];
	char str3[8];
	
	sprintf(str, "%.2f", in.maxValue);
	sprintf(str2, "%d", claps);	
	sprintf(str3, "%d", getSecondCount());	
		
	TM_ILI9341_Puts(180, 10, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	TM_ILI9341_Puts(180, 25, str2, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	TM_ILI9341_Puts(180, 40, str3, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	
  /* // Simple implementation, that works, though not very flexible
	if(in.maxValue > CLAP_AMPLITUDE) { // First clap detected
		Delayms(1000);
		claps++;
	}
	*/
	
	if(claps == 0){
		if(in.maxValue > CLAP_AMPLITUDE){
			TIM_Cmd(TIM2, ENABLE);
		}
	}
	if(in.maxValue > CLAP_AMPLITUDE) claps++;
	

	if(getSecondCount() > 5 && claps > 10) {
		TIM_Cmd(TIM2, DISABLE);
		setSecondsCount(0);
		TM_ILI9341_Puts(10, 60, "You did clap 3 times", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
		cflags.detect_clap = true;
		Delayms(1000);
	}
		
	CheckIfAllCupsPresent();
	set_flags(cflags);
	
}

void SilenceDetection(void){

	flags_t cflags;
	cflags = get_flags();
	
	FFT_OUT_t in;
	in = ComputeFFT();
	
	TM_ILI9341_Puts(10, 10, "Peak Ampl:",&TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	TM_ILI9341_Puts(10, 25, "Seconds passed:",&TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	Delayms(350);
	
	char str[16];
	char str3[8];
	
	sprintf(str, "%.2f", in.maxValue);
	sprintf(str3, "%d", getSecondCount());	
		
	TM_ILI9341_Puts(180, 10, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	TM_ILI9341_Puts(180, 25, str3, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	
	
	if(in.maxValue < SILENCE_AMPLITUDE){
			TIM_Cmd(TIM2, ENABLE);
	}
	if(in.maxValue > SILENCE_AMPLITUDE) setSecondsCount(0);
	
	if(getSecondCount() > 10) {
		TIM_Cmd(TIM2, DISABLE);
		TM_ILI9341_Puts(10, 60, "You were silent for 10 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
		cflags.detect_silence = true;
		Delayms(1000);
	}	
	
	
	
	CheckIfAllCupsPresent();
	set_flags(cflags);
	
}

/* Draw bar for LCD */
/* Simple library to draw bars */
void DrawBar(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t foreground, uint16_t background) {
	uint16_t height;
	height = (uint16_t)((float32_t)value / (float32_t)maxValue * (float32_t)maxHeight);
	if (height == maxHeight) {
		TM_ILI9341_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
	} else if (height < maxHeight) {
		TM_ILI9341_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
		TM_ILI9341_DrawLine(bottomX, bottomY - height, bottomX, bottomY - maxHeight, background);
	}
}

