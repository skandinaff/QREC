#include "movement_detection.h"

void MotionDetection(void) {
    bool motion_s1 = GPIO_ReadInputDataBit(PIR_PORT, PIR_1_PIN);
    //bool motion_s2 = GPIO_ReadInputDataBit(PIR_PORT, PIR_2_PIN);
    //bool motion = (motion_s1 || motion_s2);
    
		/*Display section
		char mvm_s1[1];
    char mvm_s2[1];
    char scd[8];
    sprintf(mvm_s1, "%d", motion_s1);
    sprintf(mvm_s2, "%d", motion_s2);
    TM_ILI9341_Puts(20, 20, mvm_s1, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    TM_ILI9341_Puts(20, 40, mvm_s2, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		*/
		
		Delayms(3); // Delay that keeps everything consistent
		
    if (!motion_s1 ) { // if there was a motion
        setSecondsCount(0);
        TIM_Cmd(TIM2, DISABLE);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			
	//		ControlBiColorLED(BC_LED_GREEN, true);
	//		ControlBiColorLED(BC_LED_RED, false);
        //TM_ILI9341_Puts(1, 65, "                              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    } else {
        TIM_Cmd(TIM2, ENABLE);
			// TODO: remove this in production. For tests only
			//	addToBuffer(getSecondCount(),false,false);
			// ***
        //sprintf(scd, "%d", getSecondCount());
        //TM_ILI9341_Puts(20, 65, scd, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
				GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);

			//	ControlBiColorLED(BC_LED_GREEN, false);
			//	ControlBiColorLED(BC_LED_RED, true);
			
			
        if (getSecondCount() >= NO_MOTION_TIME) {
            //TM_ILI9341_Puts(1, 95, "You haven't moved for 10 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            Delayms(100);
					  TIM_Cmd(TIM2, DISABLE);
            setSecondsCount(0);					
            set_task_counter(get_task_counter() + 1);
						

        }
    }

    //GPIO_ResetBits(PIR_PORT, PIR_1_PIN); // Eem... I don't think it's relevant anymore
    //GPIO_ResetBits(PIR_PORT, PIR_2_PIN);
}
