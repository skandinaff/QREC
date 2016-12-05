#include "usart.h"



//USART variables

char str[15];
char buffer[15];
uint8_t msg_size;

unsigned char sample[] = {0xC1, 0x10, 0x01, 0xFF, 0xC0};
unsigned char sample2[] = {0xC1, 0x10, 0x01, 0xAA, 0xAA, 0xFF, 0xC0};
unsigned char rx_data[DATA_PACKET_LEN];
unsigned char rx_buff[DATA_PACKET_LEN];

// USART Receiver buffer
#define RX_BUFFER_SIZE 350
volatile uint8_t  rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_wr_index=0,rx_rd_index=0;
volatile uint16_t rx_counter=0;
volatile uint8_t rx_buffer_overflow=0;

// USART Transmitter buffer
#define TX_BUFFER_SIZE 350
volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
volatile uint16_t tx_wr_index=0,tx_rd_index=0;
volatile uint16_t tx_counter=0;

char data_for_crc8[3];
char incoming_crc8;
bool break_flag = false;
bool USART_ACTIVE = false;

void init_usart(void){

    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);


    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

    // Initialize pins as alternating function
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    //Enable clock for USART3

    /**
     * Enable clock for USART3 peripheral
     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);


    USART_InitStruct.USART_BaudRate = USART_BAUD_RATE;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStruct);

    USART_Cmd(USART3, ENABLE);

    /**
     * Enable RX interrupt
     */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    /**
     * Set Channel to USART3
     * Set Channel Cmd to enable. That will enable USART3 channel in NVIC
     * Set Both priorities to 0. This means high priority
     *
     * Initialize NVIC
     */
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
		


}
void usart_put_data_on_lcd(unsigned char* input){
		//That's completlely irrelevanty since we don't have a screen in final version
    unsigned char output[14];
    sprintf(output, "%02X%02X%02X%02X%02X%02X%02X", input[0], input[1], input[2], input[3], input[4], input[5], input[6]);

    //TM_ILI9341_Puts(1, 20, output, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

    /* This should've been a proper, solution, but it doens't work for some reason
    int len = strlen(input);


    char* output = malloc(len * 2 * sizeof(char));

    char tmp[2]; // 1 char contains 2 HEX encoded "characters"
    int output_index = 0;
    for (int i = 0; i < len; i++) {
        sprintf(tmp, "%X", input[i]); // convert 1 char to 2 HEX encoded "characters"

        output[2*output_index] = tmp[0];
        output[2*output_index+1] = tmp[1];

        output_index++;
    }

    TM_ILI9341_Puts(1, 20, output, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
    */
}

void USART3_IRQHandler(void) {
	
	
		set_usart_active(true);
	
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
        if ((USART3->SR & (USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE | USART_FLAG_ORE)) == 0) {
            rx_buffer[rx_wr_index++] = (uint8_t)(USART_ReceiveData(USART3) & 0xFF);
            if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index = 0;
            if (++rx_counter == RX_BUFFER_SIZE) {
                rx_counter = 0;
                rx_buffer_overflow = 1;
            }
        }
        else USART_ReceiveData(USART3);//вообще здесь нужен обработчик ошибок, а мы просто пропускаем битый байт
    }

    if (USART_GetITStatus(USART3, USART_IT_ORE_RX) == SET) //прерывание по переполнению буфера
    {
        USART_ReceiveData(USART3); //в идеале пишем здесь обработчик переполнения буфера, но мы просто сбрасываем этот флаг прерывания чтением из регистра данных.
    }

    if (USART_GetITStatus(USART3, USART_IT_TXE) == SET) {
        if (tx_counter) {
            --tx_counter;
            USART_SendData(USART3, tx_buffer[tx_rd_index++]);
            if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index = 0;
        }
        else {
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }
    }
		if(USART_GetITStatus(USART3, USART_IT_TC) == SET){
			GPIO_ToggleBits(RS485_GPIO, RS485_EN_PIN); // RS485 RX
			USART_ITConfig(USART3, USART_IT_TC, DISABLE);
		}

		set_usart_active(false);	

}


