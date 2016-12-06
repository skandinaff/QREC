#include "detect_whistle.h"


/* Global variables */
float32_t Input[SAMPLES];
float32_t Output[FFT_SIZE];
float32_t OutputInVoiceRange[18]; 
float32_t OutputInClapRange[CLAP_RANGE]; //3 - 7
uint32_t claps = 0;
float32_t silence_thresh = SILENCE_AMPLITUDE;
float32_t silence_thresh_avg = 0;
uint32_t N = 0, K = 0;

float32_t freq;
float32_t inMaxValueInRange;

bool silence_thresh_is_set = false;

char silence_thresh_str[15];
char silence_value_str[15];
char silence_time_str[15];
char whistle_time_str[15];
char whistle_freq_str[64];

int in_max_value_int;
int silence_thresh_incr;
int silence_thresh_incr_old;
float32_t correction_value = CORRECTION_VALUE;
float32_t silence_value_biggest;
float32_t silence_value_biggest_old = 0;

char silence_value_biggest_str[15];
char silence_thresh_time_str[15];
char silence_energy_str[15];

uint16_t Sil_SignalToDraw;
uint16_t Sil_PrevSignalToDraw;

uint16_t raw_adc;

char raw_adc_str[15];

void DetectWhistle(void) {

    FFT_OUT_t in;

    in = ComputeFFT();
    
		float32_t freq;



    /*** Thus code takes frequency at a maximum amplitude and detects if it's in range of whistle ***/
    freq = in.maxIndex * (45000 / 256); 
    
		LCD_Puts("Max Fq: ", 1, 1, WHITE, BLACK,1,1);
    sprintf(whistle_freq_str, "%.2f Hz", freq);	
		LCD_Puts(whistle_freq_str, 60, 1, WHITE, BLACK,1,1);
	
		LCD_Puts("T: ", 1, 10, WHITE, BLACK,1,1);
    sprintf(whistle_time_str, "%4d", getSecondCount());	
		LCD_Puts(whistle_time_str, 60, 10, WHITE, BLACK,1,1);
		


    if ((freq >= 1000 && freq <= 3000)) {  // Whistle withing range
      TIM_Cmd(TIM2, ENABLE);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);

    }
    if (freq < 1000) {  // Cutting off frequencies less than
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			TIM_Cmd(TIM2, DISABLE);
    }
		if (freq > 3000) { // Cutting off frequencies more than 2400
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			TIM_Cmd(TIM2, DISABLE);
    }

		if (getSecondCount() > 60) setSecondsCount(0);
		
    if (getSecondCount() > WHISTLE_TIME) {
        Delayms(100);
        set_task_counter(get_task_counter() + 1);
        setSecondsCount(0);
    }
		
		
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

		for(int l = 0; l<=18; l++){
			OutputInVoiceRange[l]=0;
		}
				
		for(int j = 1; j<=18; j++){
			OutputInVoiceRange[j-1] += Output[j];
		}
		
		for(int k = 0; k<=CLAP_RANGE; k++){
			OutputInClapRange[k]=0;
		}
				
		for(int m = 2; m<=CLAP_RANGE; m++){
			OutputInClapRange[m-1] += Output[m];
		}

		arm_mean_f32(OutputInVoiceRange, FFT_SIZE, &out.VoiceEnergy);
		arm_mean_f32(OutputInClapRange, CLAP_RANGE, &out.ClapEnergy);

    return out;
}


void DetectClap(void) {

    FFT_OUT_t in;
    in = ComputeFFT();

		LCD_Puts("Max Amp: ", 1, 1, WHITE, BLACK,1,1);
		LCD_Puts("Clp det: ", 1, 10, WHITE, BLACK,1,1);
		LCD_Puts("T: ", 1, 20, WHITE, BLACK,1,1);

    char str[16];
    char str2[8];
    char str3[8];

    sprintf(str, "%.2f", in.maxValue);
    sprintf(str2, "%d", getClaps());
    sprintf(str3, "%d", getSecondCount());
		
		LCD_Puts(str, 60, 1, WHITE, BLACK,1,1);
		LCD_Puts(str2, 60, 10, WHITE, BLACK,1,1);
		LCD_Puts(str3, 60, 20, WHITE, BLACK,1,1);

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
				setSecondsCount(0);
				setClaps(0);
			}
			
			if (getSecondCount() > 5 && getClaps() > CLAP_TIMES) {
					TIM_Cmd(TIM2, DISABLE);
					setSecondsCount(0);
					setClaps(0);
					set_task_counter(get_task_counter() + 1);
					set_game_result(COMPLETED); 
					Delayms(100);
			}
		}
}

