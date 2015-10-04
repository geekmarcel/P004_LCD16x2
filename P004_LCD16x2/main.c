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

/************************************************************************/
/* Defines				                                                                  */
/************************************************************************/
#define	BYTE	unsigned char
#define	BOOL	unsigned char


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
	InitializeLcd(PORTD, PORTB0)
    while(1)
    {
        //TODO:: Please write your application code 
    }
}