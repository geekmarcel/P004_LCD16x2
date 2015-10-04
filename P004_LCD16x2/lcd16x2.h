/*--------------------------------------------------------------------------------------------------------------------------------------------------------
 * Project: 		LCD 16x2 Library
 * Hardware:		Arduino UNO
 * Micro:			ATMEGA328P
 * IDE:			Atmel Studio 6.2
 *
 * Name:    		lcd16x2.c
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

/************************************************************************/
/* Type Definitions			                                                                  */
/************************************************************************/
typedef enum{LEFT, RIGHT} Direction;
typedef enum{INCREMENT, DECREMENT} CursorDirection;	
typedef enum{DATA_REGISTER, INTRUCTION_REGISTER} RegType;
typedef enum{FOUR_BIT, EIGHT_BIT } DataLength;
typedef enum{ONE_LINE, TWO_LINES} Lines;
typedef enum{FONT5x8, FONT5x10} Font;	
	
/************************************************************************/
/* API					                                                                  */
/************************************************************************/
void InitializeLcd(uint8_t* dataregister, uint8_t* rs, uint8_t* rw);



/************************************************************************/
/* Control and Display Instructions API                                                      */
/************************************************************************/

void ClearDisplay();
void ReturnHome();
void SetEntryMode(CursorDirection direction, BOOL shift);
void DisplayOnOffControl(BOOL displayOn, BOOL cursorOn, BOOL blinkOn);
void CursorShift(Direction cursorDirection, Direction displayDirection);
void FunctionSet(DataLength length, Lines lines, Font font);
void SetCharacterGeneratorAddress(BYTE address);
void SetDisplayDataAddress(BYTE address);
BYTE ReadAddressCounter();
BOOL IsBusy();


/************************************************************************/
/* Low Level API			                                                                  */
/************************************************************************/

void WriteDataReg(BYTE dataToWrite);
void WriteInstructionReg(BYTE dataToWrite);
void WriteLcd(BYTE dataToWrite, RegType regType);

BYTE ReadInstructionReg(BYTE address);
BYTE ReadDataReg(BYTE address);

#endif /* LCD16X2_H_ */