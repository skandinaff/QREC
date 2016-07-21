#ifndef INIT_PERIPH_IRQS_H
#define INIT_PERIPH_IRQS_H

/* Includes for timer to work */
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"



void INTTIM2_Config(void);
void INTTIM5_Config(void);
void Configure_PD(void);
void Configure_MotionSensorPort(void);

uint16_t getSecondCount(void);
void setSecondsCount(uint16_t s);

uint16_t getQrSecondCount(void);
void setQrSecondsCount(uint16_t s);

uint32_t getSampleCounterIRQ(void);
void setSampleCounterIRQ(uint32_t s);

uint16_t getCounter(void);
void setCounter(uint16_t s);

uint16_t getCounter2(void);
void setCounter2(uint16_t s);

uint16_t getTIM5_count(void);
void setTIM5_count(uint16_t s);

uint16_t getTIM5_count2(void);
void setTIM5_count2(uint16_t s);


#endif
