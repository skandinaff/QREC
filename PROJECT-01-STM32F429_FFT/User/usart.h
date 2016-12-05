/* ----------------------------------------------------------
*
* Valid USART commands:
*
* 0xC1 0x10 0x01 0x50 0xC0 Tests
* 	 Work Start
* 0xC1 0x10 0x03 0x5E 0xC0 Stauts request
* 0xC1 0x10 0x04 0x4B 0xC0 Idle		
* -- Custom commands, not listed in the original protocol
* 0xC1 0x10 0x7F 0x2D 0xC0 Go to the next task
* 0xC1 0x10 0x7E 0x2A 0xC0 Restart current task
* 0xC1 0x10 0x7D 0x23 0xC0 Test 7 segement display
* 0xC1 0x10 0x7C 0x24 0xC0 Ask what task is now active
* 0xC1 0x10 0x7A 0x36 0xC0 Ask what is threshold value in Silence Detection
* 0xC1 0x10 0x79 0x3F 0xC0 Perform system reset
* 0xC1 0x10 0x78 0x38 0xC0 Go Straight to Pulse Reading
* 0xC1 0x10 0x77 0x15 0xC0 Start quest with cups override, for test purpouses
*	0xC1 0x10 0x76 0x12 0xC0 Ask how many cups are detected
*
* ---------------------------------------------------------- */

//TODO: In idle don't anwere to StatReq, in Game don't answer to Test

#ifndef USART_H
#define USART_H

//#include "tm_stm32f4_usart.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stdlib.h"
#include "init_periph_irqs.h"
#include "tm_stm32f4_delay.h"
#include "leds.h"
#include "quest.h"
#include "cup_detection.h"

#define USART_BAUD_RATE 					19200

#define QUEST_ID									0x10
#define DATA_PACKET_LEN		        7
#define OUTGOING_PACKET_LENGTH    5
#define MASTER_START_BYTE	        0xC1
#define SLAVE_START_BYTE	        0xC2
#define STOP_BYTE									0xC0
#define RESTRICTED_BYTE           0x7B
#define TEST_DISP									0x7D
#define TASK_REQUEST							0x7C
#define SIL_THR_REQUEST						0x7A	
#define SYS_RESET									0x79
#define PULSE											0x78
#define WS_TEST_MODE							0x77
#define HM_CUPS										0x76

//------------- Instructions from main device
#define INSTR_MASTER_TEST					0x01
#define INSTR_MASTER_WORK_START		0x02			
#define INSTR_MASTER_STATUS_REQ		0x03			
#define INSTR_MASTER_SET_IDLE			0x04

//------------- Instructions from this device
#define INSTR_SLAVE_NOT_READY			0x01
#define INSTR_SLAVE_READY					0x02
#define INSTR_SLAVE_NOT_COMLETED	0x03			
#define INSTR_SLAVE_COMPLETED			0x04
//------------- Supplemetery instructions for test purpouses
#define CINSTR_GOTO_END           0x7F // Not end but next task
#define CINSTR_RESTART_TASK				0x7E

#define CRC_INIT_VAL 							0x00
#define CRC_POLYNOM 							0x07


typedef struct {
	int packet_length;
	
	char master_stat_byte;
	char slave_address;
	char instruction;
	char player_count;
	char hint_count;
	char crc8;
	char stop_byte;
} incoming_packet_t;


typedef struct {
	char slave_start_byte;
	char slave_address;
	char instruction;
	char crc8;
	char stop_byte;
} outgoing_packet_t;

void init_usart(void);
void usart_put_data_on_lcd(unsigned char* input);
void USART3_IRQHandler(void);
unsigned char get_char(void);
bool usart_has_data(void);
void usart_get_data_packet(unsigned char* packet);
void put_char(uint8_t c);
void put_str(unsigned char *s); 
bool usart_packet_is_addressed_to_me(incoming_packet_t incoming_packet);
incoming_packet_t usart_packet_parser(unsigned char* packet);
outgoing_packet_t usart_assemble_response(unsigned char instruction);
void usart_convert_outgoing_packet (unsigned char* packet, outgoing_packet_t outgoing_packet);
uint8_t usart_crc8(uint8_t init, uint8_t *packet);
bool usart_validate_crc8(incoming_packet_t incoming_packet);
bool usart_break_required(void);
uint8_t SendInstruction(unsigned char instruction);
void check_usart_while_playing(void);
void set_break_flag(bool bf);
bool get_break_flag(void);

void set_usart_active(bool ua);
bool get_usart_active(void);

#endif
