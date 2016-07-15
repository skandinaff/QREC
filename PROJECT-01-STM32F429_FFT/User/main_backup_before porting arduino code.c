/**
 *	Keil project example for FFT on STM32F4 device.
 *
 *	Works on STM32F429-Discovery board because it has LCD
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 *	Notes: 
 *		- Under "Options for target" > "C/C++" > "Define" you must add 2 defines (I've already add them):
 *			- ARM_MATH_CM4
 *			- __FPU_PRESENT=1
 */
/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_sdram.h"
#include "tm_stm32f4_dac_signal.h"

#include "tm_stm32f4_button.h"




#include "stm32f4xx_rcc.h"

#include <stdio.h>

/* Include arm_math.h mathematic functions */
#include "arm_math.h"

/* Includes for timer to work */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"



/* FFT settings */
#define SAMPLES					512 			/* 256 real party and 256 imaginary parts */
#define FFT_SIZE				SAMPLES / 2		/* FFT size is always the same size as we have samples, so 256 in our case */

#define FFT_BAR_MAX_HEIGHT		120 			/* 120 px on the LCD */

GPIO_InitTypeDef GPIO_InintStructure;


TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure_TIM5;

TM_BUTTON_t* MyButton;

uint16_t secondsCount = 0;
uint16_t tim5_count = 0;
	uint16_t counter = 0;

uint16_t time = 1;

char adc_result_str[15];

void INTTIM2_Config(void);
void INTTIM5_Config(void);
void readPulse(void);

void BUTTON1_EventHandler(TM_BUTTON_PressType_t type);

/* Global variables */
float32_t Input[SAMPLES];
float32_t Output[FFT_SIZE];

uint32_t pulse = 0;

uint16_t ButtonState = 0;

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



void FlashLeds (float32_t freq){
	

	
		if(freq >= 1100){
			TM_DISCO_LedOff(LED_RED);
			TM_DISCO_LedOn(LED_GREEN);
			
			
		TIM_Cmd(TIM2, ENABLE);
			
			

		}
		
		if(freq < 900 /*&& freq > 10*/){
			TM_DISCO_LedOn(LED_RED);
			TM_DISCO_LedOff(LED_GREEN);
			TM_ILI9341_Puts(10, 25, "                       ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			TIM_Cmd(TIM2, DISABLE);
		}
		

}

void countSeconds(){
	if(secondsCount > 5) {
		TM_ILI9341_Puts(10, 25, "You Whistled for 5 sec!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		secondsCount = 0;
	}
}

void TIM2_IRQHandler(void)
{
	
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
		secondsCount+=1;
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    GPIO_ToggleBits(GPIOC, GPIO_Pin_9);
		//TM_ILI9341_Puts(10, 25, "You Whistled for 10 sec!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
  }
}

void InitGPIO(void){
	/*Initializing Pins*/
	GPIO_InitTypeDef PORT;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	PORT.GPIO_Pin = (GPIO_Pin_9 | GPIO_Pin_8);
	PORT.GPIO_Mode = GPIO_Mode_OUT;
	PORT.GPIO_Speed = GPIO_Low_Speed;
	
	GPIO_Init(GPIOC, &PORT);
	/********************/
}

void INTTIM2_Config(void)
{

	
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100000 - 1;  // 1 MHz down to 1 KHz (1 ms)
  TIM_TimeBaseStructure.TIM_Prescaler = 450 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  /* TIM2 enable counter */
  //TIM_Cmd(TIM2, ENABLE);
}


void TIM5_IRQHandler(void)
{

		
	
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		
		counter+=1;
		if(counter == 50 ){
			tim5_count+=1;
			counter =0;
		}
		//readPulse();
			//pulse = TM_ADC_Read(ADC2, ADC_Channel_1);
			//sprintf(adc_result_str, "%4d: ", pulse);
			//TM_ILI9341_Puts(10, 45, adc_result_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		//
    //GPIO_ToggleBits(GPIOC, GPIO_Pin_8);
  }
}

void INTTIM5_Config(void)
{

	
  NVIC_InitTypeDef NVIC_InitStructure_TIM5;
  /* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure_TIM5.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure_TIM5);

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure_TIM5.TIM_Period = 100 - 1;  // 1 MHz down to 1 KHz (1 ms)
  TIM_TimeBaseStructure_TIM5.TIM_Prescaler = 900 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
  TIM_TimeBaseStructure_TIM5.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure_TIM5.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure_TIM5);
  /* TIM IT enable */
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM5, ENABLE);
}

