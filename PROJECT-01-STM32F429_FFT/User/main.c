/**
 *
 *
 *	@author		Aleksandrs S.
 *	@email		sevalks@gmail.com
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
 
// Timers clock is 45 Mhz
// System Clock is 180 Mhz
/*
PA3 - Microphone
PF8 - Pulse Sensor
PA10 - USART
PA9 - USART

*/


/* Include core modules */
#include "stm32f4xx.h"
#include "defines.h"

#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_adc.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_sdram.h"
#include "tm_stm32f4_dac_signal.h"

#include "tm_stm32f4_button.h"
#include "tm_stm32f4_usart.h"

#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"


#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Include arm_math.h mathematic functions */
#include "arm_math.h"

/* Includes for timer to work */
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/* My custom includes */
#include "usart.h"
#include "stack.h"






/* FFT settings */
#define SAMPLES					    512 			/* 256 real party and 256 imaginary parts */
#define FFT_SIZE				    SAMPLES / 2		/* FFT size is always the same size as we have samples, so 256 in our case */

#define FFT_BAR_MAX_HEIGHT		    120 		    /* 120 px on the LCD */
#define TARGET_BPM 80



GPIO_InitTypeDef GPIO_InintStructure;


TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure_TIM5;

TM_BUTTON_t* MyButton;



// Function prototypes

void INTTIM2_Config(void);
void INTTIM5_Config(void);
void readPulse(void);
void BUTTON1_EventHandler(TM_BUTTON_PressType_t type);
void BUTTON2_EventHandler(TM_BUTTON_PressType_t type);
uint8_t DetectCups(void);

/* Global variables */
float32_t Input[SAMPLES];
float32_t Output[FFT_SIZE];

uint16_t secondsCount = 0;
uint16_t tim5_count = 0;
uint16_t tim5_count2 = 0;
uint16_t counter = 0;
uint16_t counter2 = 0;



uint16_t time = 1;

uint32_t pulse = 0;

uint16_t ButtonState = 0;

unsigned char c1_state = 0;
unsigned char c2_state = 0;
unsigned char c3_state = 0;
unsigned char c4_state = 0;
unsigned char c5_state = 0;

uint16_t cstate;
char cstate_str[32];


uint16_t old_tim5_count = 0;

// Flags that signal that the task is done

struct Flags {

bool detect_whistle;
bool read_pulse;
bool detect_movement;
bool detect_silence;
bool detect_clap;
bool all_tasks;

};

struct Flags t_flags;

bool all_cups_present;
	
//Pulse Reader variables

volatile uint16_t BPM;
volatile uint16_t Signal;
volatile uint16_t IBI = 600;
volatile uint16_t Pulse = 0;
volatile uint16_t QS = 0;
	
	
volatile uint16_t rate[10];                    // array to hold last ten IBI values
volatile uint32_t sampleCounter = 0;          // used to determine pulse timing
volatile uint32_t lastBeatTime = 0;           // used to find IBI
volatile uint16_t P = 2048; //512;                     // used to find peak in pulse wave, seeded
volatile uint16_t T = 2048; //512;                     // used to find trough in pulse wave, seeded
volatile uint16_t thresh = 2048; //525;                // used to find instant moment of heart beat, seeded
volatile uint16_t amp = 100; //100;                   // used to hold amplitude of pulse waveform, seeded
volatile uint16_t firstBeat = 1;        // used to seed rate array so we startup with reasonable BPM
volatile uint16_t secondBeat = 1;      // used to seed rate array so we startup with reasonable BPM

char adc_result_str[15];
char BPM_result_str[15];
char thresh_restult_str[15];

// this is the stack that holds activated cups
stackT c_stack; // Creating stack of cups 
char c_str[5] = {0,0,0,0,0};  // String of "buttons" pressed
char *traverse;  // Pointer used to traverse the string




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

