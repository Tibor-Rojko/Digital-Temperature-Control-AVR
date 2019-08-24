/***********************************************************************************************
 * Library for PCF8574                                        
 * lcd.h 
 * 2019, Tibor Rojko
 *
 * pinout:
 * PCF8574 <--> LCD Display
 *	 P0				  D4
 *   P1				  D5
 *   P2				  D6
 *   P3  			  D7
 *   P4  			  RS
 *   P5  			  RW
 *   P6  			  EN
 *   P7			    BackLight
 **********************************************************************************************/

#ifndef LCD_H
#define LCD_H

//#define F_CPU 16000000

#define PCF8574_ADDR	0x40				// Address of PCF8574

#define LCD_CLEAR		0x00				  //display clear command
#define LCD_DISPLAY_ON	0x0C			//Display on command		
#define LCD_4BIT		0x28				  // 4-bit interface

#define RS	0x10
#define RW  0x20
#define EN  0x40

void lcdInit(void);									                //Lcd init... 
void lcdClear(void);								                //Lcd clear display
void lcdWriteChar(uint8_t byte, uint8_t dataFlag);	//Print char	
void lcdPrint(const char str[]);					          //Print string
void lcdCmd(uint8_t cmd);							              //Sending command to LCD
void lcdSetCursor(uint8_t row, uint8_t col);		    //Setting the cursor

#endif
