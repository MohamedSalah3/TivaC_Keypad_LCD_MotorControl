#include "UART.h"

void uart_init(void)
{
	SYSCTL->RCGCUART |= 0x01;  
    SYSCTL->RCGCGPIO |= 0x01;  
 SysTick_Wait10us(1);
    /* UART0 initialization */
    UART0->CTL = 0;         
    UART0->IBRD = 104;      
    UART0->FBRD = 11;       
    UART0->CC = 0;          
    UART0->LCRH = 0x60;    
    UART0->CTL = 0x301;     

  
    GPIOA->DEN = 0x03;      
    GPIOA->AFSEL = 0x03;    
    GPIOA->AMSEL = 0;   
    GPIOA->PCTL = 0x11;     
}	

int UART0_Receiver(void)  
{
    int data;
	  while((UART0->FR & (1<<4)) != 0); 
    data = UART0->DR ;  	
    return (unsigned int) data; 
}

void UART0_Transmitter(unsigned int data)  
{
    while((UART0->FR & (1<<5)) != 0); 
    UART0->DR = data;                 
}

void printstring(int str)
{
  while(str)
	{
		UART0_Transmitter((str++));
	}
}
