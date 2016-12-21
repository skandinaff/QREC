#include "leds.h"



//uint8_t numbers[10] = {one, two, three, four, five, six, seven, eight, nine, zero};
uint8_t numbers_wd[] = {0x02, 0x9E, 0x24, 0x0C, 0x98, 0x48, 0x40, 0x1E, 0x01, 0x08};
uint8_t numbers[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09};


uint8_t data;

bool dot2_always_on = false;
//uint8_t dataBuffer[3] = {0xFF, 0xFF, 0xFF}; //uint8_t dataBuffer[NUM_OF_REG];

void Configure_12V_LEDS(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = LED_1 | LED_2 | LED_3 | LED_4 | LED_5 | STATE_LED;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
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

void Configure_Onboard_LEDS(void){  // Onboards LEDs
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

void addToBuffer(int digit, bool dot1, bool dot2){
	//memset(dataBuffer, 0, 3); //memset(dataBuffer, 0, sizeof(dataBuffer));
	uint8_t dataBuffer[3] = {0xFF, 0xFF, 0xFF};
	
	
	if(dot2_always_on) {
		dataBuffer[0] = 0xFF;
		dataBuffer[1] = 0xFE;
		dataBuffer[2] = 0xFF;}
	
	if(dot1 == false && dot2 == false){

		
		int c = 0;
		while(digit > 0){
			uint8_t b = numbers[digit % 10];
			dataBuffer[c] = b;
			digit /= 10;
			c++;
		}
	}	
	

	
	if(dot1 == true){
		if(digit==1){
			dataBuffer[0] = 0xFE;
			dataBuffer[1] = 0xFF;
			dataBuffer[2] = 0xFF;
		}
		if(digit==0){
			dataBuffer[0] = 0xFF;
			dataBuffer[1] = 0xFF;
			dataBuffer[2] = 0xFF;
		}
	}
	
	if(dot2 == true){
		if(digit==1){
			dataBuffer[0] = 0xFF;
			dataBuffer[1] = 0xFE;
			dataBuffer[2] = 0xFF;
		}
		if(digit==0){
			dataBuffer[0] = 0xFF;
			dataBuffer[1] = 0xFF;
			dataBuffer[2] = 0xFF;
		}
	}
	
	GPIO_WriteBit(LED_SEG_GPIO, LATCH_PIN, Bit_RESET);
  
  //for (int a = sizeof(dataBuffer) - 1 ; a >= 0  ; a--) {
  for (int a = 3; a >= 0  ; a--) {
//   for(int a = 0; a<=2; a++){	
    shiftOut(DATA_PIN, CLOCK_PIN, dataBuffer[a]);
  }
  
  GPIO_WriteBit(LED_SEG_GPIO, LATCH_PIN, Bit_SET);

	
}

void clearBuffer(void){
  GPIO_WriteBit(LED_SEG_GPIO, LATCH_PIN, Bit_RESET);
  

   for(int a = 0; a<=2; a++){	
    shiftOut(DATA_PIN, CLOCK_PIN, 0xFF);
  }
  
  GPIO_WriteBit(LED_SEG_GPIO, LATCH_PIN, Bit_SET);

}
	

void shiftOut(int myDataPin, int myClockPin, uint8_t myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;


  //clear everything out just in case to
  //prepare shift register for bit shifting
    GPIO_WriteBit(LED_SEG_GPIO, DATA_PIN, Bit_RESET);
    GPIO_WriteBit(LED_SEG_GPIO, CLOCK_PIN, Bit_RESET);

  //for each bit in the byte myDataOut?
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=0; i<=7; i++)  {
    GPIO_WriteBit(LED_SEG_GPIO, CLOCK_PIN, Bit_RESET);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    
		if(pinState == 1) GPIO_WriteBit(LED_SEG_GPIO, DATA_PIN, Bit_SET);
		if(pinState == 0) GPIO_WriteBit(LED_SEG_GPIO, DATA_PIN, Bit_RESET);
		
    //register shifts bits on upstroke of clock pin  
    GPIO_WriteBit(LED_SEG_GPIO, CLOCK_PIN, Bit_SET);
    //zero the data pin after shift to prevent bleed through
		GPIO_WriteBit(LED_SEG_GPIO, DATA_PIN, Bit_RESET);
  }

  //stop shifting
	GPIO_WriteBit(LED_SEG_GPIO, CLOCK_PIN, Bit_RESET);
}

void Control_12V_LED_individually(bool state){
	switch (get_task_counter()){
		case 0:
			if(state) GPIO_SetBits(LED_GPIO, LED_5);
			if(!state) GPIO_ResetBits(LED_GPIO, LED_5);
			break;
		case 1:
			if(state) GPIO_SetBits(LED_GPIO, LED_2);
			if(!state) GPIO_ResetBits(LED_GPIO, LED_2);
			break;
		case 2:
			if(state) GPIO_SetBits(LED_GPIO, LED_3);
			if(!state) GPIO_ResetBits(LED_GPIO, LED_3);
			break;
		case 3:
			if(state) GPIO_SetBits(LED_GPIO, LED_4);
			if(!state) GPIO_ResetBits(LED_GPIO, LED_4);
			break;
		case 4:
			if(state) GPIO_SetBits(LED_GPIO, LED_1);
			if(!state) GPIO_ResetBits(LED_GPIO, LED_1);			
			break;
	}
}

void Control_12V_LEDs(void){

		//if(getAll_cups_present()){ // Helps physically see if cups are placed or not
		//TIM_Cmd(TIM3, ENABLE);
	
		switch( get_task_counter() ) {
			case 0:
				//set_xLED(LED_1);
				//GPIO_SetBits(LED_GPIO, LED_5);
			
				break;
			case 1:
				//GPIO_SetBits(LED_GPIO, LED_1);
			
				//GPIO_SetBits(LED_GPIO, LED_5); // Because skipping first task
				set_xLED(LED_5);
				
				break;
			case 2:
				GPIO_SetBits(LED_GPIO, LED_5);
				set_xLED(LED_2);
				
				break;
			case 3:
				GPIO_SetBits(LED_GPIO, LED_2);
				set_xLED(LED_4);

				break;
			case 4:
				GPIO_SetBits(LED_GPIO, LED_4);  // In Room 1 Leds 5 and 1 are not swapped, hence this mess.. 
				set_xLED(LED_1);

				//GPIO_SetBits(LED_GPIO, LED_5);  // Blinking removed due to interference with pulse readings
				//set_xLED(0);
				break;
			case 5:
				set_xLED(0);
				GPIO_ResetBits(LED_GPIO, LED_1 | LED_2 | LED_3 | LED_4 | LED_5 | STATE_LED); 
				GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_2);
				//clearBuffer(); 																										// Clearing 7 segment display. Shouldn't be here, but idk where else to put it
				break;
		//}
	}		
}

