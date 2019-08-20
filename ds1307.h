/***********************************************************************************************
 * Library for DS1307 Real Time Clock module                                       
 * ds1307.h 
 * 2019, Tibor Rojko
 *
 * pinout:
 * AtMega328 <-->  DS1307
 *  PC5(SCL)		SCL
 *  PC4(SDA)		SDA
 **********************************************************************************************/

#ifndef DS1307_H_
#define DS1307_H_

#include <avr/io.h>

//define the struct, for storing date & time values
typedef struct 
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t weekDay;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} rtc_t;

#define DS1307_ADDR_W	0xD0			//0b11010000
#define DS1307_ADDR_R	0xD1			//0b11010001

void    rtc_Init(void);					//Initialize the ds1307 module
void    rtc_SetDateTime(rtc_t *rtc);	//Setting the date and time of RTC module
void    rtc_DateTime(rtc_t *rtc);		//Reading date & time

#endif 