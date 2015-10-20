/*--------------------------------------------------------------------------------------------------------------------------------------------------------
 * Project:		Library for LCD 16x2
 * Hardware:		LCD Display 16x2 type YM1602C
 * Micro:			ATMEGA328P
 * IDE:			Atmel Studio 6.2
 *
 * Name:    		lcd16x2.c
 * Purpose: 		LCD 16x2 Library
 * Date:			01-10-2015
 * Version:		1.0	
 * Author:		Marcel van der Ven
 *
 * Release notes:	Oct. 10, 2015:	1.0 - Initial Release
 *
 * Note(s):
 *--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/************************************************************************/
/* Defines				                                                                  */
/************************************************************************/
#define F_CPU			16000000UL
#define CLEAR_LCD		0b00000001
#define RETURN_HOME		0b00000010
#define CLEAR_CHAR		0x20

/************************************************************************/
/* Includes				                                                                  */
/************************************************************************/
#include <avr/io.h>
#include "util/delay.h"
#include "lcd16x2.h"
#include "string.h"


/************************************************************************/
/* Structures				                                                                  */
/************************************************************************/
struct Lcd16x2
{	
	volatile uint8_t* dataOutputPortRegister;
	volatile uint8_t* dataInputPortRegister;
	volatile uint8_t* dataDirRegister;
	struct PinSettings rs;
	struct PinSettings rw;
	struct PinSettings enable;
	
	/* Specifies if the LCD structure is initialized, it can only be used when its initialized */
	BOOL initialized;
	
	/* Specifies if setup of the LCD is completed (this happens after FunctionSet is called) */
	/* Afterwards we can poll the BusyFlag */
	BOOL setupCompleted;
} lcd;


/************************************************************************/
/* Functions				                                                                  */
/************************************************************************/	

/***************************************************************************
*  Function:		WriteToPosition(char* string, BYTE line, BYTE pos, BYTE posistionsToClear)
*  Description:		Writes at the given line to the given position the given string, the number of posistionsToClear
				will first be cleared before writing.
*  Receives:		char* string		:	Pointer to the string to write
				BYTE line			:	The line to write to.
				BYTE pos			:	The position on the line (zero-based)
				BYTE posistionsToClear:	Number of characters positions to clear from position onwards.
*  Returns:		Nothing
***************************************************************************/
void WriteToPosition(char* string, BYTE line, BYTE pos, BYTE positionsToClear)
{
	int length = strlen(string);
	
	/* Check if the line is smaller then 2 and the position smaller then 17 */
	if(!(line > 2 || pos > 16))
	{
		/* Check if we can write to the position, we don't use shift so maximum length of a line is 16 */
		/* If we want to write to position 10 then we can write 7 characters, so the string should not be greater then 7 characters */
		if(length <= (16 - pos))
		{
			/* First clear the characters, from position till number of posistionsToClear */
			for(int i = pos; i < (pos + positionsToClear); i++)
			{
				ClearCharacter(line, i);	
			}
			
			/* Calculate start address */
			BYTE address = (line-1) << 6;
			address += pos;
			
			/* Now put cursor back to position */
			SetDisplayDataAddress(address);
			
			/* Write characters */
			for(int i = 0; i < length; i++)
			{
				WriteDataReg(string[i]);
			}
		}
	}
}

/***************************************************************************
*  Function:		ClearCharacter(BYTE line, BYTE pos)
*  Description:		Clears the character on the given line and position.
				This is done by writing 0x20 to the character.
*  Receives:		BYTE line			:	The line to write to.
				BYTE pos			:	The position on the line (zero-based)
*  Returns:		Nothing
***************************************************************************/
void ClearCharacter(BYTE line, BYTE pos)
{
	/* Check if the line is smaller then 2 and the position smaller then 17 */
	if(!(line > 2 || pos > 16))
	{
		/* Line 1 is address 0x00 till 0x27 */
		/* Line 2 is address 0x40 till  0x67 */
		/* Set line mask, bit 6 is 0 for line 1 and 1 for line 2 */
		BYTE address = (line-1) << 6;
		
		/* Add position to address */
		address += pos;
		
		/* Set the address to write to */
		SetDisplayDataAddress(address);
		
		/* Clear the character at the set address  */
		WriteDataReg(CLEAR_CHAR);
	}
}

/***************************************************************************
*  Function:		WriteNewLine(char* string, BYTE line)
*  Description:		Writes the given string to the given line, 
				the function first clears the 16 characters of the line
*  Receives:		char* string			:	Pointer to the string to write
				BYTE line				:	The line to write to.
*  Returns:		Nothing
***************************************************************************/
void WriteNewLine(char* string, BYTE line)
{
	WriteToPosition(string, line, 0, 16);
}

