/********************************************************************************************************************
 * Digital Temperature Control - Room Thermostat with bluetooth HMI
 * 2019, Tibor Rojko
 ********************************************************************************************************************/

#include <avr/io.h>
#define F_CPU 16000000								//CPU frequency is 16MHz, external crystal
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "ds1307.h"									//include RTC library - ds1307 - real time clock
#include "DS18B20.h"								//Temperature sensor header file
#include "lcd.h"									//LCD over PCF8574 header file
#include "BlueTooth.h"								//USART for HC05 BlueTooth module	



/* Definitions & Variables */
#define BUTTON_UP		PIND2						//Button on PORTD 2nd pin for increase the threshold temperature
#define BUTTON_DOWN		PIND3						//Button on PORTD 3rd pin for decrease the threshold temperature
#define CONTROL_LINE	PIND4						//control line to climate/heater
#define CONTROL_PORT	PORTD						//port of control line

volatile uint8_t threshold_temp = 25;				//threshold variable, init with 25
volatile uint8_t lockLCD = 0;						//LCD helper
char volatile buff[17];								//buffer for sprintf
uint8_t temp = 25;									//variable for actual temperature
uint8_t state = 1;									//state of heating
uint8_t t = 0;										//temporary variable
uint8_t m = 0;										//temporary variable
rtc_t DateTime;										//struct for RTC

/* Functions */
void initPinChangeInt(void);						//Initialize the pin change interrupt
void update_display(void);							//update display
void checkState(void);								//check state
ISR(PCINT2_vect);									//Pin change Interrupt Service Routine
void increase_Ttemp(void);							//increase the threshold temp variable
void decrease_Ttemp(void);							//decrease the threshold temp variable

int main(void)
{
	/* init LCD display */
	lcdInit();
	lcdSetCursor(1,3);
	lcdPrint("INITIALIZING");
	lcdSetCursor(2,1);
	lcdPrint("DIGIT TEMP CTRLR");

	/* init USART for HC05 */
	initHC05();

	/* Control pin configuration */ 
	DDRD |= (1 << CONTROL_LINE);				//set the control line pin as output

	/* enable pin change interrupt */
	initPinChangeInt();

	/* initRTC */
	rtc_Init();

    while (1) 
    {	
		/* read temperature */
		t = temp;								//set t helper variable, for handle the temperature changing
		temp = ds18b20_GetTemp();				//get temp from DS18B20
		if(t != temp)							//if have any change in temperature		
		{
			lockLCD = 0;						//unlock display, write changes
		}

		/* control the digital climate */
		checkState();

		/* reading date&time from ds1307 */
		m = DateTime.min;						//set m helper variable, for handle the clock changing
		rtc_DateTime(&DateTime);				//read date and time from ds1307
		if(m != DateTime.min)					//have any change in real time minute
		{
			lockLCD = 0;						//unlock display, write changes
		}

		/* LCD display */
		update_display();						//execute, when the lockLCD is zero (unlock the display)
    }
	
	return 0; 
}

void initPinChangeInt()
{
	/* Enable the specific interrupt in the control register - PCICR – Pin Change Interrupt Control Register */
	PCICR |= (1 << PCIE2);								//PORTD

	/* Setting the pins in the pin mask register - for PORTD - PCMSK0 */
	PCMSK2 |= ((1 << BUTTON_UP) | (1 << BUTTON_DOWN));	//PIND2 Button UP, PIND3 Button Down
	
	sei();												//enable global interrupt
}

ISR(PCINT2_vect)
{
	/* handle the BUTTON_UP pin */
	while(!(PIND & (1 << BUTTON_UP)))		//when the button was pushed, or holding down, increase the threshold temp
	{		
		increase_Ttemp();
	}

	/* handle the BUTTON_DOWN pin */
	while(!(PIND & (1 << BUTTON_DOWN)))		//when the button was pushed, or holding down, the decrease the threshold temp
	{
		decrease_Ttemp();
	}
}

void update_display()
{
	if(lockLCD == 0)	//if LCD screen unlocked
	{
		lcdClear();														//clear display
		if(state == 1)	//ON STATE
		{
			sprintf(buff,"%d(ON) TEMP:%d'C", threshold_temp, temp);		
			printString("*LR255G0B0*\r\n");								//sending status led state to HMI
		}
		else			//OFF STATE
		{
			sprintf(buff,"%d     TEMP:%d'C", threshold_temp, temp);
			printString("*LR0G0B0*\r\n");								//sending status led state to HMI
		}
		lcdPrint(buff);
		
		/* Write date and time to the second line of display */
		lcdSetCursor(2,1);
		sprintf(buff,"20%d-%d-%d %d:%d",DateTime.year,DateTime.month,DateTime.date,DateTime.hour,DateTime.min);
		lcdPrint(buff);

		lockLCD = 1;													//lock the LCD screen

		sprintf(buff,"*A%d*\r\n",temp);									//sending the actual temp to HMI
		printString(buff);
		sprintf(buff,"*T%d*\r\n",temp);									//sending the actual temp to HMI
		printString(buff);
		sprintf(buff,"*U%d*\r\n",threshold_temp);						//sending user defined threshold temp to HMI
		printString(buff);
	}
}

void checkState()
{
	if(temp >= threshold_temp)					//if user defined threshold temp lower or equal the actual room temperature turn state to OFF
	{
		state = 0;
		CONTROL_PORT &= ~(1 << CONTROL_LINE);	//STATE TO OFF
	}
	else										//if the room temperature is lower the threshold value, turn state to ON
	{
		state = 1;
		CONTROL_PORT |= (1 << CONTROL_LINE);	//STATE TO ON
	}
}

void increase_Ttemp()
{
	threshold_temp++;							//increase
	_delay_ms(250);								//delay for the user can follow the change

	/* check the highest valid value */
	if(threshold_temp > 38)
	{
		threshold_temp = 18;
	}

	checkState();								//check state of system
	lockLCD = 0;								//unlock LCD
	update_display();							//update display screen
}

void decrease_Ttemp()
{
	threshold_temp--;							//decrease
	_delay_ms(250);

	/* check the lowest valid value */
	if(threshold_temp < 18)
	{
		threshold_temp = 38;
	}

	checkState();								//check state of system
	lockLCD = 0;								//unlock LCD
	update_display();							//update display screen
}