#ifndef INIT_PERIPH_IRQS_H
#define INIT_PERIPH_IRQS_H

/* Includes for timer to work */
/* 
*	Timers in use: TIM2, TIM5
*
*
*/
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"
#include "leds.h"

#define ONBOARD_LED_RCC RCC_AHB1Periph_GPIOD
#define ONBOARD_LED_GPIO GPIOD
#define ONBOARD_LED_1 GPIO_Pin_3
#define ONBOARD_LED_2 GPIO_Pin_2
#define ONBOARD_LED_3 GPIO_Pin_1
#define ONBOARD_LED_4 GPIO_Pin_0

#define PULSE_CAP_SENS GPIO_Pin_2

#define RS485_EN_PIN GPIO_Pin_8
#define RS485_GPIO GPIOC

void INTTIM2_Config(void);
void INTTIM5_Config(void);
void INTTIM3_Config(void);
void Configure_CupDetection(void);
void Configure_485(void);
void Configure_MotionSensorPort(void);
void Configure_Pulse_CapSens(void);

void INTTIM2_Config(void);

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

uint16_t getTIM5_count3(void);
void setTIM5_count3(uint16_t s);

uint16_t getTIM5_count4(void);
void setTIM5_count4(uint16_t s);


uint16_t getLEDCount(void);
void setLEDCount(uint16_t s);

uint16_t get_xLED(void);
void set_xLED(uint16_t s);

#endif
