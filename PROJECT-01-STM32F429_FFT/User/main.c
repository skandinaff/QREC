/**
 *
 *
 *	@author		Aleksandrs S.
 *	@email		sevalks@gmail.com
 *	@website	http://stm32f4-discovery.com
 *	@ide			Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 *	Notes: 
 *		- Under "Options for target" > "C/C++" > "Define" you must add 2 defines (I've already add them):
 *			- ARM_MATH_CM4
 *			- __FPU_PRESENT=1
 */

/*  HARDWARE SET-UP

		Timers clock is 45 Mhz
		System Clock is 180 Mhz

		Pins used in discovery kit:

		PA3 - Microphone
		PF8 - Pulse Sensor
		
		PA10 - USART
		PA9 - USART
		
		Pins used in production board:
		
		PA3 - Microphone
		PB0 - Pulse Sensor
		PA2 - Pulse Capacitive Detector
		
		PD0..PD3 - Extra LEDs
		PD8..PD12 - Reeds
		PE8..PE12 - Big 12V LEDs
		PE0..PE3 - PIRs
		PE4 - Status LED
		PE5 - 7 Seg. Disp. LATCH
		PE6 - 7 Seg. Disp. CLK
		PE7 - 7 Seg. Disp. DATA OUT
		
		PC10 USART TX
		PC11 USART RX
		
		PC8 RS485 EN 

Task's order:

		Clap
		Silence
		Motion
		Whistle
		Pulse

*/


/* Include core modules */
#include "stm32f4xx.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stdlib.h"

/* My custom includes */
#include "usart.h"
#include "init_periph_irqs.h"
#include "detect_whistle.h"
#include "init_periph_irqs.h"
#include "cup_detection.h"
#include "pulse_reading.h"
#include "movement_detection.h"
#include "leds.h"
#include "quest.h"
#include "LPH8731-3C.h"

int main(void) {
	SystemInit(); /* Initialize system */
	
	TM_DELAY_Init(); /* Delay init */
	

	INTTIM2_Config(); // Responsible only for Seconds count
	INTTIM5_Config(); // General purpouse small increment timer
	INTTIM3_Config(); // Responsibleo only for 12V leds blinking
	//	INTTIM4_Config(); // TODO: decide if this timer is needed

	Configure_CupDetection();	
	Configure_Onboard_LEDS();	
	Configure_485();
	Configure_12V_LEDS();
	//Configure_LED_indicator(); // Can't use this when LCD or BiColorLED is used
	//Configure_BiColor_LED();
	//Configure_Pulse_CapSens();
	

		
	init_usart();
	
	GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_1); // LED indicating that board is ON
	

	LCD_ON();
	
	LCD_init();
 
	LCD_FillScreen(BLACK);

	Delayms(300);
	

	int len = DATA_PACKET_LEN + 1;
	unsigned char packet[len];
	incoming_packet_t incoming_packet;
	

	GPIO_ResetBits(RS485_GPIO, RS485_EN_PIN); //RX
	

	while (1) {	

		check_usart_while_playing();
		
		switch(get_game_state()){
			case IDLE:
				//Emergency_Stop(); 
				set_game_result(NOT_COMPLETED);
				LCD_Puts("State: Idle", 1, 30, WHITE, BLACK,1,1);
				LCD_Puts("Result: ", 1, 40, WHITE, BLACK,1,1);
				if(get_game_result()==COMPLETED)					 LCD_Puts("COMPL", 50, 40, WHITE, BLACK,1,1);
				if(get_game_result()==NOT_COMPLETED) 			 LCD_Puts("NOT_C", 50, 40, WHITE, BLACK,1,1);
				GPIO_ResetBits(LED_GPIO, STATE_LED);
				break;
			case GAME:
				LCD_Puts("State: Game", 1, 30, WHITE, BLACK,1,1);
				if(get_game_result()==COMPLETED) {
					LCD_FillScreen(BLACK);
					LCD_Puts("COMPL", 50, 40, WHITE, BLACK,1,1);
					GPIO_ResetBits(LED_GPIO, STATE_LED);
					GPIO_ResetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4 | LED_5);
					set_task_counter(FIRST_TASK);
					setSecondsCount(0);
					TIM_Cmd(TIM2, DISABLE);
					TIM_Cmd(TIM5, DISABLE);
					setTIM5_count(0);
					set_xLED(0);
					setClaps(0);
					set_break_flag(false);
					set_first_start(false);
				}
				if(get_game_result()==NOT_COMPLETED) {
					LCD_Puts("NOT_C", 50, 40, WHITE, BLACK,1,1);
					GPIO_SetBits(LED_GPIO, STATE_LED);
					Control_12V_LEDs();						
				}
				while(get_game_result()==NOT_COMPLETED && get_game_state()==GAME) {
					PerformQuest();
				}
				break;
		}
		
		

	//****************	

		
	}
}