void FlashLeds(float32_t freq) {


	if (freq >= 1100) {
		TM_DISCO_LedOff(LED_RED);
		TM_DISCO_LedOn(LED_GREEN);


		TIM_Cmd(TIM2, ENABLE);


	}

	if (freq < 900 /*&& freq > 10*/) {
		TM_DISCO_LedOn(LED_RED);
		TM_DISCO_LedOff(LED_GREEN);
		TM_ILI9341_Puts(10, 25, "                       ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		TIM_Cmd(TIM2, DISABLE);
	}


}

void countSeconds() {
	if (secondsCount > 5) {
		TM_ILI9341_Puts(10, 25, "You Whistled for 5 sec!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		Delayms(750);
		t_flags.detect_whistle = true;
		secondsCount = 0;
	}
}


void INTTIM2_Config(void){

	
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

void TIM2_IRQHandler(void) {

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		secondsCount += 1;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		//TM_ILI9341_Puts(10, 25, "You Whistled for 10 sec!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	}
}

void INTTIM5_Config(void){

	
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

void TIM5_IRQHandler(void) {


	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

		sampleCounter += 1;


		counter += 1;          // This horrible thing here helps to get ~0.250ms refresh rate for drawing signal
		counter2 += 1;
		if (counter == 50) {
			tim5_count += 1;
			counter = 0;
		}
		if (counter2 == 150) {
			tim5_count2 += 1;
			counter2 = 0;
		}

	}
}

void readPulse2(void){

	

	TM_ILI9341_DrawPixel(tim5_count, 240-thresh/17, ILI9341_COLOR_RED);		


	

  Signal = TM_ADC_Read(ADC3, ADC_Channel_6);              // read the Pulse Sensor 
  //sampleCounter += 2; // 2 (ms)                         // keep track of the time in mS with this variable
																													// We've assigned this variable incrementation to a timer iinterrupts
  uint16_t N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

    //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T){                        // T is the trough
      T = Signal;                         // keep track of lowest point in pulse wave 
    }
  }

  if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
    P = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250){                                   // avoid high frequency noise
		
		//Here I should add a condition that will check signal's amplitude, to avoiud measuremets that are just above 0
		
    if ( (Signal > thresh) && (Pulse == 0) && (N > (IBI/5)*3) && ((Signal - thresh) > 100) && (Signal - thresh) < 400 ){        
      Pulse = 1;                               // set the Pulse flag when we think there is a pulse
      TM_DISCO_LedOn(LED_RED);               // turn on pin 13 LED
			//TM_DISCO_LedOn(LED_GREEN);
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if(secondBeat == 1){                        // if this is the second beat, if secondBeat == TRUE
        secondBeat = 0;                  // clear secondBeat flag
        for(uint16_t i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;                      
        }
      }

      if(firstBeat == 1){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = 0;                   // clear firstBeat flag
        secondBeat = 1;                   // set the second beat flag

        return;                              // IBI value is unreliable so discard it
      }   


																								// keep a running total of the last 10 IBI values
      uint32_t runningTotal = 0;                  // clear the runningTotal variable    

      for(uint16_t i=0; i<=8; i++){                // shift data in the rate array
        rate[i] = rate[i+1];                  // and drop the oldest IBI value 
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      QS = 1;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }

  if (Signal < thresh && Pulse == 1){   // when the values are going down, the beat is over
    TM_DISCO_LedOff(LED_RED);              // turn off pin 13 LED
    //TM_DISCO_LedOff(LED_GREEN);
		Pulse = 0;                         // reset the Pulse flag so we can do it again

    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 1000){                           // if N milliseconds go by without a beat
    thresh = 2058;                          // set thresh default
    P = 2048;                               // set P default
    T = 2048;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
    firstBeat = 1;                      // set these to avoid noise
    secondBeat = 0;                    // when we get the heartbeat back
		
		//BPM = 0;		// Add this line here, so when no beat detected display shows 0
  }
	

	

///////////
	

	sprintf(adc_result_str, "%4d: ", Signal);
	TM_ILI9341_Puts(1, 5, "RAW ADC: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	if(tim5_count2 != old_tim5_count) TM_ILI9341_Puts(125, 5, adc_result_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	
	sprintf(thresh_restult_str, "%4d: ",thresh);
	TM_ILI9341_Puts(1, 25, "Thresh: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
  TM_ILI9341_Puts(125, 25, thresh_restult_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	
	sprintf(BPM_result_str, "%4d: ", BPM);
	TM_ILI9341_Puts(1, 45, "BPM: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	if(Pulse == 1){
		TM_ILI9341_Puts(125, 45, BPM_result_str, &TM_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE);
		//TM_USART_Putc(USART1, BPM);
	}
	

	
	old_tim5_count = tim5_count2;

	if(tim5_count >= 320){
		tim5_count = 1;
		TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
	
	}
	


		TM_ILI9341_DrawPixel(tim5_count, 240-Signal/17, 0x1234);		
	
	 if(BPM > TARGET_BPM) {
		TIM_Cmd(TIM2, ENABLE);
		if(secondsCount >= 5){
			TM_ILI9341_Puts(1, 65, "You have >120 BPM for >5 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			Delayms(2000);
			t_flags.read_pulse = true;
			
		}
	 }else{
		TIM_Cmd(TIM2, DISABLE);
		 TM_ILI9341_Puts(1, 65, "                              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		 secondsCount=0;
	 }
	 
	if(DetectCups()<5){
		all_cups_present = false;
		cstate = 0;
	}

	
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
		//countSeconds();

		if (secondsCount > 5) {
			TM_ILI9341_Puts(10, 25, "You Whistled for 5 sec!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			Delayms(2000);
			t_flags.detect_whistle = true;
			secondsCount = 0;
		}
	
		if(DetectCups()<5){
			all_cups_present = false;
			cstate = 0;
		}
		
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


void peform_instruction(incoming_packet_t incoming_packet) {
    switch (incoming_packet.instruction) {
        case INSTR_MASTER_TEST:
            // TODO: gotov ili ne gotov k rabote
            put_str("A");
            break;
        case INSTR_MASTER_WORK_START:
            // TODO: nachinaem quest
            put_str("B");
            break;
        case INSTR_MASTER_STATUS_REQ:
            put_str("C");
            // TODO: zapros viponen kvest ili net
            break;
        case INSTR_MASTER_SET_IDLE:
            put_str("D");
            // TODO: vernutj v ishodnoe sostojanie
            break;
    }
}



void Configure_PD(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
}

void Configure_MotionSensorPort(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOG, &GPIO_InitStruct);
	
}

uint8_t DetectCups(void){
	
	c1_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7);
	c2_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5);
	c3_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);
	c4_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4);
	c5_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
	
	return c1_state + c2_state + c3_state + c4_state + c5_state;
}

void MotionDetection(void){
	
	bool motion = GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1);
	char mvm[1];
	char scd[8];
	sprintf(mvm, "%d", motion);
	TM_ILI9341_Puts(20, 20, mvm, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);		
	
	//TODO: Abstract this into separate functuon: cupsAllThere
	if(DetectCups()<5){
		all_cups_present = false;
		cstate = 0;
	}
	
		if(motion) {
		TIM_Cmd(TIM2, ENABLE);
		
		sprintf(scd, "%d", secondsCount);
		TM_ILI9341_Puts(20, 45, scd, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
			
		if(secondsCount >= 10){
			TM_ILI9341_Puts(1, 65, "You haven't moved for 10 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			Delayms(2000);
			t_flags.detect_movement = true;
			
		}
	 }else{
		TIM_Cmd(TIM2, DISABLE);
		 TM_ILI9341_Puts(1, 65, "                              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		 secondsCount=0;
	 }

	
}

uint8_t SendComplPacket(void){
	unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	outgoing_packet_t outgoing_packet = usart_assemble_response(INSTR_SLAVE_COMPLETED);
	usart_convert_outgoing_packet(packet, outgoing_packet);
	put_str(packet);
	Delayms(100);
	return 1;
}

void reset_all_t_flags(void){
	t_flags.detect_whistle = false;
	t_flags.detect_clap = false;
	t_flags.detect_movement = false;
	t_flags.read_pulse = false;
	t_flags.detect_silence = false;
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
		

	
	INTTIM2_Config();
	INTTIM5_Config();

	
	/* Initialize LCD */
	TM_ILI9341_Init();
	TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);


	/* Initialize ADC, PA0 is used */
	TM_ADC_Init(ADC1, ADC_Channel_3); // PA3 Microphone's ADC 
	
	TM_ADC_Init(ADC3, ADC_Channel_6); // PF8 PulseSensor's ADC
	

	init_usart();
	
	Configure_PD();
	Configure_MotionSensorPort();
	
	Delayms(300);
	

	
	unsigned char* packet = malloc((DATA_PACKET_LEN + 1) * sizeof(unsigned char));
	incoming_packet_t incoming_packet;
	
	
	reset_all_t_flags();
	t_flags.all_tasks = false;



	while (1) {	


		
		switch (cstate) {
			case 0: // Wait for cups to be placed
				
				
				TM_ILI9341_Puts(1, 100, "Hello! Please put all 5 cups!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				
				
				if(!t_flags.all_tasks){
					if(DetectCups() == 5){
						//cstate +=1;
						cstate = 3;
						all_cups_present = true;
					}
				}
				
				
			  
				
				break;
			case 1:  // Whistle Detection 
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				while(!t_flags.detect_whistle && all_cups_present) DetectWhistle(S, maxValue, maxIndex, i);
			
				if(!all_cups_present) cstate = 0;
				else if(all_cups_present) cstate+=1;
			
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
			case 2:  // Pulse Readings
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				tim5_count=1;
				TM_DISCO_LedOff(LED_RED);
				TM_DISCO_LedOff(LED_GREEN);
			
				while(!t_flags.read_pulse && all_cups_present) readPulse2();
			
				if(!all_cups_present) cstate = 0;
				else if(all_cups_present) cstate+=1;
				
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
			case 3:  // Motion detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				while(!t_flags.detect_movement && all_cups_present) MotionDetection();
			
				if(!all_cups_present) cstate = 0;
				else if(all_cups_present) cstate+=1;
			
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);	
			  break;
			case 4:	// Silence detection
				//TODO: implemet silence detection.
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			  TM_ILI9341_Puts(1, 100, "Silence detection coming soon!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			  Delayms(1500);
				cstate+=1;				
				break;
			case 5: // Clap detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			  TM_ILI9341_Puts(1, 100, "Clap detection coming soon!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			  Delayms(1500);
				cstate+=1;				
				break;
			case 6: // All Done, you Win
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			  TM_ILI9341_Puts(1, 100, "All done, you win!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			  Delayms(2000);
				do SendComplPacket();
				while(!SendComplPacket());
				reset_all_t_flags();
				t_flags.all_tasks = true;
				cstate=0;
				break;
			default:
				cstate = 0;
				break;
		}

		
		if (usart_has_data()) {
			usart_get_data_packet(packet);
            incoming_packet = usart_packet_parser(packet);
            if (usart_packet_is_addressed_to_me(incoming_packet)) {
                peform_instruction(incoming_packet);

                // TODO: THIS BIT SENDS THE OUTGOING PACKET
                unsigned char instruction = INSTR_SLAVE_NOT_READY;
                instruction = INSTR_SLAVE_READY;
                instruction = INSTR_SLAVE_NOT_COMLETED;
                instruction = INSTR_SLAVE_READY;

                unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
                outgoing_packet_t outgoing_packet = usart_assemble_response(instruction);
                usart_convert_outgoing_packet(packet, outgoing_packet);
                put_str("www");
                put_str(packet);
            }
		}

		
		
		
		char state[1];
		
		sprintf(state, "%d", cstate);
		TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	

		
		/*** This is for pulse readings ***/
		if(QS == 1) QS = !QS; 						// A Heartbeat Was Found, reset the Quantified Self flag for next time    
		/**********************************/	
		

		
	}
	
	// free(packet);
}
