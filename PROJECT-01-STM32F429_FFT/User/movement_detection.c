#include "movement_detection.h"

void MotionDetection(void){

	flags_t cflags;
	
	cflags = get_flags();
	

	bool motion_s1 = GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1);
	bool motion_s2 = GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3);
	//bool motion = (motion_s1 || motion_s2);
	char mvm_s1[1];
	char mvm_s2[1];
	char scd[8];
	sprintf(mvm_s1, "%d", motion_s1);
	sprintf(mvm_s2, "%d", motion_s2);
	TM_ILI9341_Puts(20, 20, mvm_s1, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);		
	TM_ILI9341_Puts(20, 40, mvm_s2, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);		
	
		if(motion_s1 && motion_s2) {
		TIM_Cmd(TIM2, ENABLE);
		
		sprintf(scd, "%d", getSecondCount());
		TM_ILI9341_Puts(20, 65, scd, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
			
		if(getSecondCount() >= 10){
			TM_ILI9341_Puts(1, 95, "You haven't moved for 10 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			Delayms(1000);
			cflags.detect_movement = true;
			setSecondsCount(0);
			TIM_Cmd(TIM2, DISABLE);			
		}
	 }else if (!motion_s1 || !motion_s2){
		 setSecondsCount(0);
		 TIM_Cmd(TIM2, DISABLE);
		 //TM_ILI9341_Puts(1, 65, "                              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	 }
	 
	 GPIO_ResetBits(GPIOG, GPIO_Pin_1);
	 GPIO_ResetBits(GPIOG, GPIO_Pin_3);
	 
	 CheckIfAllCupsPresent();

	set_flags(cflags);
}
