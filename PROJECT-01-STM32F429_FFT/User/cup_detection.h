#ifndef CUP_DETECTION_H
#define CUP_DETECTION_H

#define TASK_COUNT 5

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_gpio.h"
#include "leds.h"

void reset_task_counter(void);
int get_task_counter(void);
void set_task_counter(int counter);

uint8_t DetectCups(void);
uint16_t getCstate(void);
void setCstate(uint16_t s);
bool getAll_cups_present(void);

#endif
