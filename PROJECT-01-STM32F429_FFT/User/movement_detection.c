#include "movement_detection.h"




void MotionDetection(void){

	flags_t cflags;
	
	cflags = get_flags();
	

	bool motion = GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1);
	char mvm[1];
	char scd[8];
	sprintf(mvm, "%d", motion);
	TM_ILI9341_Puts(20, 20, mvm, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);		
	
		if(motion) {
		TIM_Cmd(TIM2, ENABLE);
		
		sprintf(scd, "%d", getSecondCount());
		TM_ILI9341_Puts(20, 45, scd, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);	
			
		if(getSecondCount() >= 10){
			TM_ILI9341_Puts(1, 65, "You haven't moved for 10 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			Delayms(2000);
			cflags.detect_movement = true;
			setSecondsCount(0);
			TIM_Cmd(TIM2, DISABLE);			
		}
	 }else{
		 setSecondsCount(0);
		 TIM_Cmd(TIM2, DISABLE);
		 //TM_ILI9341_Puts(1, 65, "                              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	 }
	 
	 GPIO_ResetBits(GPIOG, GPIO_Pin_1);
	 
	 CheckIfAllCupsPresent();

	set_flags(cflags);
}
