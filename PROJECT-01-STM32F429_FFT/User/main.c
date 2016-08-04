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

		Pins used:

		PA3 - Microphone
		PF8 - Pulse Sensor
		PA10 - USART
		PA9 - USART

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

// Function prototypes
void PerformQuest(void);
uint8_t SendInstruction(unsigned char instruction);
void idle(void);


//bool break_flag = false;


uint8_t SendInstruction(unsigned char instruction){
	unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	outgoing_packet_t outgoing_packet = usart_assemble_response(instruction);
	usart_convert_outgoing_packet(packet, outgoing_packet);
	put_str(packet);
	Delayms(100);
	return 1;
}


void PerformQuest(void){
	char state[1];

	unsigned int c_state = getCstate();
	int task_counter = get_task_counter();

	TM_ILI9341_Fill(ILI9341_COLOR_WHITE);

	if (c_state == 0) { // Wait for cups to be placed
		sprintf(state, "%d", getCstate());
		TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		TM_ILI9341_Puts(1, 100, "Hello! Please put all 5 cups!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		if (task_counter != TASK_COUNT && DetectCups() == 5) {
			setCstate(getCstate() + 1);
			setAll_cups_present(true);
		}
	} else if (c_state == 6) { // All Done, you Won
		sprintf(state, "%d", getCstate());
		TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		TM_ILI9341_Puts(1, 100, "All done!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		Delayms(2000);

		SendInstruction(INSTR_SLAVE_COMPLETED);

		reset_task_counter();
		setCstate(0);
	} else { // We are performing the quest
		sprintf(state, "%d", getCstate());
		TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

		switch (c_state) {
			case 1:  // Whistle Detection
				break;
			case 2:  // Pulse Readings
				setTIM5_count(1);
				TM_DISCO_LedOff(LED_RED);
				TM_DISCO_LedOff(LED_GREEN);
				break;
			case 3:  // Motion detection
				Configure_MotionSensorPort();
				break;
			case 4:	// Clap detection
				break;
			case 5: // Silence detection
				break;
		}

		int old_task_counter = get_task_counter();
		while (get_task_counter() == old_task_counter && getAll_cups_present()) {
			switch (c_state) {
				case 1:  // Whistle Detection
					DetectWhistle();
					break;
				case 2:  // Pulse Readings
					ReadPulse();
					break;
				case 3:  // Motion detection
					MotionDetection();
				case 4:	// Clap detection
					DetectClap();
					break;
				case 5: // Silence detection
					SilenceDetection();
					break;
				default:
					setCstate(0);
					break;
			}

			/*
			if (usart_break_required()) {
				break_flag = true;
				return;
			}
			 */
		}

		// TODO: fix
		if (!getAll_cups_present()) {
			setCstate(0);
		} else if (getAll_cups_present()) {
			setCstate(c_state + 1);
		}

		TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
	}
}



int main(void) {
	SystemInit(); /* Initialize system */
	
	TM_DELAY_Init(); /* Delay init */
	
	TM_DISCO_LedInit(); /* Initialize LED's on board */

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
	Delayms(300);
	
	TM_ILI9341_Puts(1, 41, "Status: Idle", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

	int len = DATA_PACKET_LEN + 1;
	unsigned char packet[len];
	incoming_packet_t incoming_packet;
	
	while (1) {	
		//label: infiniteloop;
		
		if (usart_has_data()) {
			usart_get_data_packet(packet);
			incoming_packet = usart_packet_parser(packet);
			
			if (usart_validate_crc8(incoming_packet) && usart_packet_is_addressed_to_me(incoming_packet)){
				switch (incoming_packet.instruction) {
					case INSTR_MASTER_TEST:
						SendInstruction(INSTR_SLAVE_READY); //TODO: think about, how the device can not be ready...
						break;
					case INSTR_MASTER_WORK_START:
						while (get_task_counter() <= TASK_COUNT) {
							PerformQuest();
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
						setCstate(6);
						set_task_counter(5); // TODO: TAKE A LOOK
						PerformQuest();
						break;
				}				
			}
		}
		
		// break_flag = false;

		set_task_counter(0);
		
		//put_str("w");
		//Delayms(500);
	}
}
