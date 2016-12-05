#include "quest.h"

void PerformQuest(void){
	int task_counter = get_task_counter();

	//TM_ILI9341_Fill(ILI9341_COLOR_WHITE);

	//char state[1];
	//sprintf(state, "%d", task_counter);
	//TM_ILI9341_Puts(280, 10, state, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

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
		case 1: // Silence detection
			LCD_FillScreen(BLACK);
			setTIM5_count2(0);
			TM_ADC_Init(ADC1, ADC_Channel_3);
			ClearOnboardLEDS();
			Delayms(3000);
			break;
		case 2:  // Motion detection
			LCD_FillScreen(BLACK);
			Configure_MotionSensorPort();
			ClearOnboardLEDS();
			break;
		case 3:  // Whistle Detection
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
			case 1: // Silence detection
				SilenceDetectionByEnergy();
				break;
			case 2:  // Motion detection
			  MotionDetection();
				break;
			case 3:  // Whistle Detection
				DetectWhistle();
				break;
			case 4:  // Pulse Readings
				DetectClap();
				break;
		}

		// TODO:
		/* 1) check usart
			 2) if can be handled, then handle it right here!
			 3) if can not be handled, then break out of this function to the outer while
			 5) let the outer while handle the request (reset the state)
		 */
		if (get_break_flag()) return;
		check_usart_while_playing();
		
	}


	//TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
}
