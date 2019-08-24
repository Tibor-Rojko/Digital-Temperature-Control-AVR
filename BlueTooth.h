/***********************************************************************************************
 * UART library for HC05 bluetotth module                                        
 * BlueTooth.h 
 * 2019, Tibor Rojko
 *
 * pinout:
 * HC05 <--> MCU
 *	RX		 TX	(PD1)
 *  TX 		 RX (PD0)
 **********************************************************************************************/


#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

//externed from main.c
extern volatile uint8_t threshold_temp;					
extern volatile uint8_t lockLCD;						

void initHC05(void);
void printString(char myString[]);						//print string to serial
void readString(char myString[], uint8_t maxLength);	//read string from serial
ISR(USART_RX_vect);

#endif /* BLUETOOTH_H_ */