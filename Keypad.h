#ifndef _KEYPAD_H_
#define _KEYPAD_H_
#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "LCD.h"
#include "UART.h"
#include "Strings.h"
void delay(unsigned long millisecs);
void keypad_Init(void); /* Keypad initialization */
char keypad_getkey(void); /*Scan and detect key pressed */
unsigned char keypad_kbhit(void);
extern volatile uint32_t count_pwm;
extern int uart_Reciever_VAR;
extern LCDDisplay_t display2_;
#endif
