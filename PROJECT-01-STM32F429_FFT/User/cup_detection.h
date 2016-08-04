#ifndef CUP_DETECTION_H
#define CUP_DETECTION_H

#define TASK_COUNT 5

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_gpio.h"

void reset_task_counter(void);
int get_task_counter(void);
void set_task_counter(int counter);

uint8_t DetectCups(void);
bool getAll_cups_present(void);
void setAll_cups_present(bool s);
uint16_t getCstate(void);
void setCstate(uint16_t s);
void CheckIfAllCupsPresent(void);

#endif
