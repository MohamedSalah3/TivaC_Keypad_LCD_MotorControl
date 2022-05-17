#ifndef _LCD_H_
#define _LCD_H_
#include "LCD_Raw.h"

// This structure holds some additional state for the display, including the
//	raw state.  LCD_Initialize will take care of initializing it after which it can
//	be passed into every subsequent LCD function.

typedef struct {
	
	LCDRawDisplay_t raw;
	
	int rows;
	int columns;

	int cursorRow;
	int	cursorColumn;
	
} LCDDisplay_t;

typedef enum {
	LCD_CURSOR_LEFT,
	LCD_CURSOR_RIGHT,
	LCD_CURSOR_UP,
	LCD_CURSOR_DOWN
} LCDCursorDirection_t;

/*Prototypes*/
int LCD_Initialize(LCDDisplay_t* display, const LCDPinConfig_t* config, int rows, int columns);
void LCD_SetCursorPosition(LCDDisplay_t* display, int row, int column);
void LCD_ShiftCursor(LCDDisplay_t* display, LCDCursorDirection_t direction);
void LCD_PutChar(LCDDisplay_t* display, char c, int row, int column);
void LCD_PutString(LCDDisplay_t* display, const char* s, int row, int column);
void LCD_EnableCursor(LCDDisplay_t* display, uint8_t enable, uint8_t blinking);

#endif
