#include "quest.h"

state game_state;
result game_result;

void PerformQuest(void){
	
	int task_counter = get_task_counter();


	if (!getAll_cups_present()) {
		TIM_Cmd(TIM3, DISABLE);  //TODO: Check if this works upd 4.11.16: seems not to..
		if(get_first_start() == true){
			Control_12V_LED_individually(false); // if false, when remove cups the last activated task will remain dark
			Control_12V_LEDs(); // Moved to here because othewise leds lit before cups were placed
		}
    setSecondsCount(0);	
		check_usart_while_playing();
	}

	
	
	switch (task_counter) {
		case 0:	// Pulse readings
			TIM_Cmd(TIM5, ENABLE); 
			TM_ADC_Init(ADC2, ADC_Channel_8);
			ClearOnboardLEDS();
			setTIM5_count(1);	
			break;
		case 2: // Silence detection
			LCD_FillScreen(BLACK);
			setTIM5_count2(0);
			//TM_ADC_Init(ADC1, ADC_Channel_3);
			ClearOnboardLEDS();
			Delayms(3000);
			break;
		case 3:  // Motion detection
			LCD_FillScreen(BLACK);
			Configure_MotionSensorPort();
			ClearOnboardLEDS();
			break;
		case 1:  // Whistle Detection
			setTIM5_count2(0);
			TM_ADC_Init(ADC1, ADC_Channel_3);
			LCD_FillScreen(BLACK);
			ClearOnboardLEDS();
			break;
		case 4:  // Clap Detection
			LCD_FillScreen(BLACK);
			ClearOnboardLEDS();
			break;
	}

	while (task_counter == get_task_counter() && getAll_cups_present() ) {
		
		if(get_first_start()) Control_12V_LEDs();
		
		switch (task_counter) {
			case 0:	// Clap detection
				ReadPulse();			  
				break;
			case 2: // Silence detection
				SilenceDetectionByEnergy();
				break;
			case 3:  // Motion detection
			  MotionDetection();
				break;
			case 1:  // Whistle Detection
				DetectWhistle();
				break;
			case 4:  // Pulse Readings
				DetectClap();
				break;
		}

		check_usart_while_playing();
		
		if (get_break_flag()) return;

		
	}



}

state get_game_state(){
	return game_state;
}

void set_game_state(state s){
	game_state = s;
}

result get_game_result(){
	return game_result;
}

void set_game_result(result r){
	game_result = r;
}

void Emergency_Stop(void){
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		USART_ITConfig(USART3, USART_IT_ORE_RX, ENABLE);
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		__enable_irq();
		LCD_FillScreen(BLACK);
		set_task_counter(FIRST_TASK);					
		GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_2);
		GPIO_ResetBits(LED_GPIO, STATE_LED);
		set_xLED(0);
		TIM_Cmd(TIM2, DISABLE);
		TIM_Cmd(TIM5, DISABLE);
		TIM_Cmd(TIM3, DISABLE);
		setSecondsCount(0); 
		setTIM5_count(0);
		setTIM5_count2(0);
		setTIM5_count3(0);
		setTIM5_count4(0);
		setClaps(0);
		set_break_flag(true);
		set_first_start(false);
		return;	
}
