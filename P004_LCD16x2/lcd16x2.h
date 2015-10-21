/*--------------------------------------------------------------------------------------------------------------------------------------------------------
 * Project: 		LCD 16x2 Library
 * Hardware:		Arduino UNO
 * Micro:			ATMEGA328P
 * IDE:			Atmel Studio 6.2
 *
 * Name:    		lcd16x2.h
 * Purpose: 		LCD 16x2 Library Header
 * Date:			01-10-2015
 * Author:		Marcel van der Ven
 *
 * Hardware setup:	
 *
 * Note(s):
 *--------------------------------------------------------------------------------------------------------------------------------------------------------*/


#ifndef LCD16X2_H_
#define LCD16X2_H_


#include "common.h"

/************************************************************************/
/* Defines				                                                                  */
/************************************************************************/
#define LINE1			1
#define LINE2			2

/************************************************************************/
/* Type Definitions			                                                                  */
/************************************************************************/
typedef enum{LEFT, RIGHT} Direction;
typedef enum{INCREMENT, DECREMENT} CursorDirection;	
typedef enum{DATA_REGISTER, INSTRUCTION_REGISTER} RegType;
typedef enum{FOUR_BIT, EIGHT_BIT } DataLength;
typedef enum{ONE_LINE, TWO_LINES} Lines;
typedef enum{FONT5x8, FONT5x10} Font;	
	
/************************************************************************/
/* API					                                                                  */
/************************************************************************/
void InitializeLcd(volatile BYTE* dataOutputPortReg,
				   volatile BYTE* dataInputPortReg,
				   volatile BYTE* dataDirReg,
				   volatile BYTE* controlOutputPortReg,
				   volatile BYTE* controlInputPortReg,
				   BYTE rsPin,
				   BYTE rwPin,
				   BYTE enablePin);

void WriteNewLine(char* string, BYTE line);
void ClearCharacter(BYTE line, BYTE pos);
void WriteToPosition(char* string, BYTE line, BYTE pos, BYTE positionsToClear);

/************************************************************************/
/* Control and Display Instructions API                                                      */
/************************************************************************/

void ClearDisplay(void);
void ReturnHome(void);
void SetEntryMode(CursorDirection direction, BOOL shift);
void DisplayOnOffControl(BOOL displayOn, BOOL cursorOn, BOOL blinkOn);
void CursorShift(Direction cursorDirection, Direction displayDirection);
void FunctionSet(DataLength length, Lines lines, Font font);
void SetCharacterGeneratorAddress(BYTE address);
void SetDisplayDataAddress(BYTE address);
BYTE ReadAddressCounter(void);
BOOL IsBusy(void);


/************************************************************************/
/* Low Level API			                                                                  */
/************************************************************************/

void WriteDataReg(BYTE dataToWrite);
void WriteInstructionReg(BYTE dataToWrite);
void WriteLcd(BYTE dataToWrite, RegType regType);

BYTE ReadInstructionReg(void);
BYTE ReadDataReg(BYTE address);
BYTE ReadLcd(RegType regType);

#endif /* LCD16X2_H_ */