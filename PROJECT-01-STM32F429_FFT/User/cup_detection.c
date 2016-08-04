#include "cup_detection.h"


unsigned char c1_state = 0;
unsigned char c2_state = 0;
unsigned char c3_state = 0;
unsigned char c4_state = 0;
unsigned char c5_state = 0;

volatile uint16_t cstate;

int _task_counter = 0;


bool all_cups_present;

void reset_task_counter(void) {
    _task_counter = 0;
}


int get_task_counter(void) {
    return _task_counter;
}


void set_task_counter(int counter) {
    _task_counter = counter;
}


uint8_t DetectCups(void) {
    c1_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7);
    c2_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5);
    c3_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);
    c4_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4);
    c5_state = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);

    return c1_state + c2_state + c3_state + c4_state + c5_state;
}


void CheckIfAllCupsPresent(void) {
    if (DetectCups() < 5) {
        setAll_cups_present(false);
        setCstate(0);
    }
}

bool getAll_cups_present(void) {
    return all_cups_present;
}


void setAll_cups_present(bool s) {
    all_cups_present = s;
}


uint16_t getCstate(void) {
    return cstate;
}


void setCstate(uint16_t s) {
    cstate = s;
}