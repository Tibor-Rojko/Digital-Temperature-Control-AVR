/***********************************************************************************************
 * UART library for HC05 bluetotth module                                        
 * BlueTooth.c 
 * 2019, Tibor Rojko
 *
 * pinout:
 * HC05 <--> MCU
 *  RX	     TX	(PD1)
 *  TX 	     RX (PD0)
 **********************************************************************************************/

#include <avr/io.h>
#define F_CPU 16000000
#define BAUD 9600
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "BlueTooth.h"

void initHC05(void)						//requires BAUD 
{					                                
	UBRR0H = UBRRH_VALUE;					// defined in setbaud.h */
	UBRR0L = UBRRL_VALUE;
	
	/* Enable USART transmitter/receiver & RX interrupt */ 
	UCSR0B = (1 << RXCIE0) |(1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);			//8 data bits, 1 stop bit
}

static void transmitByte(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));

	/* Put data into buffer, sends the data */
	UDR0 = data;
}

static unsigned char receiveByte(void)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}

void printString(char myString[])
{
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}

void readString(char myString[], uint8_t maxLength)
{
	char response;
	uint8_t i;
	i = 0;
	while (i <= (maxLength - 1)) {                  //prevent over-runs
		response = receiveByte();
		//transmitByte(response);               //echoing
		if (response == '\r') {                 //enter (\r) marks the end
			break;
		}
		else {
			myString[i] = response;         //add in a letter
			i++;
		}
	}
	myString[i] = 0;				//terminal NULL character
}

ISR(USART_RX_vect)
{
	/* read date from serial */
	char rx_buffer[2];								
	uint8_t rx_leght = 2;
	readString(rx_buffer, rx_leght);
	uint8_t rx_number = atoi(rx_buffer);

	if(rx_number >= 18 && rx_number <= 38)
	{ 
		threshold_temp = rx_number;
	
		checkState();				//check state of system
		lockLCD = 0;				//unlock LCD
		update_display();			//write changes to LCD
	}
}

