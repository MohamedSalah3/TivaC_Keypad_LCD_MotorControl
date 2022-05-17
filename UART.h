#ifndef __UART_H_
#define  __UART_H_



#include "TM4C123.h"
#include <stdint.h>
#include <stdlib.h>
#include "HAL_SysTick.h"


int UART0_Receiver(void);
void UART0_Transmitter(unsigned int data);
void printstring(int str);
void uart_init(void);

#endif 

