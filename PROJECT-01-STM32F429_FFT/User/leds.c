#include "leds.h"




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