/***************************************************************************
*  Function:		InitializeLcd(volatile BYTE* dataOutputPortReg,
						   volatile BYTE* dataInputPortReg,
						   volatile BYTE* dataDirReg,
						   volatile BYTE* controlOutputPortReg,
						   volatile BYTE* controlInputPortReg,
						   BYTE rsPin,
						   BYTE rwPin,
						   BYTE enablePin)
*  Description:		Initializes the LCD structure with the given register addresses and port numbers.
				After that the LCD API can be used without specifying addresses or pin numbers.
*  Receives:		BYTE* dataOutputPortReg	:	Dataregister output port address
				BYTE* dataInputPortReg		:	Data input port register
				BYTE* dataDirReg			:	Data direction register
				BYTE* controlOutputPortReg	:	Control output port register
				BYTE* controlInputPortReg	:	Control input port register
				BYTE* rsPort				:	RS port address			
				BYTE rsPin,				:	RS pin number	
				BYTE* rwPort				:	RW port address
				BYTE rwPin				:	RW pin number		
				BYTE* enablePort			:	Enable port address	
				BYTE enablePin				:	Enable pin number
*  Returns:		Nothing
***************************************************************************/
void InitializeLcd(volatile BYTE* dataOutputPortReg, 
				   volatile BYTE* dataInputPortReg, 	
				   volatile BYTE* dataDirReg,
				   volatile BYTE* controlOutputPortReg,
				   volatile BYTE* controlInputPortReg,
				   BYTE rsPin,
				   BYTE rwPin,
				   BYTE enablePin)
{
	lcd.dataOutputPortRegister = dataOutputPortReg;
	lcd.dataInputPortRegister = dataInputPortReg;
	lcd.dataDirRegister = dataDirReg;
	
	lcd.rs.outputPort = controlOutputPortReg;
	lcd.rs.inputPort = controlInputPortReg;
	lcd.rs.pin = rsPin;
	lcd.rw.outputPort = controlOutputPortReg;
	lcd.rw.inputPort = controlInputPortReg;
	lcd.rw.pin = rwPin;
	lcd.enable.outputPort = controlOutputPortReg;
	lcd.enable.inputPort = controlInputPortReg;
	lcd.enable.pin = enablePin;
	
	/* Set boolean to indicate LCD struct is initialized */
	lcd.initialized = TRUE;
}


/*********************************************************************************************/
/*********************************************************************************************/
/* Low-level functions																*/
/*********************************************************************************************/
/*********************************************************************************************/

/***************************************************************************
*  Function:		WriteLcd(BYTE dataToWrite, RegType regType)
*  Description:		Writes the given byte to the instruction register.
*  Receives:		BYTE dataToWrite			:	Byte to write.
				RegisterType regType		:	Type of register to write to.
*  Returns:		Nothing
***************************************************************************/
void WriteLcd(BYTE dataToWrite, RegType regType)
{
	if(lcd.initialized)
	{
		while(IsBusy() && lcd.setupCompleted);
		
		/* Set the port as output */
		*lcd.dataDirRegister = 0b11111111;
		
		/* Determine register to write to */
		if(regType == INSTRUCTION_REGISTER)
		{
			CLEAR_BIT(lcd.rs.outputPort, lcd.rs.inputPort, lcd.rs.pin);
		}
		else
		{
			SET_BIT(lcd.rs.outputPort, lcd.rs.inputPort, lcd.rs.pin);			
		}
		
		/* Set to write */
		CLEAR_BIT(lcd.rw.outputPort, lcd.rw.inputPort, lcd.rw.pin);
		
		/* Wait at least 40 ns (Address Setup Time tsp1) */
		_delay_us(1);
		SET_BIT(lcd.enable.outputPort, lcd.enable.inputPort, lcd.enable.pin);
		
		/* Set data to write */
		*lcd.dataOutputPortRegister = dataToWrite;
		
		/* Wait at least 230 ns (E pulse Width tpw) */
		_delay_us(1);
		
		/* Disable LCD */
		CLEAR_BIT(lcd.enable.outputPort, lcd.enable.inputPort, lcd.enable.pin);
		
		/* Wait at least 10 ns (Address Hold Time thd) */
		_delay_us(1);
		
		/* Reset to reading */
		SET_BIT(lcd.rw.outputPort, lcd.rw.inputPort, lcd.rw.pin);
	}
}

/***************************************************************************
*  Function:		WriteInstructionReg(BYTE dataToWrite)
*  Description:		Writes the given byte to the instruction register.
*  Receives:		BYTE dataToWrite			:	Byte to write.
*  Returns:		Nothing
***************************************************************************/
void WriteInstructionReg(BYTE dataToWrite)
{
	WriteLcd(dataToWrite, INSTRUCTION_REGISTER);	
}