void DetectClapByEnergy(void) {

    FFT_OUT_t in;
    in = ComputeFFT();
	
		in.ClapEnergy = in.ClapEnergy*100; // Scaling!

		LCD_Puts("Max Amp: ", 1, 1, WHITE, BLACK,1,1);
		LCD_Puts("Clp det: ", 1, 10, WHITE, BLACK,1,1);
		LCD_Puts("T: ", 1, 20, WHITE, BLACK,1,1);

    char str[16];
    char str2[8];
    char str3[8];

    sprintf(str, "%.2f", in.ClapEnergy);
    sprintf(str2, "%d", getClaps());
    sprintf(str3, "%d", getSecondCount());
		
		LCD_Puts(str, 60, 1, WHITE, BLACK,1,1);
		LCD_Puts(str2, 60, 10, WHITE, BLACK,1,1);
		LCD_Puts(str3, 60, 20, WHITE, BLACK,1,1);

		Delayms(DELAY_VALUE); // This delay is essential for correct timing. Default value = 10


   // float32_t freq;

   // freq = in.maxIndex * (45000 / 256);  // Obtaining frequency from the signal

		//if(freq > 500 && freq < 1200) {

			if (getClaps() == 0) {
					if (in.ClapEnergy > CLAP_AMPLITUDE) {
							TIM_Cmd(TIM2, ENABLE);
					}
			}
		
			if (in.ClapEnergy > CLAP_AMPLITUDE) {
				setClaps(getClaps() + 1);
				BlinkOnboardLED(3);
			}

			if(getSecondCount() > 60) {
				setSecondsCount(0);
				setClaps(0);
			}
			
			if (getSecondCount() > 5 && getClaps() > 10) {
					TIM_Cmd(TIM2, DISABLE);
					setSecondsCount(0);
					setClaps(0);
					set_task_counter(get_task_counter() + 1);
					set_game_result(COMPLETED); 
					Delayms(100);
			}
		//}
}


