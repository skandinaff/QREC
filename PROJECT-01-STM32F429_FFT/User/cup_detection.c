
#include "cup_detection.h"


unsigned char c1_state = 0;
unsigned char c2_state = 0;
unsigned char c3_state = 0;
unsigned char c4_state = 0;
unsigned char c5_state = 0;

volatile uint16_t cstate;

flags_t _cflags;


bool all_cups_present;


uint8_t DetectCups(void){
	
	c1_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7);
	c2_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5);
	c3_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);
	c4_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4);
	c5_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
	
	return c1_state + c2_state + c3_state + c4_state + c5_state;
}

void reset_all_flags(void){
	_cflags.detect_whistle = false;
	_cflags.detect_clap = false;
	_cflags.detect_movement = false;
	_cflags.read_pulse = false;
	_cflags.detect_silence = false;
}

void CheckIfAllCupsPresent(void){
	if(DetectCups()<5){
		setAll_cups_present(false);
		setCstate(0);
	}
}

flags_t get_flags(void){
	return _cflags;
}

void set_flags(flags_t new_flags){
	_cflags = new_flags;
}

bool getAll_cups_present(void){
	return all_cups_present;
}

void setAll_cups_present(bool s){
	all_cups_present = s;
}

uint16_t getCstate(void){
	return cstate;
}

void setCstate(uint16_t s){
	cstate = s;
}