/***************************************************************************
*  Function:		WriteDataReg(BYTE dataToWrite)
*  Description:		Writes the given byte to the data register.
*  Receives:		BYTE dataToWrite			:	Byte to write.
*  Returns:		Nothing
***************************************************************************/
void WriteDataReg(BYTE dataToWrite)
{
	WriteLcd(dataToWrite, DATA_REGISTER);	
}

/***************************************************************************
*  Function:		ReadLcd(RegType regType)
*  Description:		Reads the LCD display instruction or data register, in the later
				case the calling function should first set the address to read. 
*  Receives:		RegType regType	:	Type of register to read from.
*  Returns:		Nothing
***************************************************************************/
BYTE ReadLcd(RegType regType)
{
	BYTE dataRead = 0;
	
	if(lcd.initialized == TRUE)
	{
		/* First set the port as input */
		*lcd.dataDirRegister = 0b00000000;
	
		/* Determine register to read from */
		if(regType == INSTRUCTION_REGISTER)
		{
			CLEAR_BIT(lcd.rs.outputPort, lcd.rs.inputPort, lcd.rs.pin);
		}
		else
		{
			SET_BIT(lcd.rs.outputPort, lcd.rs.inputPort, lcd.rs.pin);
		}
	
		/* Set to read */
		SET_BIT(lcd.rw.outputPort, lcd.rw.inputPort, lcd.rw.pin);
	
		/* Wait at least 40 ns (Address Setup Time tsp1) */
		_delay_us(1);
		SET_BIT(lcd.enable.outputPort, lcd.enable.inputPort, lcd.enable.pin);
	
		/* Wait at least 150 ns (Data output delay time td) */
		_delay_us(1);
		
		/* Read data */
		dataRead = *lcd.dataInputPortRegister;
		
		/* Disable LCD */
		CLEAR_BIT(lcd.enable.outputPort, lcd.enable.inputPort, lcd.enable.pin);
	
		/* Wait at least 10 ns (Address Hold Time thd) */
		_delay_us(1);
	
	}
	return dataRead;
}

/***************************************************************************
*  Function:		ReadInstructionReg()
*  Description:		Reads the instruction register.
*  Receives:		Nothing
*  Returns:		The byte that is read.
***************************************************************************/
BYTE ReadInstructionReg(void)
{
	/* Read from the address and return the byte thats read */
	return ReadLcd(INSTRUCTION_REGISTER);
}

/***************************************************************************
*  Function:		ReadDataReg()
*  Description:		Reads the data register from the given address.
*  Receives:		BYTE address		:	Address to read from.
*  Returns:		The byte that is read.
***************************************************************************/
BYTE ReadDataReg(BYTE address)
{
	/* First set the DDRAM address to read from */
	SetDisplayDataAddress(address);
	
	/* Read from the address and return the byte thats read */
	return ReadLcd(DATA_REGISTER);
}


/***************************************************************************
*  Function:		ClearDisplay()
*  Description:		Clears the display.
*  Receives:		Nothing
*  Returns:		Nothing
***************************************************************************/
void ClearDisplay(void)
{
	WriteInstructionReg(CLEAR_LCD);
}

/***************************************************************************
*  Function:		ReturnHome()
*  Description:		Put the cursor at the home position and set DDRAM address counter to 00h.
*  Receives:		Nothing
*  Returns:		Nothing
***************************************************************************/
void ReturnHome(void)
{
	WriteInstructionReg(RETURN_HOME);
}

/***************************************************************************
*  Function:		SetEntryMode()
*  Description:		Sets the entry mode, by setting the direction the cursor moves
				and if the display shifts this is used when writing or reading data from the LCD.
*  Receives:		CursorDirection direction	:	The direction of the cursor (Increment or Decrement)
				BOOL shift				:	True when the display needs to shift.
*  Returns:		Nothing
***************************************************************************/
void SetEntryMode(CursorDirection direction, BOOL shift)
{
	/* Create data byte */
	BYTE dataToWrite = 0b00000100;
	
	if(direction == INCREMENT)
		dataToWrite |= 0b00000010;
		
	if(shift == TRUE)
		dataToWrite |= 0b00000001;
		
	WriteInstructionReg(dataToWrite);
}

/***************************************************************************
*  Function:		DisplayOnOffControl(BOOL displayOn, BOOL cursorOn, BOOL blinkOn)
*  Description:		Controls if the display needs to be set on, the cursor need to be on 
				and if the cursor should blink.
*  Receives:		BOOL displayOn	:	True when the display should be set ON
				BOOL cursorOn	:	True when the cursor should be set ON
				BOOL blinkOn		:	True when the cursor should blink.
*  Returns:		Nothing
***************************************************************************/
void DisplayOnOffControl(BOOL displayOn, BOOL cursorOn, BOOL blinkOn)
{
	/* Create data byte */
	BYTE dataToWrite = 0b00001000;
	
	if(displayOn != 0)
		dataToWrite |= 0b00000100;
	if(cursorOn != 0)
		dataToWrite |= 0b00000010;
	if(blinkOn != 0)
		dataToWrite |= 0b00000001;

	WriteInstructionReg(dataToWrite);
}

