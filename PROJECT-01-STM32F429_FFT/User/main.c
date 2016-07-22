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








uint8_t SendComplPacket(void){
	//TODO: This should be substituted with more universal methode
	unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	outgoing_packet_t outgoing_packet = usart_assemble_response(INSTR_SLAVE_COMPLETED);
	usart_convert_outgoing_packet(packet, outgoing_packet);
	put_str(packet);
	Delayms(100);
	return 1;
}



int main(void) {
	
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
	
	
	Delayms(300);

	unsigned char* packet = malloc((DATA_PACKET_LEN + 1) * sizeof(unsigned char));
	incoming_packet_t incoming_packet;
	
	reset_all_flags();
	
	flags_t cflags;
	
	cflags.all_tasks = false;
	
	

	while (1) {	

		cflags = get_flags();
		
		switch (getCstate()) {
			case 0: // Wait for cups to be placed
				
				
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
					Delayms(300);
					while(!cflags.detect_whistle && getAll_cups_present()) {
						DetectWhistle(); 
						cflags = get_flags();
					}
				
					if(!getAll_cups_present()) {setCstate(0);}
					else if(getAll_cups_present()) {setCstate(getCstate()+1);}
			
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
					
			case 2:  // Pulse Readings
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
					setTIM5_count(1);
					TM_DISCO_LedOff(LED_RED);
					TM_DISCO_LedOff(LED_GREEN);
				
					while(!cflags.read_pulse && getAll_cups_present()) {
						ReadPulse();
						cflags = get_flags();
					}
					
					if(!getAll_cups_present()) {setCstate(0);}
					else if(getAll_cups_present()) {setCstate(getCstate()+1);}
				
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
					
			case 3:  // Motion detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);

				Configure_MotionSensorPort();
			
					while(!cflags.detect_movement && getAll_cups_present()) {
						MotionDetection();
						cflags = get_flags();
					}
					
					if(!getAll_cups_present()) {setCstate(0);}
					else if(getAll_cups_present()) {setCstate(getCstate()+1);}
			
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);	
			  break;
					
			case 4:	// Clap detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);

				while(!cflags.detect_clap && getAll_cups_present()) {
					DetectClap();
					cflags = get_flags();
				}
			
					if(!getAll_cups_present()) setCstate(0);
					else if(getAll_cups_present()) setCstate(getCstate()+1);	
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
				break;
				
			case 5: // Silence detection
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);

				while(!cflags.detect_silence && getAll_cups_present()) {
					SilenceDetection();
					cflags = get_flags();
				}
			
					if(!getAll_cups_present()) setCstate(0);
					else if(getAll_cups_present()) setCstate(getCstate()+1);		
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);				
				break;
				
			case 6: // All Done, you Win
				TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
			
			  TM_ILI9341_Puts(1, 100, "All done, you win!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			  Delayms(2000);
				do SendComplPacket();
				while(!SendComplPacket());
				reset_all_flags();
				cflags.all_tasks = true;
				setCstate(0);
			
				break;
			default:
				setCstate(0);
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
		sprintf(state, "%d", getCstate());
		TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	

		/*** This is for pulse readings ***/
		if(getQS() == 1) setQS(!getQS());						// A Heartbeat Was Found, reset the Quantified Self flag for next time    
		/**********************************/	
		

		
	set_flags(cflags);
	}
	
	// free(packet);
}
