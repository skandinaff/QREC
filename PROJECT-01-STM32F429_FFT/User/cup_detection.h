#ifndef CUP_DETECTION_H
#define CUP_DETECTION_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_gpio.h"

typedef struct {

bool detect_whistle;
bool read_pulse;
bool detect_movement;
bool detect_silence;
bool detect_clap;
bool all_tasks;

} flags_t;

void reset_all_flags(void);

flags_t get_flags(void);
void set_flags(flags_t new_flags);
uint8_t DetectCups(void);
bool getAll_cups_present(void);
void setAll_cups_present(bool s);
uint16_t getCstate(void);
void setCstate(uint16_t s);
void CheckIfAllCupsPresent(void);

#endif
