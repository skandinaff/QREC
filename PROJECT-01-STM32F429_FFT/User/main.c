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
	

	INTTIM2_Config();
	INTTIM5_Config();
	INTTIM3_Config();
	INTTIM4_Config();

	Configure_CupDetection();	
	Configure_Onboard_LEDS();	
	Configure_485();
	Configure_12V_LEDS();
	//Configure_LED_indicator(); // Can't use this when LCD or BiColor LED is used
	//Configure_BiColor_LED();
	Configure_Pulse_CapSens();
	

		
	init_usart();
	
	GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_1); // LED indicating that board is ON
	LCD_OFF();
	
	Delayms(300);
	

	int len = DATA_PACKET_LEN + 1;
	unsigned char packet[len];
	incoming_packet_t incoming_packet;
	



	while (1) {	


		
		if (usart_has_data()) {
			
			
			
			usart_get_data_packet(packet);
			incoming_packet = usart_packet_parser(packet);
			
			if (usart_validate_crc8(incoming_packet) && usart_packet_is_addressed_to_me(incoming_packet)){
				BlinkOnboardLED(2);
				switch (incoming_packet.instruction) {
					case INSTR_MASTER_TEST:
						SendInstruction(INSTR_SLAVE_READY);
						break;
					case INSTR_MASTER_WORK_START:
						while (get_task_counter() <= TASK_COUNT) {
							GPIO_SetBits(LED_GPIO, STATE_LED);
							PerformQuest();
							if(get_break_flag()){
								GPIO_ResetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4 | LED_5);
								set_task_counter(FIRST_TASK);
								setSecondsCount(0);
								TIM_Cmd(TIM2, DISABLE);

								set_break_flag(false);
								set_first_start(false);
								break;
							}
						}

						break;
					case INSTR_MASTER_STATUS_REQ:	
						
						break;
					case INSTR_MASTER_SET_IDLE:
						set_task_counter(FIRST_TASK);
						break;
					case CINSTR_GOTO_END:
						set_cups_override();
						set_task_counter(TASK_COUNT + 1);
						if(get_task_counter() == TASK_COUNT ) set_task_counter(TASK_COUNT); // Maybe I should go back to task 1, but, meh..
						PerformQuest();
						break;
					case TEST_DISP:
						Test_7Seg();
						break;
					case SYS_RESET:
						NVIC_SystemReset();
						break;
					case PULSE:
						set_cups_override();
						set_task_counter(4);
						PerformQuest();
						break;
					case WS_TEST_MODE:
						set_cups_override();
						while (get_task_counter() <= TASK_COUNT) {
							GPIO_SetBits(LED_GPIO, STATE_LED);
							Control_12V_LEDs();
							PerformQuest();
							if(get_break_flag()){
								GPIO_ResetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4 | LED_5);
								set_task_counter(FIRST_TASK);
								setSecondsCount(0);
								TIM_Cmd(TIM2, DISABLE);
								set_break_flag(false);
								}
						}
						break;
					break;
				}				
			}
		}
		
		set_task_counter(FIRST_TASK);
		
	}
}
