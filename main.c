/*
 * Digital Temperature Control - Room Thermostat - main.c
 * Author : Tibor Rojko
 */ 

#include <avr/io.h>
#define F_CPU 16000000	//CPU frequency is 16MHz
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "ds1307.h"			//include RTC library - ds1307 - real time clock
#include "DS18B20.h"		//Temperature sensor header file
#include "lcd.h"			//LCD over PCF8574 header file

#define BUTTON_UP		PIND2	//Button on PORTD 2nd pin
#define BUTTON_DOWN		PIND3	//Button on PORTD 3rd pin
#define CONTROL_LINE	PIND4	//control line to climate/heater
#define CONTROL_PORT	PORTD	//port of control line

//Initialize the pin change interrupt
void initPinChangeInt(void);

//update display
void update_display(void);

//check state
void checkState(void);

//Pin change Interrupt Service Routine
ISR(PCINT2_vect);

uint8_t volatile threshold_temp = 25;				//threshold variable, init with 25 
uint8_t volatile lockLCD = 0;						//LCD helper
char volatile buff[17];								//buffer for sprintf
uint8_t temp = 25;									//variable for actual temperature
uint8_t state = 1;									//state of heating
uint8_t t = 0;										//temporary variable
uint8_t m = 0;										//temporary variable
rtc_t DateTime;										//struct for RTC

int main(void)
{
	//init LCD
	lcdInit();
	lcdSetCursor(1,3);
	lcdPrint("INITIALIZING");

	//pin conf.
	DDRD |= (1 << CONTROL_LINE);				//set the control line pin as output

	//enable pin change interrupt
	initPinChangeInt();

	//initRTC
	rtc_Init();

    while (1) 
    {	
		//read temperature
		t = temp;
		temp = ds18b20_GetTemp();
		if(t != temp)
		{
			lockLCD = 0;				//have any change in temperature
		}

		//control the digital climate
		checkState();

		//reading date&time
		m = DateTime.min;
		rtc_DateTime(&DateTime);
		if(m != DateTime.min)
		{
			lockLCD = 0;				//have any change in real time minute
		}

		//LCD display
		update_display();
    }
	
	return 0; 
}

void initPinChangeInt()
{
	//Enable the specific interrupt in the control register - PCICR – Pin Change Interrupt Control Register
	PCICR |= (1 << PCIE2);	//PORTD

	//Setting the pins in the pin mask register - for PORTD - PCMSK0
	PCMSK2 |= ((1 << BUTTON_UP) | (1 << BUTTON_DOWN));	//PIND2 Button UP, PIND3 Button Down

	//Enable global interrupt
	sei();
	
}

ISR(PCINT2_vect)
{
	//handle the BUTTON_UP pin
	
	while(!(PIND & (1 << BUTTON_UP)))		//when the button was pushed, or holding down, the counter variable is set
	{		
		threshold_temp++;	//increase
		_delay_ms(250);

		//check the highest valid value
		if(threshold_temp > 38)
		{
			threshold_temp = 18;
		}

		checkState();		//check state of system
		lockLCD = 0;		//unlock LCD
		update_display();
	}

	

	//handle the BUTTON_DOWN pin
	while(!(PIND & (1 << BUTTON_DOWN)))		//when the button was pushed, or holding down, the counter variable is set
	{
		threshold_temp--;	//decrease
		_delay_ms(250);

		//check the lowest valid value
		if(threshold_temp < 18)
		{
			threshold_temp = 38;
		}

		checkState();		//check state of system
		lockLCD = 0;		//unlock LCD
		update_display();
	}
}

void update_display()
{
	if(lockLCD == 0)
	{
		lcdClear();
		if(state == 1)		//ON STATE
		{
			sprintf(buff,"%d(ON) TEMP:%d'C", threshold_temp, temp);
		}
		else				//OFF STATE
		{
			sprintf(buff,"%d     TEMP:%d'C", threshold_temp, temp);
		}
	
		lcdPrint(buff);
		lcdSetCursor(2,1);
		sprintf(buff,"20%d-%d-%d %d:%d",DateTime.year,DateTime.month,DateTime.date,DateTime.hour,DateTime.min);
		lcdPrint(buff);
		lockLCD = 1;
	}
}

void checkState()
{
	if(temp >= threshold_temp)
	{
		state = 0;
		CONTROL_PORT &= ~(1 << CONTROL_LINE);	//STATE TO OFF
	}
	else
	{
		state = 1;
		CONTROL_PORT |= (1 << CONTROL_LINE);	//STATE TO ON
	}
}