void BlinkOnboardLED(uint8_t L){
	

	switch(L) {
		case 1:
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_1);
			Delayms(150);
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_1);
			break;
		case 2:
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_2);
			Delayms(50);
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_2);
			break;
		case 3:
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			Delayms(150);
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			break;
		case 4:
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			Delayms(150);
			GPIO_ToggleBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
			break;
	}

}

void Test_7Seg(void){
	clearBuffer();

	for(uint8_t i = 0; i <= 9; i++){
		addToBuffer(i,false,false);
		Delayms(250);
	}
	for(uint8_t i = 10; i <= 90; i+=10){
		addToBuffer(i,false,false);
		Delayms(250);
	}
	for(uint16_t i = 100; i <= 900; i+=100){
		addToBuffer(i,false,false);
		Delayms(250);
	}
	addToBuffer(888,false,false);
	Delayms(500);
	addToBuffer(1,true,false);
	Delayms(500);
	addToBuffer(1,false,true);
	Delayms(500);
	clearBuffer();
}

void ClearOnboardLEDS(void){
		  GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_3);
			GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
}

void set_dot2_always_on(void){
	dot2_always_on = true;
}

void Configure_BiColor_LED(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
}
void ControlBiColorLED(int _LED, bool state){
	if(state) GPIO_SetBits(LED_GPIO, _LED);
	if(!state) GPIO_ResetBits(LED_GPIO, _LED);
}