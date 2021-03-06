/*--------------------------------------------------------------------------------------------------------------------------------------------------------
 * Project: 		Post 4 LCD 16x2
 * Hardware:		Arduino UNO
 * Micro:			ATMEGA328P
 * IDE:			Atmel Studio 6.2
 *
 * Name:    		main.c
 * Purpose: 		LCD16x2 Example
 * Date:			01-10-2015
 * Author:		Marcel van der Ven
 *
 * Hardware setup:		PORTD	:	8-bit databus (pins 7 till 14 of LCD)
 *					PORTB0	:	Register Select (pin 4 of LCD)	
 *					PORTB1	:	Read/Write (pin 5 of LCD)
 *					PORTB2	:	Enable (pin 6 of LCD)
 * Note(s):
 *--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/************************************************************************/
/* Defines				                                                                  */
/************************************************************************/
#define F_CPU	16000000UL

/************************************************************************/
/* Includes				                                                                  */
/************************************************************************/
#include <avr/io.h>
#include "util/delay.h"
#include "lcd16x2.h"
#include "common.h"

/***************************************************************************
*  Function:		Setup()
*  Description:		Setup PORTD, this function need to be called at the beginning of main.
*  Receives:		Nothing
*  Returns:		Nothing
***************************************************************************/
void Setup()
{
	/* Setup Port B */
	/* Pins 0,1,2 as outputs */
	DDRB = 0b00000111;
	
	/* Setup PORTD */
	/* Set all pins as output for now */
	DDRD = 0b11111111;
}

/***************************************************************************
*  Function:		Main(void)
*  Description:		Main function of the program.
*  Receives:		Nothing
*  Returns:		Optionally the exit code.
***************************************************************************/
int main(void)
{
	/* Setup and initialization */
	Setup();
	InitializeLcd(
		&PORTD,
		&PIND,
		&DDRD,
		&PORTB,
		&PINB,
		PORTB0,
		PORTB1,
		PORTB2);
	
	/* LCD Startup delay */
	_delay_ms(20);
	
	/* Startup routine, mandatory sequence with delays (see datasheet) */
	FunctionSet(EIGHT_BIT, TWO_LINES, FONT5x8);
	
	/* Setup display */
	DisplayOnOffControl(TRUE, TRUE, TRUE);
	SetEntryMode(INCREMENT, FALSE);
	ClearDisplay();
	ReturnHome();

	/* String initialization */
	char str1[] = {"This is a test 1"};
	char str2[] = {"This is a test 2"};
		
	/* Test writing string first line */
	WriteNewLine(str1, LINE1);
	_delay_ms(1000);
	WriteNewLine(str2, LINE2);
	_delay_ms(1000);
	
	/* Clear display for new text */
	ClearDisplay();	
	DisplayOnOffControl(TRUE, FALSE, FALSE);
	
	/* String initialization */
	char str3[] = {"Temp: 25 deg."};
	char str4[] = {"35 deg."};
	char str5[] = {"45 deg."};
	char str6[] = {"5 deg."};	
		
	/* Write temperature text */	
	WriteNewLine(str3, LINE1);
	_delay_ms(1000);
	
	/* Replace only the temperature-text with a new value */
	WriteToPosition(str4, LINE1, 6, 7);
	_delay_ms(1000);
	WriteToPosition(str5, LINE1, 6, 7);
	_delay_ms(1000);
	WriteToPosition(str6, LINE1, 6, 7);
	_delay_ms(1000);
	
	
	while(1)
	{
	}
}