#include "detect_whistle.h"


/* Global variables */
float32_t Input[SAMPLES];
float32_t Output[FFT_SIZE];
uint8_t claps = 0;
float32_t silence_thresh = SILENCE_AMPLITUDE;
float32_t silence_thresh_avg;
uint8_t N = 0, K = 0;
bool silence_thresh_is_set = 0;

void DetectWhistle(void) {
    //TM_ILI9341_Puts(150, 10, "           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    FFT_OUT_t in;

    in = ComputeFFT();

    /* This is me trying to output frequency as a number */
    //TM_ILI9341_Puts(10, 10, "Peak Freq:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    float32_t freq;


    /*** Thus code takes frequency at a maximum amplitude and detects if it's in range of whistle ***/
    freq = in.maxIndex * (45000 / 256);  // If a condition is added here, that cut's off low frequencies, just reaching
    // certain frequency is enough to trigger the event
    
		//char str[64];
    //sprintf(str, "%.0f Hz", freq);

    if (in.maxIndex > 0) //TM_ILI9341_Puts(150, 10, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    if (freq >= 1000 && freq <= 2400) {  // Whistle withing range
        TIM_Cmd(TIM2, ENABLE);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
      //  TM_DISCO_LedOff(LED_RED);
      //  TM_DISCO_LedOn(LED_GREEN);
    }
    if (freq < 1000) {  // Cutting off frequencies less than 900
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
       // TM_DISCO_LedOn(LED_RED);
       // TM_DISCO_LedOff(LED_GREEN);
        //TM_ILI9341_Puts(10, 25, "                       ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
      //setSecondsCount(0);  
			TIM_Cmd(TIM2, DISABLE);
    }
		if (freq > 2400) { // Cutting off frequencies more than 2400
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
       // TM_DISCO_LedOn(LED_RED);
       // TM_DISCO_LedOff(LED_GREEN);
        //TM_ILI9341_Puts(10, 25, "                       ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
      //setSecondsCount(0);  
			TIM_Cmd(TIM2, DISABLE);
    }

		if (getSecondCount() > 60) setSecondsCount(0);
		
    if (getSecondCount() > WHISTLE_TIME) {
        //TM_ILI9341_Puts(10, 25, "You Whistled for N sec!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        Delayms(100);
        set_task_counter(get_task_counter() + 1);
        setSecondsCount(0);
    }
		
		
    // Display data on LCD 
		/*
    for (in.i = 0; in.i < FFT_SIZE / 2; in.i++) {
        // Draw FFT results 
        DrawBar(30 + 2 * in.i,
                220,
                FFT_BAR_MAX_HEIGHT,
                (uint16_t) in.maxValue,
                (float32_t) Output[(uint16_t) in.i],
                0x1234,
                0xFFFF
        );
    }
		*/
		
}


FFT_OUT_t ComputeFFT(void) {
    FFT_OUT_t out;

    arm_cfft_radix4_instance_f32 S;    /* ARM CFFT module */
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
        Input[(uint16_t) out.i] =
                (float32_t)((float32_t) TM_ADC_Read(ADC1, ADC_Channel_3) - (float32_t) 2048.0) / (float32_t) 2048.0;
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
		
		arm_min_f32(Output, FFT_SIZE, &out.minValue, &out.minIndex);

    return out;
}


void DetectClap(void) {
    //	TM_ILI9341_Puts(180, 10,"           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    FFT_OUT_t in;
    in = ComputeFFT();
		

/*  Display Section
    //TM_ILI9341_Puts(10, 10, "Peak Ampl:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    //TM_ILI9341_Puts(10, 25, "Claps detected:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    //TM_ILI9341_Puts(10, 40, "Seconds passed:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);


    char str[16];
    char str2[8];
    char str3[8];

    sprintf(str, "%.2f", in.maxValue);
    sprintf(str2, "%d", getClaps());
    sprintf(str3, "%d", getSecondCount());

    //TM_ILI9341_Puts(180, 10, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    //TM_ILI9341_Puts(180, 25, str2, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    //TM_ILI9341_Puts(180, 40, str3, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				
*/

// TODO: remove this in production. For tests only
//		addToBuffer(in.maxValue,false,false);
// ***		
		
		Delayms(DELAY_VALUE); // This delay is essential for correct timing. Default value = 10


    float32_t freq;

    freq = in.maxIndex * (45000 / 256);  // Obtaining frequency from the signal

		if(freq > 500 && freq < 1200) {

			if (getClaps() == 0) {
					if (in.maxValue > CLAP_AMPLITUDE) {
							TIM_Cmd(TIM2, ENABLE);
					}
			}
		
			if (in.maxValue > CLAP_AMPLITUDE) {
				setClaps(getClaps() + 1);
				BlinkOnboardLED(3);
			}

			if(getSecondCount() > 60) {
				//TM_ILI9341_Puts(180, 40, "   ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				setSecondsCount(0);
				setClaps(0);
			}
			
			if (getSecondCount() > 5 && getClaps() > 10) {
					TIM_Cmd(TIM2, DISABLE);
					setSecondsCount(0);
					setClaps(0);
					//TM_ILI9341_Puts(10, 60, "You did clap 3 times", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
					set_task_counter(get_task_counter() + 1);
					Delayms(100);
			}
		}
}