void readPulse(void){
	
	pulse = TM_ADC_Read(ADC3, ADC_Channel_6);
	sprintf(adc_result_str, "%4d: ", pulse);
	TM_ILI9341_Puts(10, 45, adc_result_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	

	if(tim5_count >= 320){
		tim5_count = 1;
		TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
	}
	

		TM_ILI9341_DrawPixel(tim5_count, 240-pulse/17, 0x1234);

	
//			/* Display data on LCD */
//		for (uint16_t i = 0; i < 250; i++) {
//			/* Draw FFT results */
//			DrawBar(30 + 2 * i,
//					220,
//					FFT_BAR_MAX_HEIGHT,
//					pulse,
//					pulse,
//					0x1234,
//					0xFFFF
//			);
//	
//		}
	
		
}

void DetectWhistle(arm_cfft_radix4_instance_f32 S, float32_t maxValue, uint32_t maxIndex, uint16_t i){
	
	
		TM_ILI9341_Puts(150, 10,"           ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	//arm_cfft_radix4_instance_f32 S;	/* ARM CFFT module */
	//float32_t maxValue;							/* Max FFT value is stored here */
	//uint32_t maxIndex;							/* Index in Output array where max value is */
	//uint16_t i;
		
		/* This part should be done with DMA and timer for ADC treshold */
		/* Actually, best solution is double buffered DMA with timer for ADC treshold */
		/* But this is only for show principle on how FFT works */
		for (i = 0; i < SAMPLES; i += 2) {
			/* Each 22us ~ 45kHz sample rate */
			Delay(21);
			
			/* We assume that sampling and other stuff will take about 1us */
			
			/* Real part, must be between -1 and 1 */
			Input[(uint16_t)i] = (float32_t)((float32_t)TM_ADC_Read(ADC1, ADC_Channel_3) - (float32_t)2048.0) / (float32_t)2048.0;
			/* Imaginary part */
			Input[(uint16_t)(i + 1)] = 0;
		}
		
		/* Initialize the CFFT/CIFFT module, intFlag = 0, doBitReverse = 1 */
		arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1);
		
		/* Process the data through the CFFT/CIFFT module */
		arm_cfft_radix4_f32(&S, Input);
		
		/* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
		arm_cmplx_mag_f32(Input, Output, FFT_SIZE);
		
		/* Calculates maxValue and returns corresponding value */
		arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);
	
		/* This is me trying to output frequency as a number */
		TM_ILI9341_Puts(10, 10, "Peak Freq:",&TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			
		
		char str[64];
		float32_t freq;
			
		freq = maxIndex * (45000 / 256);  // If a condition is added here, that cut's off low frequencies, just reaching 
																											// certain frequency is enough to trigger the event
			
		sprintf(str, "%.0f Hz", freq);
		
		if(maxIndex > 0) TM_ILI9341_Puts(150, 10, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				
		FlashLeds( freq );
		countSeconds();
	
		/* Display data on LCD */
		for (i = 0; i < FFT_SIZE / 2; i++) {
			/* Draw FFT results */
			DrawBar(30 + 2 * i,
					220,
					FFT_BAR_MAX_HEIGHT,
					(uint16_t)maxValue,
					(float32_t)Output[(uint16_t)i],
					0x1234,
					0xFFFF
			);
		}

}


void BUTTON1_EventHandler(TM_BUTTON_PressType_t type) {
	/* Check button */
	if (type == TM_BUTTON_PressType_OnPressed) {
		
			ButtonState = !ButtonState;
		  TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			tim5_count=1;
		
	} else if (type == TM_BUTTON_PressType_Normal) {

			//ButtonState = !ButtonState;
		
		
	} else {

			//ButtonState = !ButtonState;
		
		
	}
}

int main(void) {
	
	arm_cfft_radix4_instance_f32 S;	/* ARM CFFT module */
	float32_t maxValue;							/* Max FFT value is stored here */
	uint32_t maxIndex;							/* Index in Output array where max value is */
	uint16_t i;
	
	/* Initialize system */
	SystemInit();
	
	/* Delay init */
	TM_DELAY_Init();
	
	/* Initialize LED's on board */
	TM_DISCO_LedInit();
		
	TM_BUTTON_Init(GPIOA, GPIO_PIN_0, 1, BUTTON1_EventHandler);

	InitGPIO();
	INTTIM2_Config();
	INTTIM5_Config();
	
	
	/* Initialize LCD */
	TM_ILI9341_Init();
	TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
	


	/* Initialize ADC, PA0 is used */
	TM_ADC_Init(ADC1, ADC_Channel_3);
	
	TM_ADC_Init(ADC3, ADC_Channel_6);
	
	/* Print on LCD */
	//TM_ILI9341_Puts(10, 10, "Some random text here", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			
		
	while (1) {
	
	TM_BUTTON_Update();
	
	if(ButtonState == 0) {

		DetectWhistle(S, maxValue, maxIndex, i);
	}
	if(ButtonState == 1) {

		readPulse();
	}
	char state[16];
		
		
  sprintf(state, "%d", ButtonState);
	TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	

	}

}
