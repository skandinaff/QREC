#include "init_periph_irqs.h"

long secondsCount = 0;
long tim2_count = 0;
long tim5_count = 0;
long tim5_count2 = 0;
long counter = 0;
long counter2 = 0;

volatile uint32_t sampleCounterIRQ = 0;          // used to determine pulse timing, triggered by Timer 5 IRQ

void INTTIM2_Config(void){

	
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100000 - 1; //
  TIM_TimeBaseStructure.TIM_Prescaler = 450 - 1; // 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  /* TIM2 enable counter */
  //TIM_Cmd(TIM2, ENABLE); // We are enableing it on demand
}

void INTTIM5_Config(void){

	
  NVIC_InitTypeDef NVIC_InitStructure_TIM5;
  /* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure_TIM5.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure_TIM5);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100 - 1;  // 1 MHz down to 1 KHz (1 ms)
  TIM_TimeBaseStructure.TIM_Prescaler = 900 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  /* TIM IT enable */
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM5, ENABLE); // This timer runs all the time. 
												 // Used to calculate BPM value
}

void Configure_PD(void){ // That is for Cup Detection Buttons, sensors, whatever
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
}

void Configure_PD_LEDS(void){  // Onboards LEDs
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(ONBOARD_LED_RCC, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = ONBOARD_LED_1 | ONBOARD_LED_2 | ONBOARD_LED_3 | ONBOARD_LED_4;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(ONBOARD_LED_GPIO, &GPIO_InitStruct);
	
	GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_1 | ONBOARD_LED_2 | ONBOARD_LED_3 | ONBOARD_LED_4);
	
}

void Configure_485(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = RS485_EN_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(RS485_GPIO, &GPIO_InitStruct);
}

void Configure_12V_LEDS(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(LED_RCC, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = LED_1 | LED_2 | LED_3 | LED_4 | LED_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(LED_GPIO, &GPIO_InitStruct);
	
	//GPIO_SetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4);
}

void Configure_MotionSensorPort(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
}

void Configure_LED_indicator(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
}

void TIM2_IRQHandler(void) {

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		secondsCount += 1;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		}
}



void TIM5_IRQHandler(void) {


	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

		sampleCounterIRQ += 1;


		counter += 1;          // This horrible thing here helps to get ~0.250ms refresh rate for drawing signal
		counter2 += 1;
		
		if (counter == 50) {	// This gives you 
			tim5_count += 1;
			counter = 0;
		}
		if (counter2 == 150) {
			tim5_count2 += 1;
			counter2 = 0;
		}

	}
}

uint16_t getSecondCount(void){
	return secondsCount;
}

void setSecondsCount(uint16_t s){
	secondsCount = s;
}

uint16_t getQrSecondCount(void){
	return tim2_count;
}

void setQrSecondsCount(uint16_t s){
	tim2_count = s;
}

uint32_t getSampleCounterIRQ(void){
	return sampleCounterIRQ;
}
void setSampleCounterIRQ(uint32_t s){
	sampleCounterIRQ = s;
}
uint16_t getCounter(void){
	return counter;
}
void setCounter(uint16_t s){
	counter = s;
}
uint16_t getCounter2(void){
	return counter2;
}
void setCounter2(uint16_t s){
	counter2 = s;
}
uint16_t getTIM5_count(void){
	return tim5_count;
}
void setTIM5_count(uint16_t s){
	tim5_count = s;
}
uint16_t getTIM5_count2(void){
	return tim5_count2;
}
void setTIM5_count2(uint16_t s){
	tim5_count2 = s;
}

