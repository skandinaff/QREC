#include "cup_detection.h"


unsigned char c1_state = 0;
unsigned char c2_state = 0;
unsigned char c3_state = 0;
unsigned char c4_state = 0;
unsigned char c5_state = 0;

volatile uint16_t cstate;

uint8_t _task_counter = FIRST_TASK; // that will be our inital task. Default: 0

bool all_cups_present;
bool cups_override = false;

bool first_start = false;

void reset_task_counter(void) {
    _task_counter = FIRST_TASK;
}


int get_task_counter(void) {
    return _task_counter;
}


void set_task_counter(int counter) {
		if(counter > _task_counter) BlinkOnboardLED(4); // To indicate change of task
    _task_counter = counter;
}


uint8_t DetectCups(void) {
	  c1_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8);
    c2_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9);
    c3_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10);
    c4_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11);
    c5_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12);
    return c1_state + c2_state + c3_state + c4_state + c5_state;
}


bool getAll_cups_present(void) {
		if(cups_override) { 
			return true;
		}
		if(DetectCups()==5) { 
			TIM_Cmd(TIM3, ENABLE);
			if(get_first_start() == false) set_first_start(true);
		}
    return DetectCups() == 5;
}


uint16_t getCstate(void) {
    return cstate;
}


void setCstate(uint16_t s) {
    cstate = s;
}

void set_cups_override(void){
	cups_override = true;
}

bool get_cups_override(void){
	return cups_override;
}

void set_first_start(bool fs){
	first_start = fs;
}

bool get_first_start(void){
	return first_start;
}