#include "cup_detection.h"


unsigned char c1_state = 0;
unsigned char c2_state = 0;
unsigned char c3_state = 0;
unsigned char c4_state = 0;
unsigned char c5_state = 0;

volatile uint16_t cstate;

uint8_t _task_counter = 0; // that will be our inital task. Default: 0

bool all_cups_present;
bool cups_override = false;

void reset_task_counter(void) {
    _task_counter = 0;
}


int get_task_counter(void) {
    return _task_counter;
}


void set_task_counter(int counter) {
		if(counter > _task_counter) BlinkOnboardLED(4); // To indicate change of task
    _task_counter = counter;
}


uint8_t DetectCups(void) {
	// TODO: Invert all the states, since schematics reqire that (c1_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8); etc)
    c1_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8);
    c2_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9);
    c3_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10);
    c4_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11);
    c5_state = !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12);

    return c1_state + c2_state + c3_state + c4_state + c5_state;
		// For debug with no reed used this instead:
		//return 5;
}


bool getAll_cups_present(void) {
		if(cups_override) return true;
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
