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


void init_usart(void){

    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    // Initialize pins as alternating function
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    //Enable clock for usart1

    /**
     * Enable clock for USART1 peripheral
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


    USART_InitStruct.USART_BaudRate = USART_BAUD_RATE;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);

    /**
     * Enable RX interrupt
     */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /**
     * Set Channel to USART1
     * Set Channel Cmd to enable. That will enable USART1 channel in NVIC
     * Set Both priorities to 0. This means high priority
     *
     * Initialize NVIC
     */
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);

}

void send_data(unsigned char tx_data[DATA_PACKET_LEN]) {
    // Older implementation
    unsigned char i;

    for (i = 0; i < DATA_PACKET_LEN; i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, tx_data[i]);
        //memset(&rx_data[0], 0, sizeof(rx_data)); // Clear the buffer once data sent out

    }

}

void usart_put_data_on_lcd(unsigned char* input){
    unsigned char output[14];
    sprintf(output, "%02X%02X%02X%02X%02X%02X%02X", input[0], input[1], input[2], input[3], input[4], input[5], input[6]);

    TM_ILI9341_Puts(1, 20, output, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

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

void USART1_IRQHandler(void) {


    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        if ((USART1->SR & (USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE | USART_FLAG_ORE)) == 0) {
            rx_buffer[rx_wr_index++] = (uint8_t)(USART_ReceiveData(USART1) & 0xFF);
            if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index = 0;
            if (++rx_counter == RX_BUFFER_SIZE) {
                rx_counter = 0;
                rx_buffer_overflow = 1;
            }
        }
        else USART_ReceiveData(USART1);//вообще здесь нужен обработчик ошибок, а мы просто пропускаем битый байт
    }

    if (USART_GetITStatus(USART1, USART_IT_ORE_RX) == SET) //прерывание по переполнению буфера
    {
        USART_ReceiveData(
                USART1); //в идеале пишем здесь обработчик переполнения буфера, но мы просто сбрасываем этот флаг прерывания чтением из регистра данных.
    }

    if (USART_GetITStatus(USART1, USART_IT_TXE) == SET) {
        if (tx_counter) {
            --tx_counter;
            USART_SendData(USART1, tx_buffer[tx_rd_index++]);
            if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index = 0;
        }
        else {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }


}


unsigned char get_char(void) { // Data recive
    uint8_t data;
    data = rx_buffer[rx_rd_index++]; //Getting data from the buffer
    if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index = 0; //cycling through buffer
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // disabling interrupt
    --rx_counter;                                                                        // so it won't interfere change variable
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // enebling it back again
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
        while (rx_counter == 0); // Wait if there's no data

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
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        if (tx_counter || (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)) {
            tx_buffer[tx_wr_index++] = c;
            if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index = 0;
            ++tx_counter;
            USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
        }
        else
            USART_SendData(USART1, c);
    }
}


void put_str(unsigned char *s) {
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

void usart_convert_outgoing_packet (unsigned char* packet, outgoing_packet_t outgoing_packet){ //, bool crc8) {
    packet[0] = outgoing_packet.slave_start_byte;
    packet[1] = outgoing_packet.slave_address;
    packet[2] = outgoing_packet.instruction;
	//if (crc8){
    packet[3] = outgoing_packet.crc8;
    packet[4] = outgoing_packet.stop_byte;
    packet[5] = '\0';
	//} else {
		packet[3] = outgoing_packet.stop_byte;
    packet[4] = '\0';
	//}
	
}