/***************************************************************************
*  Function:		CursorShift(Direction cursorDirection, Direction displayDirection)
*  Description:		Shifts the cursor and or display, without changing the DDRAM data.
*  Receives:		Direction	cursorDirection	:	The direction the cursor should shift (left or right)
				Direction displayDirection	:	The direction the display should shift (left or right)
*  Returns:		Nothing
***************************************************************************/
void CursorShift(Direction cursorDirection, Direction displayDirection)
{
	/* Create data byte */
	BYTE dataToWrite = 0b00010000;
		
	if(cursorDirection == RIGHT)
		dataToWrite |= 0b00000100;
	if(displayDirection == RIGHT)
		dataToWrite |= 0b00001000;
		
	WriteInstructionReg(dataToWrite);
}

/***************************************************************************
*  Function:		FunctionSet(DataLength length, Lines lines, Font font)
*  Description:		Sets the data length the uP uses to communicate with the display,
				the number of lines that need to be displayed and the font that need
				to be used.
*  Receives:		DataLength length	:	The data length, either 4 or 8 bits.
				Lines lines		:	The number of lines, 1 or 2.
				Font font			:	The font, either 5x8 or 5x10 dots
*  Returns:		Nothing
***************************************************************************/
void FunctionSet(DataLength length, Lines lines, Font font)
{
	/* Create data byte */
	BYTE dataToWrite = 0b00100000;
	
	if(length == EIGHT_BIT)
		dataToWrite |= 0b00010000;
	if(lines == TWO_LINES)
		dataToWrite |= 0b00001000;
	/* The display cant display two lines with font 5x10 dots */
	if(lines != TWO_LINES && font == FONT5x10)
		dataToWrite |= 0b00000100;
		
	WriteInstructionReg(dataToWrite);
	
	/* Set setup to complete, after this function we can use the BusyFlag */
	lcd.setupCompleted = TRUE;
}

/***************************************************************************
*  Function:		SetCharacterGeneratorAddress(BYTE address)
*  Description:		Sets the given character generator RAM address to the address counter.
				Afterwards we can read or write from the address.
*  Receives:		BYTE address	: The address to set.
*  Returns:		Nothing
***************************************************************************/
void SetCharacterGeneratorAddress(BYTE address)
{
	/* Create data byte */
	BYTE dataToWrite = 0b01000000;
	
	/* Set address, mask out bits 6 and 7 of the address (the address is only 6 bits) */
	dataToWrite |= (address & 0b00111111);
	
	WriteInstructionReg(dataToWrite);
}

/***************************************************************************
*  Function:		SetDisplayDataAddress(BYTE address)
*  Description:		Sets the given Display Data address to the address counter.
				Afterwards we can read or write from the address.
				
				For a one-line display the address range is 0x00 - 0x4F.
				For a two-line display the address range is 0x00 - 0x27 (line1) and 0x40 - 0x67 (line2)
				
*  Receives:		BYTE address	: The address to set.
*  Returns:		Nothing
***************************************************************************/
void SetDisplayDataAddress(BYTE address)
{
	/* Create data byte */
	BYTE dataToWrite = 0b10000000;
	
	/* Set address, mask out bit 7 of the address (the address is only 7 bits) */
	dataToWrite |= (address & 0b01111111);
	
	WriteInstructionReg(dataToWrite);
}

/***************************************************************************
*  Function:		ReadAddressCounter()
*  Description:		Reads the Address Counter, ignores the busy flag value by masking it out.
*  Receives:		Nothing
*  Returns:		Address counter address.
***************************************************************************/
BYTE ReadAddressCounter(void)
{
	/* Read address and ignore bit 7 by masking it out */
	BYTE address = ReadInstructionReg() & 0x7F;
	
	return address;
}

/***************************************************************************
*  Function:		IsBusy()
*  Description:		Reads the busy flag, ignores the Address Counter value.
*  Receives:		Nothing
*  Returns:		Boolean indicating the state of the busy flag (True == Busy).
***************************************************************************/
BOOL IsBusy(void)
{
	BYTE data = ReadInstructionReg();
	BOOL isBusy = TRUE;
	
	/* Check if bit 7 is 0, then we return FALSE */
	if(((data >> 7) & 0x01) == 0)
		isBusy = FALSE; 
	
	_delay_ms(1);
		
	return isBusy;
}