void SilenceDetection(void) {
	
    FFT_OUT_t in;
    in = ComputeFFT();

		/*	//Display Section
	  TM_ILI9341_Puts(10, 10, "Peak Ampl:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		TM_ILI9341_Puts(10, 25, "Current Thresh:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(10, 40, "Seconds passed:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	  char amp_str[16];
    char time_str[8];
		char thresh_str[8];
    sprintf(amp_str, "%.2f", in.maxValue);
    sprintf(time_str, "%d", getSecondCount());
		sprintf(thresh_str, "%d", getSilenceThresh());
		TM_ILI9341_Puts(180, 25, "    ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 10, amp_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		TM_ILI9341_Puts(180, 25, thresh_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(180, 40, time_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		*/
	
// TODO: remove this in production. For tests only
			addToBuffer(getSecondCount(),false,false);
	
			//addToBuffer(in.maxValue,false,false);
// ***

	Delayms(DELAY_VALUE); // This delay is essential for correct timing. Default value = 10
	
		if(N < SIL_AVG_SAMPLES){
			silence_thresh_avg += in.maxValue;
			//TM_ILI9341_Puts(10, 65, "Acquiring threshold value", &TM_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE);
		}
		if(N >= SIL_AVG_SAMPLES && silence_thresh_is_set == 0){
			setSilenceThresh( (silence_thresh_avg/SIL_AVG_SAMPLES) + CORRECTION_VALUE ); 			// Add correction value, 4 seems to be optimal
			addToBuffer(getSilenceThresh(),false,false);
			Delayms(100);
			//TM_ILI9341_Puts(10, 65, "                         ", &TM_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE);
		} 
		if(N<SIL_AVG_SAMPLES + 1) N++;

		float32_t freq;

    freq = in.maxIndex * (45000 / 256); 

		if(freq > 100 && freq < 6000) {
			
			if (in.maxValue < getSilenceThresh())
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);				
					TIM_Cmd(TIM2, ENABLE);
			}
			
			if (in.maxValue > getSilenceThresh() && silence_thresh_is_set == 1) {
				setSecondsCount(0); 
				GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
				GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);	
				K++;
			}																							// Here, instead of comparing MAX to a threshold (SILENCE_AMPLITEUD) 
																										// Should be comparing average between min and max 
																										// from a previous iterationto a max 
		//? }
			
			if (getSecondCount() > SILENCE_TIME) { //Silence time
					TIM_Cmd(TIM2, DISABLE);
					//TM_ILI9341_Puts(10, 60, "You were silent for 10 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

					set_task_counter(get_task_counter() + 1);
					setSecondsCount(0);
					setSilenceThresh(SILENCE_AMPLITUDE);
					Delayms(100);
			}
			if(K > 5) {
				resetSilenceThresh();
				K=0;
			}
			
		
		
}

/* Draw bar for LCD */
/* Simple library to draw bars */
void DrawBar(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t foreground,
        uint16_t background) {
    uint16_t height;
    height = (uint16_t)((float32_t) value / (float32_t) maxValue * (float32_t) maxHeight);
    if (height == maxHeight) {
        //TM_ILI9341_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
    } else if (height < maxHeight) {
        //TM_ILI9341_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
        //TM_ILI9341_DrawLine(bottomX, bottomY - height, bottomX, bottomY - maxHeight, background);
    }
}

void setClaps(uint8_t c) {
	claps = c;
}

uint8_t getClaps(void) {
	return claps;
}

void setSilenceThresh(float32_t st) {
	silence_thresh_is_set = 1;
	silence_thresh = st;
}

float32_t getSilenceThresh(void) {
	return silence_thresh;
}

bool is_silence_thresh_set(void){
	return silence_thresh_is_set;
}

void resetSilenceThresh(void){
	N = 0;
	silence_thresh_avg = 0;
} 