void SilenceDetectionByEnergy(void) {
	
		raw_adc = TM_ADC_Read(ADC1, ADC_Channel_3);
	
    FFT_OUT_t in;
    in = ComputeFFT();
	
    //freq = in.maxIndex * (45000 / 256); 
	
		in.VoiceEnergy = in.VoiceEnergy*100; // Scaling!
	
		//if(freq > 100 && freq < 6000) inMaxValueInRange = in.maxValue;  // Approximately range of human voice
		// TODO: actually look in that freq. range, for now omitted
		Delayms(DELAY_VALUE); // This delay is essential for correct timing. Default value = 10
		// ********** Setting the Threshold 
		if(N < SIL_AVG_SAMPLES && silence_thresh_is_set == 0){
			silence_thresh_avg += in.VoiceEnergy;
			LCD_Puts("                ", 1, 1, WHITE, BLACK,1,1);
			LCD_Puts("Acq. Thr...        ", 1, 1, WHITE, BLACK,1,1);
			Delayms(50);
		}
		
		if(N >= SIL_AVG_SAMPLES && silence_thresh_is_set == false){
			setSilenceThresh( (silence_thresh_avg/SIL_AVG_SAMPLES) + correction_value ); 			// Add correction value, 4 seems to be optimal
			//if(getSilenceThresh() > SIL_THR_MAX) setSilenceThresh(SIL_THR_MAX);
			silence_thresh_avg = 0; // Clearing average variable after threshold is set
			silence_thresh_is_set = true;
			silence_value_biggest_old = 0;


		//	TIM_Cmd(TIM5, ENABLE);
		} 
		if(N < SIL_AVG_SAMPLES + 1 && silence_thresh_is_set == false) N++;
/*
		
		if(getTIM5_count() > 200 && silence_thresh_is_set == true){
			LCD_Puts("Thr reset!", 1, 1, RED, BLACK,1,1);
			resetSilenceThresh(); // After 2 periouds of tim5 (37.5*2s) redefine silence threshold
			silence_thresh_is_set = false;
			setTIM5_count(0);
			silence_thresh_incr+=0.125;
			TIM_Cmd(TIM5, DISABLE);
		}
		
		if(silence_thresh_incr>silence_thresh_incr_old){
			correction_value+=1;
			silence_thresh_incr_old=silence_thresh_incr;
			LCD_Puts("Thr. set: ", 1, 1, WHITE, BLACK,1,1);
			sprintf(silence_thresh_str, "%3.0f", getSilenceThresh());
			LCD_Puts(silence_thresh_str, 70, 1, WHITE, BLACK,1,1);
			//Maybe set a limit to increasing correction value..
			//if(silence_thresh_incr == 32) correction_value = CORRECTION_VALUE;
		}
		*/

			// ******* Setting Threshold END	

			
		if (in.VoiceEnergy <= getSilenceThresh() && silence_thresh_is_set == true){
			//	TIM_Cmd(TIM4, DISABLE); // Disabling timer that counts Loudness time
				TIM_Cmd(TIM2, ENABLE);  // Enabling timer that counts quitetness time
				setTIM4_count0(0); // Resetting timer that counts Loundess time
			
				GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
				GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			

		}
		
		if (in.VoiceEnergy > getSilenceThresh() && silence_thresh_is_set == true) {
			setSecondsCount(0);
	//		TIM_Cmd(TIM4, ENABLE);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			
			//if(getTIM4_count0() >= 10) setSecondsCount(0);
			//ControlBiColorLED(BC_LED_GREEN, true);
			//ControlBiColorLED(BC_LED_RED, false);
			//	GPIO_SetBits(LED_GPIO, BC_LED_GREEN);
			//	GPIO_ResetBits(LED_GPIO, BC_LED_RED);
		}
		
		//if(getTIM4_count0() >= 10) setSecondsCount(0);
		
		if (getSecondCount() > SILENCE_TIME) { //Silence time
				__enable_irq();
				TIM_Cmd(TIM2, DISABLE);
				LCD_Puts("Done", 1, 50, WHITE, BLACK,1,1);
				set_task_counter(get_task_counter() + 1);
				setSecondsCount(0);
				setSilenceThresh(SILENCE_AMPLITUDE);
				Delayms(100);
		}
		
		if(silence_thresh_is_set == true){
			LCD_Puts("Thr. set: ", 1, 1, WHITE, BLACK,1,1);
			sprintf(silence_thresh_str, "%.2f", getSilenceThresh());
			LCD_Puts(silence_thresh_str, 70, 1, WHITE, BLACK,1,1);
		}
		LCD_Puts("C.V.: ", 1, 10, WHITE, BLACK,1,1);
		sprintf(silence_value_str, "%.2f", in.VoiceEnergy);
		LCD_Puts(silence_value_str, 70, 10, RED, BLACK,1,1);
		
		LCD_Puts("T->Done: ", 1, 30, WHITE, BLACK,1,1);
		sprintf(silence_time_str, "%4d", getSecondCount());
		LCD_Puts(silence_time_str, 70, 30, WHITE, BLACK,1,1);
	/*	
		LCD_Puts("T->res Th: ", 1, 40, WHITE, BLACK,1,1);
		sprintf(silence_thresh_time_str, "%4d", getTIM5_count()/10);
		LCD_Puts(silence_thresh_time_str, 70, 40, WHITE, BLACK,1,1);
		*/
		silence_value_biggest = in.VoiceEnergy;
		if(silence_value_biggest > silence_value_biggest_old){
			silence_value_biggest_old = silence_value_biggest;
		}
		LCD_Puts("Max Val:", 1, 20, WHITE, BLACK,1,1);
		sprintf(silence_value_biggest_str, "%.2f", silence_value_biggest_old);
		LCD_Puts(silence_value_biggest_str, 70, 20, WHITE, BLACK,1,1);
		
		/*Sil_SignalToDraw = ( in.Energy );
		LCD_DrawLine(getTIM5_count3()+2,( 80-Sil_SignalToDraw *100),getTIM5_count3()+2,( 80-Sil_PrevSignalToDraw*100),RED);
		LCD_DrawLine(getTIM5_count3()+1,0,getTIM5_count3()+1,80,BLACK);
		Sil_PrevSignalToDraw = Sil_SignalToDraw;
		if (getTIM5_count3() >= 101) setTIM5_count3(1);	
		
		LCD_Puts("E: ", 1, 60, WHITE, BLACK,1,1);
		sprintf(silence_energy_str, "%4.0f", in.Energy);
		LCD_Puts(silence_energy_str, 15, 60, RED, BLACK,1,1);
		*/
		
		LCD_Puts("RAW AD: ", 1, 40, WHITE, BLACK,1,1);
		sprintf(raw_adc_str, "%d", raw_adc);
		LCD_Puts(raw_adc_str, 50, 40, WHITE, BLACK,1,1);
		
		
		
}



/* Draw bar for LCD */
/* Simple library to draw bars */
void DrawBar(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t foreground,
        uint16_t background) {
    uint16_t height;
    height = (uint16_t)((float32_t) value / (float32_t) maxValue * (float32_t) maxHeight);
    if (height == maxHeight) {
        TM_ILI9341_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
    } else if (height < maxHeight) {
        TM_ILI9341_DrawLine(bottomX, bottomY, bottomX, bottomY - height, foreground);
        TM_ILI9341_DrawLine(bottomX, bottomY - height, bottomX, bottomY - maxHeight, background);
    }
}
				


void setClaps(uint8_t c) {
	claps = c;
}

uint8_t getClaps(void) {
	return claps;
}

void setSilenceThresh(float32_t st) {
	silence_thresh_is_set = true;
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
