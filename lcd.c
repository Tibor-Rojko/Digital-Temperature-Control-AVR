/***********************************************************************************************
 * Library for PCF8574                                        
 * lcd.c 
 * 2019, Tibor Rojko
 *
 * pinout:
 * PCF8574 <--> LCD Display
 *   P0		D4
 *   P1		D5
 *   P2		D6
 *   P3  	D7
 *   P4  	RS
 *   P5  	RW
 *   P6  	EN
 *   P7		BackLight
 **********************************************************************************************/


#include <avr/io.h>
#include "lcd.h"
#include "twimaster.h"
#include <util/delay.h>

void lcdWriteChar(uint8_t byte, uint8_t dataFlag)
{
	uint8_t Lower_Nibble;
	uint8_t Upper_Nibble;
	Lower_Nibble = byte & 0x0F;
	Upper_Nibble = ((byte & 0xF0) >> 4);

    twiStart();							//send TWI start condition
    twiSend(PCF8574_ADDR);					//send PCF8574 address to the i2c bus
	
	//output high nibble
	if(dataFlag) Upper_Nibble |= RS;			//RS=1, RS enable
	twiSend(Upper_Nibble | EN);				//EN=1, EN enable
	twiSend(Upper_Nibble);					//EN=0, RS=1 EN Disable
	
	//output low nibble
	if(dataFlag) Lower_Nibble |= RS;			//RS=1, RS enable
	twiSend(Lower_Nibble | EN);				//EN=1, EN enable 
	twiSend(Lower_Nibble);					//EN=0, RS=1 EN Disable

    twiStop();							//send TWI stop condition
    _delay_us(40);                              
}

void lcdCmd(uint8_t cmd)
{ 
    lcdWriteChar(cmd, 0);					//dataFlag to zero
}

void lcdClear(void)
{
    lcdCmd(1<<LCD_CLEAR);
    _delay_us(1500);
}

void lcdPrint(const char str[])
{
	uint8_t i = 0;
    while (str[i] != '\0')
	{
        lcdWriteChar(str[i], 1);
		i++;
	}
}

void lcdInit(void)
{
    twiInit();

    _delay_ms(16);               

    twiStart();
    twiSend(PCF8574_ADDR);

    twiSend(0x03);
    twiSend(EN | 0x03);
    twiSend(0x03);
    _delay_us(4992);
    twiSend(EN | 0x03);
    twiSend(0x03);
    _delay_us(64);
    twiSend(EN | 0x03);
    twiSend(0x03);
    _delay_us(64);
    twiSend(0x02);					
    twiSend(EN | 0x02);
    twiSend(0x02);

    twiStop();

    lcdCmd(LCD_4BIT);	
    lcdCmd(LCD_DISPLAY_ON);   
    lcdClear();
}

void lcdSetCursor(uint8_t row, uint8_t col)
{
		uint8_t cmd = 0;

		if(row == 1)
		{
			cmd = 0x80 + col - 1;
		}
		else if(row == 2)
		{
			cmd = 0xC0 + col - 1;
		}

		lcdCmd(cmd);
}
