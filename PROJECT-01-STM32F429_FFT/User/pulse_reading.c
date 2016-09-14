#include "pulse_reading.h"

volatile uint16_t BPM;
volatile uint16_t Signal;
volatile uint16_t IBI = 600;
volatile uint16_t Pulse = 0;
volatile uint16_t QS = 0;

volatile uint16_t rate[10];                    // array to hold last ten IBI values

volatile uint32_t lastBeatTime = 0;           // used to find IBI
volatile uint16_t P = 2048; //512;                     // used to find peak in pulse wave, seeded
volatile uint16_t T = 2048; //512;                     // used to find trough in pulse wave, seeded
volatile uint16_t thresh = 2048; //525;                // used to find instant moment of heart beat, seeded
volatile uint16_t amp = 100; //100;                   // used to hold amplitude of pulse waveform, seeded
volatile uint16_t firstBeat = 1;        // used to seed rate array so we startup with reasonable BPM
volatile uint16_t secondBeat = 1;      // used to seed rate array so we startup with reasonable BPM

char adc_result_str[15];
char BPM_result_str[15];
char thresh_restult_str[15];
char time_str[15];
char pulse_str[15];

uint16_t old_tim5_count = 0;


void ReadPulse(void) {
    //TM_ILI9341_DrawPixel(getTIM5_count(), 240 - thresh / 17, ILI9341_COLOR_RED);

    Signal = TM_ADC_Read(ADC3, ADC_Channel_11);              // read the Pulse Sensor from PC1
    //sampleCounter += 2; // 2 (ms)                         // keep track of the time in mS with this variable
    // We've assigned this variable incrementation to a timer iinterrupts
    uint16_t N = getSampleCounterIRQ() - lastBeatTime;       // monitor the time since the last beat to avoid noise

    //  find the peak and trough of the pulse wave
    if (Signal < thresh && N > (IBI / 5) * 3) {       // avoid dichrotic noise by waiting 3/5 of last IBI
        if (Signal < T) {                        // T is the trough
            T = Signal;                         // keep track of lowest point in pulse wave
        }
    }

    if (Signal > thresh && Signal > P) {          // thresh condition helps avoid noise
        P = Signal;                             // P is the peak
    }                                        // keep track of highest point in pulse wave

    //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
    // signal surges up in value every time there is a pulse
    if (N > 250) {                                   // avoid high frequency noise

        //Here I should add a condition that will check signal's amplitude, to avoiud measuremets that are just above 0

        if ((Signal > thresh) && (Pulse == 0) && (N > (IBI / 5) * 3) && /*((Signal - thresh) > 5) &&*/
            (Signal - thresh) < 700) { 
							/* Signal-thresh < 700 is to avoid huge spikes  
									Signal-thresh > 5 is to avoid little fluctutations, like ambient noise
							*/
            Pulse = 1;                               // set the Pulse flag when we think there is a pulse
							//TM_DISCO_LedOn(LED_RED);               // turn on pin 13 LED
							GPIO_ResetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4); // Red LED turns on when we have a beat
            IBI = getSampleCounterIRQ() - lastBeatTime;         // measure time between beats in mS
            lastBeatTime = getSampleCounterIRQ();;               // keep track of time for next pulse

            if (secondBeat == 1) {                        // if this is the second beat, if secondBeat == TRUE
                secondBeat = 0;                  // clear secondBeat flag
                for (uint16_t i = 0;
                     i <= 9; i++) {             // seed the running total to get a realisitic BPM at startup
                    rate[i] = IBI;
                }
            }

            if (firstBeat ==
                1) {                         // if it's the first time we found a beat, if firstBeat == TRUE
                firstBeat = 0;                   // clear firstBeat flag
                secondBeat = 1;                   // set the second beat flag

                return;                              // IBI value is unreliable so discard it
            }


            // keep a running total of the last 10 IBI values
            uint32_t runningTotal = 0;                  // clear the runningTotal variable

            for (uint16_t i = 0; i <= 8; i++) {                // shift data in the rate array
                rate[i] = rate[i + 1];                  // and drop the oldest IBI value
                runningTotal += rate[i];              // add up the 9 oldest IBI values
            }

            rate[9] = IBI;                          // add the latest IBI to the rate array
            runningTotal += rate[9];                // add the latest IBI to runningTotal
            runningTotal /= 10;                     // average the last 10 IBI values
            BPM = 60000 / runningTotal;               // how many beats can fit into a minute? that's BPM!
            QS = 1;                              // set Quantified Self flag
            // QS FLAG IS NOT CLEARED INSIDE THIS ISR
        }
    }

    if (Signal < thresh && Pulse == 1) {   // when the values are going down, the beat is over
        //TM_DISCO_LedOff(LED_RED);              // turn off pin 13 LED
					GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_4);
        Pulse = 0;                         // reset the Pulse flag so we can do it again

        amp = P - T;                           // get amplitude of the pulse wave
        thresh = amp / 2 + T;                    // set thresh at 50% of the amplitude
        P = thresh;                            // reset these for next time
        T = thresh;
    }

    if (N > 1000) {                           // if N milliseconds go by without a beat (Original value 2500, I mostly used 1000)
        thresh = 2058;                          // set thresh default
        P = 2048;                               // set P default
        T = 2048;                               // set T default
        lastBeatTime = getSampleCounterIRQ();;          // bring the lastBeatTime up to date
        firstBeat = 1;                      // set these to avoid noise
        secondBeat = 0;                    // when we get the heartbeat back
				/* These were added by me */
				Pulse = 0;    // Added here to reduce 
        BPM = 0;		// Add this line here, so when no beat detected display shows 0

    }


    {

        //sprintf(adc_result_str, "%4d: ", Signal);
        //TM_ILI9341_Puts(1, 5, "RAW ADC: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        if (getTIM5_count2() != old_tim5_count)
            //TM_ILI9341_Puts(100, 5, adc_result_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

        //sprintf(thresh_restult_str, "%4d: ", thresh);
        //TM_ILI9341_Puts(1, 25, "Thresh: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        //TM_ILI9341_Puts(100, 25, thresh_restult_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

        //sprintf(BPM_result_str, "%4d: ", BPM);
        //TM_ILI9341_Puts(1, 45, "BPM: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				
        if (Pulse == 1) {
            //TM_ILI9341_Puts(100, 45, BPM_result_str, &TM_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE);
					//put_char(BPM);
					//if(BPM < 100) clearBuffer(); // So we're clearing LED indicator
					addToBuffer(BPM);
					//Delayms(125);
					//clearBuffer();
        }
				
				//sprintf(time_str, "%4d: ", getSecondCount());
        //TM_ILI9341_Puts(160, 5, "Time: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				//TM_ILI9341_Puts(190, 5, time_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				
				//sprintf(pulse_str, "%4d: ", Pulse);
				
        //TM_ILI9341_Puts(160, 25, "Pulse: ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				//TM_ILI9341_Puts(190, 25, pulse_str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);


    }


    old_tim5_count = getTIM5_count2();

    if (getTIM5_count() >= 320) {
        setTIM5_count(1);
        //TM_ILI9341_Fill(ILI9341_COLOR_WHITE);

    }


    //TM_ILI9341_DrawPixel(getTIM5_count(), 240 - Signal / 17, 0x1234);

    if (BPM > TARGET_BPM) {
        TIM_Cmd(TIM2, ENABLE);
        if (getSecondCount() >= TARGET_TIME) {
            //TM_ILI9341_Puts(1, 65, "You have >120 BPM for >10 sec", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
            Delayms(2000);
            set_task_counter(get_task_counter() + 1);
						// After task is done, we're cleaning all the counters
            setSecondsCount(0);
						BPM = 0;
        }
    } else {
        TIM_Cmd(TIM2, DISABLE);
        //TM_ILI9341_Puts(1, 65, "                              ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
        setSecondsCount(0);
    }

    /*** This is for pulse readings ***/
    if (getQS() == 1)
        setQS(!getQS());                        // A Heartbeat Was Found, reset the Quantified Self flag for next time
    /**********************************/
}

uint16_t getQS(void) {
    return QS;
}

void setQS(uint16_t s) {
    QS = s;
}
