#ifndef _LCD_RAW_H_
#define _LCD_RAW_H_
#include "HAL.h"

typedef enum {
	LCD_DATALEN_4 = 0,
	LCD_DATALEN_8 = 1
} LCDDataLen_t;

typedef enum {
	LCD_LINEMODE_1 = 0,
	LCD_LINEMODE_2 = 1
} LCDLineMode_t;

typedef enum {
	LCD_FONTMODE_5X8 = 0,
	LCD_FONTMODE_5X10 = 1
} LCDFontMode_t;

typedef struct {
	
	PinDef_t			rsPin;
	PinDef_t			rwPin;
	PinDef_t			enablePin;
	
	// For 8-bit data length, set all data pins 0-7.
	// For 4-bit data length, only pin definitins for data pins 4-7 are required. 
	LCDDataLen_t	dataLen;
	PinDef_t			dataPins[8];
	
	// Set to 1 if software initialization is required (i.e. no internal reset circuit). 
	uint8_t				initByInstruction;

} LCDPinConfig_t;

// This structure holds the internal state of the display.  Its contents can be ignored
//	outside the driver.  LCD_RawInitialize will take care of initializing it after which it
//	must be passed into every subsequent raw function.
typedef struct {

	LCDPinConfig_t pinConfig;
	
	enum DataDirection_t {
		DATA_UNCONFIGURED = -1,
		DATA_WRITE = 0,
		DATA_READ = 1
	} dataDirection;

	volatile uint32_t* dataPinAddresses[8];
	volatile uint32_t* rsPinAddress;
	volatile uint32_t* rwPinAddress;
	volatile uint32_t* enablePinAddress;
	
} LCDRawDisplay_t;

int LCD_RawInitialize(LCDRawDisplay_t* display, const LCDPinConfig_t* pinConfig, LCDLineMode_t lineMode, LCDFontMode_t fontMode);
void LCD_RawSetDisplayControl(LCDRawDisplay_t* display, uint8_t displayOn, uint8_t cursorOn, uint8_t blinkOn);
void LCD_RawSetEntryMode(LCDRawDisplay_t* display, uint8_t incrementMode, uint8_t shiftOn);
void LCD_RawSetDDRAMAddress(LCDRawDisplay_t* display, uint8_t address);
void LCD_RawClearDisplay(LCDRawDisplay_t* display);
void LCD_RawReturnHome(LCDRawDisplay_t* display);
void LCD_RawShiftCursor(LCDRawDisplay_t* display, uint8_t right);
void LCD_RawShiftDisplay(LCDRawDisplay_t* display, uint8_t right);
void LCD_RawPutChar(LCDRawDisplay_t* display, char c);
void LCD_RawPutString(LCDRawDisplay_t* display, const char* s);
#endif
