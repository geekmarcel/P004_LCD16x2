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
 * Hardware setup:	
 *
 * Note(s):
 *--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/************************************************************************/
/* Includes				                                                                  */
/************************************************************************/
#include <avr/io.h>
#include "util/delay.h"
#include "lcd16x2.h"
#include "common.h"

/************************************************************************/
/* Defines				                                                                  */
/************************************************************************/


/***************************************************************************
*  Function:		Setup(int number)
*  Description:		Setup PORTD and the timer, this function need to be called at
*				the beginning of main.
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
	Setup();
	InitializeLcd(
		&PORTD, 
		&DDRD,
		&PORTB,
		PORTB0 + 1,
		&PORTB,
		PORTB1 + 1,
		&PORTB,
		PORTB2 + 2);
	
	/* LCD Startup delay */
	_delay_ms(20);
	
	/* Startup routine, mandatory sequence with delays (see datasheet) */
	FunctionSet(EIGHT_BIT, TWO_LINES, FONT5x8);
	_delay_ms(5);
	FunctionSet(EIGHT_BIT, TWO_LINES, FONT5x8);
	_delay_us(100);
	
	/* Setup display */
	DisplayOnOffControl(TRUE, TRUE, TRUE);
	SetEntryMode(INCREMENT, FALSE);

	/* Test writing string first line */
	WriteNewLine("Dit is een test1", LINE1);
	_delay_ms(2000);
	WriteNewLine("Dit is een test2", LINE2);
	_delay_ms(2000);
	
	WriteNewLine("Test2 Line 1", LINE1);
	_delay_ms(2000);
	WriteNewLine("Test2 Line 2", LINE2);
	_delay_ms(2000);
	
	WriteNewLine("Temp: 25 deg.", LINE1);
	_delay_ms(500);
	WriteToPosition("35 deg.", LINE1, 6, 7);
	_delay_ms(500);
	WriteToPosition("45 deg.", LINE1, 6, 7);
	_delay_ms(500);
	WriteToPosition("5 deg.", LINE1, 6, 7);
	_delay_ms(500);
	WriteToPosition("-1 deg.", LINE1, 6, 7);
	_delay_ms(500);
	WriteToPosition("99 deg.", LINE1, 6, 7);
	
	
    while(1)
    {	
    }
}