unsigned char get_char(void) { 															// Data recive
    uint8_t data;
    data = rx_buffer[rx_rd_index++]; 												//Getting data from the buffer
    if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index = 0; 		//cycling through buffer
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); 				// disabling interrupt
    --rx_counter;                                   				// so it won't interfere change variable
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  				// enebling it back again
    return data;
}


bool usart_has_data() {

    return rx_counter != 0;

}


void usart_get_data_packet(unsigned char* packet) {
    if (!usart_has_data()) {
        *packet = '\0';
        return;
    }

    uint8_t packet_byte;
    do {
        while (rx_counter == 0); 														// Wait if there's no data

        packet_byte = get_char();

        *packet++ = packet_byte;
    } while (packet_byte != STOP_BYTE);

    *packet = '\0';
}


bool usart_break_required(void){
	return false;
	
	/*
	unsigned char packet[128];
	
	incoming_packet_t incoming_packet;
	
	if (usart_has_data()) {
		usart_get_data_packet(packet);
		incoming_packet = usart_packet_parser(packet);
			
		if (usart_validate_crc8(incoming_packet) && usart_packet_is_addressed_to_me(incoming_packet)){
			if (incoming_packet.instruction == INSTR_MASTER_SET_IDLE) {
				
				
				return true;
			}
		}
	}
	
	return false;
	*/
}


void put_char(uint8_t c) {
    if (c) {
        while (tx_counter == TX_BUFFER_SIZE);
        USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        if (tx_counter || (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)) {
            tx_buffer[tx_wr_index++] = c;
            if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index = 0;
            ++tx_counter;
            USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
						USART_ITConfig(USART3, USART_IT_TC, ENABLE);
        }
        else
            USART_SendData(USART3, c);
    }
}


void put_str(unsigned char *s) {
		GPIO_ToggleBits(RS485_GPIO, RS485_EN_PIN); // RS485 TX
    while (*s != 0)
        put_char(*s++);
}


bool usart_packet_is_addressed_to_me(incoming_packet_t incoming_packet) {
    return incoming_packet.slave_address == QUEST_ID;
}


incoming_packet_t usart_packet_parser(unsigned char* packet) {
    incoming_packet_t incoming_packet;
    incoming_packet.packet_length = strlen(packet);

    incoming_packet.master_stat_byte = packet[0];
    incoming_packet.slave_address = packet[1];
    incoming_packet.instruction = packet[2];

    if (incoming_packet.packet_length == 7) {
        incoming_packet.player_count = packet[3];
        incoming_packet.hint_count = packet[4];
    }

    incoming_packet.stop_byte = packet[incoming_packet.packet_length == 7 ? 6 : 4];
    incoming_packet.crc8 = packet[incoming_packet.packet_length == 7 ? 5 : 3];

		return incoming_packet;

}


outgoing_packet_t usart_assemble_response(unsigned char instruction) {
  
		char data_for_crc8[] = {QUEST_ID, instruction, '\0'};
	
		outgoing_packet_t outgoing_packet;
    outgoing_packet.slave_start_byte = SLAVE_START_BYTE;
    outgoing_packet.slave_address = QUEST_ID;
    outgoing_packet.instruction = instruction;
    outgoing_packet.crc8 = usart_crc8(CRC_INIT_VAL, data_for_crc8); 
		
    outgoing_packet.stop_byte = STOP_BYTE;

    if (outgoing_packet.crc8 == STOP_BYTE ||
            outgoing_packet.crc8 == MASTER_START_BYTE ||
            outgoing_packet.crc8 == SLAVE_START_BYTE ||
            outgoing_packet.crc8 == RESTRICTED_BYTE) {
        outgoing_packet.crc8 ^= RESTRICTED_BYTE;
    }

		
		
    return outgoing_packet;
}

bool usart_validate_crc8(incoming_packet_t incoming_packet){

	char data_for_crc8[] = {incoming_packet.slave_address, incoming_packet.instruction, '\0'};
	
	char incoming_crc8 = usart_crc8(CRC_INIT_VAL, data_for_crc8);
	
	if(incoming_packet.crc8 == incoming_crc8) {
		return true;
	}
	else{
		return false;
	}
	
}

