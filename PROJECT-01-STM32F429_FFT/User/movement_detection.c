#include "movement_detection.h"

char scd[15];

void MotionDetection(void) {
    bool motion_s1 = GPIO_ReadInputDataBit(PIR_PORT, PIR_1_PIN);

		
		Delayms(3); // Delay that keeps everything consistent
		
    if (!motion_s1) { // if there was a motion
        setSecondsCount(0);
        TIM_Cmd(TIM2, DISABLE);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			
    } else {
        TIM_Cmd(TIM2, ENABLE);
				LCD_Puts("T w/o mvmnt: ", 1, 1, WHITE, BLACK,1,1);
        sprintf(scd, "%d", getSecondCount());
				LCD_Puts(scd, 1, 10, WHITE, BLACK,1,1);

				GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
				GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);

			
        if (getSecondCount() >= NO_MOTION_TIME) {
            Delayms(100);
					  TIM_Cmd(TIM2, DISABLE);
            setSecondsCount(0);					
            set_task_counter(get_task_counter() + 1);
						

        }
    }

}
