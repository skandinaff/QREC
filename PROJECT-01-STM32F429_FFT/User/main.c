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
		PC1 - Pulse Sensor
		
		PD0..PD3 - Extra LEDs
		PD8..PD12 - Reeds
		PE8..PE12 - Big 12V LEDs
		PE0..PE7 - PIRs
		
		PC10 USART TX
		PC11 USART RX

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

// Function prototypes
void PerformQuest(void);
uint8_t SendInstruction(unsigned char instruction);
void idle(void);
void check_usart_while_playing(void);

bool break_flag = false;


uint8_t SendInstruction(unsigned char instruction){
	GPIO_ToggleBits(RS485_GPIO, RS485_EN_PIN);
	unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	outgoing_packet_t outgoing_packet = usart_assemble_response(instruction);
	usart_convert_outgoing_packet(packet, outgoing_packet);
	put_str(packet);
	Delayms(100);
	free(packet);
	GPIO_ToggleBits(RS485_GPIO, RS485_EN_PIN);
	return 1;
}


void PerformQuest(void){
	int task_counter = get_task_counter();

	//TM_ILI9341_Fill(ILI9341_COLOR_WHITE);

	//char state[1];
	//sprintf(state, "%d", task_counter);
	//TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	if (!getAll_cups_present()) {
    setSecondsCount(0);	
		check_usart_while_playing();
		//TM_ILI9341_Puts(1, 100, "Hello! Please put all 5 cups!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	}

	
	
	switch (task_counter) {
		case 0:	// Clap detection
			TM_ADC_Init(ADC1, ADC_Channel_3);
		  GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			break;
		case 1: // Silence detection
			//setSilenceThresh(SILENCE_AMPLITUDE); 
		  GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			break;
		case 2:  // Motion detection
			Configure_MotionSensorPort();
		  GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			break;
		case 3:  // Whistle Detection
		  GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			break;
		case 4:  // Pulse Readings
			TM_ADC_Init(ADC2, ADC_Channel_8);
		  GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);

			break;
	}

	while (task_counter == get_task_counter() && getAll_cups_present() ) {
		switch (task_counter) {
			case 0:	// Clap detection
			  DetectClap();
				break;
			case 1: // Silence detection
				SilenceDetection();
				break;
			case 2:  // Motion detection
			  MotionDetection();
				break;
			case 3:  // Whistle Detection
				DetectWhistle();
				break;
			case 4:  // Pulse Readings
				ReadPulse();
				break;
		}

		/*
		if (usart_break_required()) {
			break_flag = true;
			return;
		}
		 */

		// TODO:
		/* 1) check usart
			 2) if can be handled, then handle it right here!
			 3) if can not be handled, then break out of this function to the outer while
			 5) let the outer while handle the request (reset the state)
		 */
		check_usart_while_playing();
		if (break_flag) return;
	}


	//TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
}

void check_usart_while_playing(){
		incoming_packet_t incoming_packet;
	
		unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	
		if (usart_has_data()) {
			
			usart_get_data_packet(packet);
			incoming_packet = usart_packet_parser(packet);
			if (usart_validate_crc8(incoming_packet) && usart_packet_is_addressed_to_me(incoming_packet)){
			BlinkOnboardLED(2);
				switch (incoming_packet.instruction) {
					case INSTR_MASTER_TEST:
						SendInstruction(INSTR_SLAVE_READY); //TODO: think about, how the device can not be ready...
						break;
					case INSTR_MASTER_WORK_START:
						
						break;
					case INSTR_MASTER_STATUS_REQ:				
						if (get_task_counter() == TASK_COUNT) {
							SendInstruction(INSTR_SLAVE_COMPLETED);
						} else {
							SendInstruction(INSTR_SLAVE_NOT_COMLETED);
						}
						break;
					case INSTR_MASTER_SET_IDLE:
						GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_2);
						setClaps(0);
						break_flag = true;
						return;
					case CINSTR_GOTO_END:
						set_task_counter(get_task_counter() + 1); // Skips a task
						PerformQuest();
						break;
					case CINSTR_RESTART_TASK:
						setSecondsCount(0);
						setClaps(0);
						setSilenceThresh(SILENCE_AMPLITUDE);
						resetSilenceThresh();
						set_task_counter(get_task_counter());
						PerformQuest();
						break;
					case TEST_DISP:
						Test_7Seg();
						break;
					case TASK_REQUEST:
						//put_char(get_task_counter()+1);
					  SendInstruction(get_task_counter()+1);
						break;
					case SIL_THR_REQUEST:
						SendInstruction(getSilenceThresh());
						//put_char(getSilenceThresh());
						//put_char('w');
						break;
					case SYS_RESET:
						NVIC_SystemReset();
						break;
				}	
			}
		}
		
		free(packet);

}

