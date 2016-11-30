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

void PerformQuest(void);

#endif
