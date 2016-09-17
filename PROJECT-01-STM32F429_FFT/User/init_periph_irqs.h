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



#define RS485_EN_PIN GPIO_Pin_8
#define RS485_GPIO GPIOC

void INTTIM2_Config(void);

void Configure_CupDetection(void);
void Configure_485(void);
void Configure_MotionSensorPort(void);


uint16_t getSecondCount(void);
void setSecondsCount(uint16_t s);

#endif