int main(void) {
	SystemInit(); /* Initialize system */
	
	TM_DELAY_Init(); /* Delay init */
	
	//TM_DISCO_LedInit(); /* Initialize LED's on board */

	INTTIM2_Config();
	INTTIM5_Config();

	/* Initialize LCD */
	
	//TM_ILI9341_Init();
	//TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);

	/* Initialize ADC */
	//TM_ADC_Init(ADC1, ADC_Channel_3); 		// PA3 Microphone's ADC 
	//TM_ADC_Init(ADC2, ADC_Channel_8); 		// PC1 PulseSensor's ADC, will be initialized after tasks with microphone are through

	Configure_CupDetection();	
	Configure_Onboard_LEDS();	
	Configure_485();
	Configure_12V_LEDS();
	Configure_LED_indicator();
	
	//GPIO_ToggleBits(RS485_GPIO, RS485_EN_PIN);
	
	init_usart();
	
	GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_1);

	
	Delayms(300);
	

	
	//TM_ILI9341_Puts(1, 41, "Status: Idle", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	int len = DATA_PACKET_LEN + 1;
	unsigned char packet[len];
	incoming_packet_t incoming_packet;
	
	//Test_7Seg();
	
	clearBuffer();

	

	while (1) {	

//	SendInstruction(INSTR_SLAVE_COMPLETED);
//		put_str("www");
		//put_char('w');
	//	GPIO_ToggleBits(LED_GPIO, LED_1);
	//	GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
		
  // 	Delayms(500);
		
		if (usart_has_data()) {
			
			
			
			usart_get_data_packet(packet);
			incoming_packet = usart_packet_parser(packet);
			
			if (usart_validate_crc8(incoming_packet) && usart_packet_is_addressed_to_me(incoming_packet)){
				BlinkOnboardLED(2);
				switch (incoming_packet.instruction) {
					case INSTR_MASTER_TEST:
						SendInstruction(INSTR_SLAVE_READY); //TODO: think about, how the device can not be ready...
						break;
					case INSTR_MASTER_WORK_START:
						while (get_task_counter() <= TASK_COUNT) {
							Control_12V_LEDs();
							//if(getAll_cups_present() == 1) GPIO_SetBits(LED_GPIO, STATE_LED);
							//if(getAll_cups_present() == 0) GPIO_ResetBits(LED_GPIO, STATE_LED);
							PerformQuest();
							if(break_flag){
								GPIO_ResetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4 | LED_5);
								set_task_counter(0);
								setSecondsCount(0);
								TIM_Cmd(TIM2, DISABLE);
								//TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
								break_flag = false;
								break;
							}
						}

						break;
					case INSTR_MASTER_STATUS_REQ:				
						if (get_task_counter() == TASK_COUNT) {
							SendInstruction(INSTR_SLAVE_COMPLETED);
						} else {
							SendInstruction(INSTR_SLAVE_NOT_COMLETED);
						}
						break;
					case INSTR_MASTER_SET_IDLE:
						set_task_counter(0);
						break;
					case CINSTR_GOTO_END:
						set_task_counter(TASK_COUNT + 1); // TODO: TAKE A LOOK
						PerformQuest();
						break;
					case TEST_DISP:
						Test_7Seg();
						break;
					case SYS_RESET:
						NVIC_SystemReset();
						break;
				}				
			}
		}
		
		set_task_counter(0);
		
	}
}
