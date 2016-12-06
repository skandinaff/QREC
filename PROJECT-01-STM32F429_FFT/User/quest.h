#ifndef QUEST_H
#define QUEST_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stdlib.h"


#include "leds.h"
#include "usart.h"
#include "movement_detection.h"
#include "LPH8731-3C.h"

typedef enum {IDLE, GAME} state;
typedef enum {COMPLETED, NOT_COMPLETED} result;

void PerformQuest(void);

state get_game_state(void);
void set_game_state(state s);

result get_game_result(void);
void set_game_result(result r);

void Emergency_Stop(void);

#endif
