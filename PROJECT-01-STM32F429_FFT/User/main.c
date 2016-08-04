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
void PerformqQuest(void);
uint8_t SendInstruction(unsigned char instruction);
void idle(void);


bool break_flag = false;


uint8_t SendInstruction(unsigned char instruction){
	unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	outgoing_packet_t outgoing_packet = usart_assemble_response(instruction);
	usart_convert_outgoing_packet(packet, outgoing_packet);
	put_str(packet);
	Delayms(100);
	return 1;
}


void PerformqQuest(void){
		char state[1];
	
		flags_t cflags = get_flags();
		
		switch (getCstate()) {
			case 0: // Wait for cups to be placed
			sprintf(state, "%d", getCstate());
			TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
			TM_ILI9341_Puts(1, 100, "Hello! Please put all 5 cups!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				if(!cflags.all_tasks){
					if(DetectCups() == 5){
						setCstate(getCstate()+1);
						setAll_cups_present(true);
					}
				}
				break;
			case 1:  // Whistle Detection 
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				sprintf(state, "%d", getCstate());
				TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
				Delayms(300);
			
				while (!cflags.detect_whistle && getAll_cups_present()) {
					DetectWhistle();
					
					if (usart_break_required()) {
						break_flag = true;
						return;
					}
					
					cflags = get_flags();
				}
			
				if (!getAll_cups_present()) {
					setCstate(0);
				} else if(getAll_cups_present()) {
					setCstate(getCstate() + 1);
				}
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
			case 2:  // Pulse Readings
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				sprintf(state, "%d", getCstate());
				TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
			
				setTIM5_count(1);
				TM_DISCO_LedOff(LED_RED);
				TM_DISCO_LedOff(LED_GREEN);
			
				while (!cflags.read_pulse && getAll_cups_present()) {
					ReadPulse();
					if (usart_break_required()) {
						break_flag = true;
						return;
					}
					cflags = get_flags();
				}
				
				if (!getAll_cups_present()) {
					setCstate(0);
				} else if (getAll_cups_present()) {
					setCstate(getCstate()+1);
				}
			
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
			case 3:  // Motion detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				sprintf(state, "%d", getCstate());
				TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
	
				Configure_MotionSensorPort();
			
				while (!cflags.detect_movement && getAll_cups_present()) {
					MotionDetection();
					if (usart_break_required()) {
						break_flag = true;
						return;
					}
					cflags = get_flags();
				}
				
				if (!getAll_cups_present()) {
					setCstate(0);
				} else if(getAll_cups_present()) {
					setCstate(getCstate()+1);
				}
			
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);	
				break;
			case 4:	// Clap detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				sprintf(state, "%d", getCstate());
				TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
	
				while (!cflags.detect_clap && getAll_cups_present()) {
					DetectClap();
					
					if (usart_break_required()) {
						break_flag = true;
						// put_str("X");
						cflags = get_flags();
						return;
					}	 else {
						
						// put_str("A");
						// ===================================================================
						// TODO: fix....
						// ===================================================================
						// usart_check_for_data();
					}
					
					cflags = get_flags();
				}
			
				if (!getAll_cups_present()) {
					setCstate(0);
				} else if (getAll_cups_present()) {
					setCstate(getCstate()+1);	
				}
				
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
				
			case 5: // Silence detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				sprintf(state, "%d", getCstate());
				TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	
				while (!cflags.detect_silence && getAll_cups_present()) {
					SilenceDetection();
						if (usart_break_required()) {
							break_flag = true;
							return;
						}
					cflags = get_flags();
				}
			
				if (!getAll_cups_present()) { 
					setCstate(0); 
				} else if(getAll_cups_present()) { 
					setCstate(getCstate()+1); 
				}		
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);				
				break;
			case 6: // All Done, you Won
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
				sprintf(state, "%d", getCstate());
				TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
	
			
				TM_ILI9341_Puts(1, 100, "All done!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				Delayms(2000);
			
				SendInstruction(INSTR_SLAVE_COMPLETED);	
			
				reset_all_flags();
				cflags.all_tasks = true;
				set_flags(cflags);
			
				break;
			default:
				setCstate(0);
				break;
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

	flags_t cflags;
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
						while (!(cflags.all_tasks || break_flag)) {
							PerformqQuest();
							cflags = get_flags();
						}
						break;
					case INSTR_MASTER_STATUS_REQ:				
						if (cflags.all_tasks) {
							SendInstruction(INSTR_SLAVE_COMPLETED);
						} else {
							SendInstruction(INSTR_SLAVE_NOT_COMLETED);
						}
						break;
					case INSTR_MASTER_SET_IDLE:
						reset_all_flags();
						cflags.all_tasks = false;
						set_flags(cflags);
						break;
					case CINSTR_GOTO_END:
						setCstate(6);
						PerformqQuest();
						break;
				}				
			}
		}
		
		break_flag = false;

		reset_all_flags();
		cflags = get_flags();
		cflags.all_tasks = false;
		set_flags(cflags);
		
		//put_str("w");
		//Delayms(500);
	}
}
