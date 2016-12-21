#include "init_periph_irqs.h"

long secondsCount = 0;
long tim2_count = 0;
long tim5_count = 0;
long tim5_count2 = 0;
long tim5_count3 = 0;
long tim5_count4 = 0;
long counter = 0;
long counter2 = 0;
long counter3 = 0;
long counter4 = 0;
long tim4_counter0 = 0;
long tim4_count0 = 0;

long led_counter = 0;
int xLED = 0;

volatile uint32_t sampleCounterIRQ = 0;          // used to determine pulse timing, triggered by Timer 5 IRQ

void INTTIM4_Config(void){

	
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the TIM4 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100 - 1; //
  TIM_TimeBaseStructure.TIM_Prescaler = 900 - 1; // 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  /* TIM IT enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  /* TIM4 enable counter */
  //TIM_Cmd(TIM4, ENABLE); // We are enableing it on demand
}

void INTTIM3_Config(void){

	
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100000 - 1; //
  TIM_TimeBaseStructure.TIM_Prescaler = 1200 - 1; // 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  /* TIM2 enable counter */
  TIM_Cmd(TIM3, ENABLE); // We are enableing it on demand
}

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
	
	/* This timer should give us interrupt every 2ms for pulse sensor 
		Actually it gives one per 1ms */
	
  NVIC_InitTypeDef NVIC_InitStructure_TIM5;
  /* Enable the TIM5 gloabal Interrupt */
  NVIC_InitStructure_TIM5.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure_TIM5.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure_TIM5);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  /* TIM5 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100 - 1;  // 1 MHz down to 1 KHz (1 ms)
  TIM_TimeBaseStructure.TIM_Prescaler = 900 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  /* TIM IT enable */
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
  /* TIM5 enable counter */
  //TIM_Cmd(TIM5, ENABLE); // This timer runs all the time. 
												 // Used to calculate BPM value
}

void Configure_CupDetection(void){ // That is for Cup Detection Buttons, sensors, whatever
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
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



void Configure_MotionSensorPort(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
}

void Configure_Pulse_CapSens(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
}

void TIM2_IRQHandler(void) {

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		secondsCount += 1;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		}
}

void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		led_counter += 1;
		if(xLED!=0) GPIO_ToggleBits(LED_GPIO, xLED);
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		}
}

void TIM5_IRQHandler(void) {


	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		
		//GPIO_ToggleBits(LED_GPIO, STATE_LED);
		
		sampleCounterIRQ += 1;	// This increments a counter for pulse sensor

		

		counter += 1;          // This horrible thing here helps to get ~2ms refresh rate for drawing signal
		counter2 += 1;
		counter3 += 1;
		counter4 += 1;
		
		if (counter == 50) {	// This gives you 50*2ms=100ms (why did I need that??)
			tim5_count += 1;
			counter = 0;
		}
		if (counter2 == 150) { //And this gives you 150*2=300ms (why did I need that )
			tim5_count2 += 1;
			counter2 = 0;
		}
		if (counter3 == 25) { //25*0.250
			tim5_count3 += 1;
			counter3 = 0;
		}
		if (counter4 == 1){ // 1*0.250
			tim5_count4 += 1;
			counter4 = 0;
		}

	}
}

void TIM4_IRQHandler(void) {
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		tim4_counter0 +=1;
		if (tim4_counter0 == 50){
			tim4_count0+=1;	
		}
	}
}

uint16_t getSecondCount(void){
	return secondsCount;
}

void setSecondsCount(uint16_t s){
	secondsCount = s;
}

uint16_t getLEDCount(void){
	return led_counter;
}

void setLEDCount(uint16_t s){
	led_counter = s;
}

uint16_t get_xLED(void){
	return xLED;
}

void set_xLED(uint16_t s){
	xLED = s;
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
uint16_t getTIM5_count3(void){
	return tim5_count3;
}
void setTIM5_count3(uint16_t s){
	tim5_count3 = s;
}

uint16_t getTIM5_count4(void){
	return tim5_count4;
}
void setTIM5_count4(uint16_t s){
	tim5_count4 = s;
}
///
uint16_t getTIM4_count0(void){
	return tim4_count0;
}
void setTIM4_count0(uint16_t s){
	tim4_count0 = s;
}

