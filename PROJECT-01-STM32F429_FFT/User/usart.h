#ifndef USART_H
#define USART_H

#include "tm_stm32f4_usart.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "tm_stm32f4_ili9341_ltdc.h"



#define QUEST_ID					0x10
#define DATA_PACKET_LEN		        7
#define OUTGOING_PACKET_LENGTH      5
#define MASTER_START_BYTE	        0xC1
#define SLAVE_START_BYTE	        0xC2
#define STOP_BYTE					0xC0
#define RESTRICTED_BYTE             0x7B

//------------- 
#define INSTR_MASTER_TEST			0x01
#define INSTR_MASTER_WORK_START		0x02			
#define INSTR_MASTER_STATUS_REQ		0x03			
#define INSTR_MASTER_SET_IDLE		0x04

//------------- 
#define INSTR_SLAVE_NOT_READY		0x01
#define INSTR_SLAVE_READY			0x02
#define INSTR_SLAVE_NOT_COMLETED	0x03			
#define INSTR_SLAVE_COMPLETED		0x04

#define CRC_INIT_VAL 0x00
#define CRC_POLYNOM 0x07


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

// Functions protoypes

void init_usart(void);
void send_data(unsigned char tx_data[DATA_PACKET_LEN]); 
void usart_put_data_on_lcd(unsigned char* input);
void USART1_IRQHandler(void);
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

#endif