uint8_t usart_crc8(uint8_t init, uint8_t *packet){
	uint8_t i;
	uint8_t crc = init;

	uint8_t len = strlen(packet);
	
	while(len--){
			
		crc ^= *packet++;
		for(i = 0; i < 8; i++){
			crc = crc & 0x80 ? (crc << 1) ^ CRC_POLYNOM : crc <<1;
		}
		
	}
	return crc;
	
}

void usart_convert_outgoing_packet (unsigned char* packet, outgoing_packet_t outgoing_packet){ 
    packet[0] = outgoing_packet.slave_start_byte;
    packet[1] = outgoing_packet.slave_address;
    packet[2] = outgoing_packet.instruction;
    packet[3] = outgoing_packet.crc8;
    packet[4] = outgoing_packet.stop_byte;
    packet[5] = '\0';
}


void check_usart_while_playing(){
		incoming_packet_t incoming_packet;
	
		set_usart_active(true);	
	
		unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	
		if (usart_has_data()) {
			
			usart_get_data_packet(packet);
			incoming_packet = usart_packet_parser(packet);
			if (usart_validate_crc8(incoming_packet) && usart_packet_is_addressed_to_me(incoming_packet)){
			//TM_ILI9341_Puts(1, 220, "We recevied some data", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			//BlinkOnboardLED(2);
				switch (incoming_packet.instruction) {
					case INSTR_MASTER_TEST:
						//SendInstruction(INSTR_SLAVE_NOT_READY); 
						break;

					case INSTR_MASTER_STATUS_REQ:				
						if (get_task_counter() == TASK_COUNT) {
							SendInstruction(INSTR_SLAVE_COMPLETED);
						} else {
							SendInstruction(INSTR_SLAVE_NOT_COMLETED);
							//put_char('w');
							//put_char(get_task_counter()+1); // +1 since task counter starts with 0
						}
						break;
					case INSTR_MASTER_SET_IDLE:
						/*setTIM5_count(0);
					  setSecondsCount(0); */
						TIM_Cmd(TIM2, DISABLE);
						TIM_Cmd(TIM5, DISABLE);
						GPIO_SetBits(ONBOARD_LED_GPIO, ONBOARD_LED_2);
						GPIO_ResetBits(LED_GPIO, STATE_LED);
					  set_xLED(0);
						setClaps(0);
						set_break_flag(true);
						set_first_start(false);
						set_task_counter(FIRST_TASK);
						//clearBuffer();
						return;
					case CINSTR_GOTO_END:
						set_task_counter(get_task_counter() + 1); // Skips a task
						if(get_task_counter() == TASK_COUNT ) set_task_counter(TASK_COUNT);
						Control_12V_LEDs();
						PerformQuest();
						break;
					case CINSTR_RESTART_TASK:
						setSecondsCount(0);
						setClaps(0);
						setSilenceThresh(SILENCE_AMPLITUDE);
						resetSilenceThresh();
						setTIM5_count(0);
						set_task_counter(get_task_counter());
						PerformQuest();
						break;
					case TEST_DISP:
						Test_7Seg();
						break;
					case TASK_REQUEST:
						//put_char(get_task_counter()+1);
					  SendInstruction(get_task_counter()+1);
						break;
					case SIL_THR_REQUEST:
						SendInstruction(getSilenceThresh());
						//put_char(getSilenceThresh());
						//put_char('w');
						break;
					case SYS_RESET:
						NVIC_SystemReset();
						break;
					case HM_CUPS:
						SendInstruction(DetectCups());
						break;
				}	
			}
		}
		
		free(packet);
		
		set_usart_active(false);	
}


uint8_t SendInstruction(unsigned char instruction){
	set_usart_active(true);
	unsigned char* packet = malloc((OUTGOING_PACKET_LENGTH + 1) * sizeof(char));
	outgoing_packet_t outgoing_packet = usart_assemble_response(instruction);
	usart_convert_outgoing_packet(packet, outgoing_packet);
	put_str(packet);
	free(packet);
	set_usart_active(false);
	return 1;
}

void set_break_flag(bool bf) {
    break_flag = bf;
}


bool get_break_flag(void) {
	
    return break_flag;
}

void set_usart_active(bool ua) {
    USART_ACTIVE = ua;
}

bool get_usart_active(void) {
	
    return USART_ACTIVE;
}

