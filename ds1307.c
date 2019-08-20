/***********************************************************************************************
 * Library for DS1307 Real Time Clock module                                       
 * ds1307.c 
 * 2019, Tibor Rojko
 *
 * pinout:
 * AtMega328 <-->  DS1307
 *  PC5(SCL)		SCL
 *  PC4(SDA)		SDA
 **********************************************************************************************/

#include  "ds1307.h"
#include  "twimaster.h"

//Convert Decimal (DEC) to Binary Coded Decimal (BCD)
static uint8_t dec2bcd(char num)
{
	return ((num/10 * 16) + (num % 10));
}

//Convert Binary Coded Decimal (BCD) to Decimal (DEC)
static uint8_t bcd2dec(char num)
{
	return ((num/16 * 10) + (num % 16));
}

void rtc_Init(void)
{
    twiInit();								// Initialize the I2c bus.
    twiStart();								// Start I2C communication
    twiSend(DS1307_ADDR_W);					// Connect to ds1307 by sending his Address 
    twiSend(0x0E);							// Select the ds1307 ControlRegister 
    twiSend(0x20);							// Write 0x00 to Control register to disable SQW-Out
	twiStop();								// Stop I2C communication after initializing ds3231
}

void rtc_SetDateTime(rtc_t *rtc)
{
    twiStart();								// Start I2C communication
    twiSend(DS1307_ADDR_W);					// connect to ds1307 by sending its ID on I2c Bus
    twiSend(0x00);							// Request sec RAM address at 0x00

    twiSend(dec2bcd(rtc->sec));				// Write sec from RAM address 0x00
    twiSend(dec2bcd(rtc->min));				// Write min from RAM address 0x01
    twiSend(dec2bcd(rtc->hour));			// Write hour from RAM address 0x01
    twiSend(dec2bcd(rtc->weekDay));			// Write weekDay on RAM address 0x03
    twiSend(dec2bcd(rtc->date));			// Write date on RAM address 0x04
    twiSend(dec2bcd(rtc->month));           // Write month on RAM address 0x05
    twiSend(dec2bcd(rtc->year));			// Write year on RAM address 0x06

    twiStop();                              // Stop I2C communication
}

void rtc_DateTime(rtc_t *rtc)
{
    twiStart();									//Sending twi start condition
    twiSend(DS1307_ADDR_W);						//connect to ds1307 by sending its ID trough TWI bus
    twiSend(0x00);								//Set the register pointer to seconds, 0x00
    twiStop();									//Sending TWI stop cond.

    twiStart();									//Sending twi start condition
    twiSend(DS1307_ADDR_R);						//Connect to ds1307(Read mode) by sending its ID

    rtc->sec = bcd2dec(twiReadWithAck());		//Read second reg. (0x00) and send back ACK
    rtc->min = bcd2dec(twiReadWithAck());		//Read minute reg. (0x01) and return ACK
    rtc->hour= bcd2dec(twiReadWithAck());		//Read hour (0x02) and return Negative ACK
	
    rtc->weekDay = bcd2dec(twiReadWithAck());   //Read Day Of Week (0x03, 1 is Sun., 7 is Sat.) and return ACK
    rtc->date = bcd2dec(twiReadWithAck());		//Read date (0x04) and return Positive ACK
    rtc->month = bcd2dec(twiReadWithAck());		//read month (0x05) and return Positive ACK
    rtc->year = bcd2dec(twiReadNoAck());        //read year (0x06) and return Negative/No ACK. Stored in XX format, 2019 > 19
	twiReadNoAck();
    twiStop();									//Stop I2C communication
}
