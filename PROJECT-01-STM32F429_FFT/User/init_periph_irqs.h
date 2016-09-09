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

#define ONBOARD_LED_RCC RCC_AHB1Periph_GPIOD
#define ONBOARD_LED_GPIO GPIOD
#define ONBOARD_LED_1 GPIO_Pin_3
#define ONBOARD_LED_2 GPIO_Pin_2
#define ONBOARD_LED_3 GPIO_Pin_1
#define ONBOARD_LED_4 GPIO_Pin_0

#define LED_RCC RCC_AHB1Periph_GPIOE
#define LED_GPIO GPIOE
#define LED_1 GPIO_Pin_8
#define LED_2 GPIO_Pin_9
#define LED_3 GPIO_Pin_10
#define LED_4 GPIO_Pin_11
#define LED_5 GPIO_Pin_12

#define RS485_EN_PIN GPIO_Pin_8
#define RS485_GPIO GPIOC

void INTTIM2_Config(void);
void INTTIM5_Config(void);
void Configure_PD(void);
void Configure_PD_LEDS(void);
void Configure_485(void);
void Configure_12V_LEDS(void);
void Configure_MotionSensorPort(void);
void Configure_LED_indicator(void);

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
