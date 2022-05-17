#include "Keypad.h" 


#define  ROWSSIZE  4
#define  COLSSIZE  4
volatile uint32_t count_pwm = 0;
volatile static char ARR2 ={0};
/* Array of 4x4 to define characters which will be printe on specific key pressed *//*
static unsigned  char symbol[ROWSSIZE][COLSSIZE] = {{ '1', '2',  '3', 'A'},      
                                                { '4', '5',  '6', 'B'},      
                                               { '7', '8',  '9', 'C'},      
                                               { '*', '0',  '#', 'D'}}; 
*/void keypad_Init(void){ /* Keypad initialization */
 
SYSCTL_RCGCGPIO_R |= 0x02; /* enable clock to GPIOB */
SYSCTL_RCGCGPIO_R |= 0x04; /* enable clock to GPIOC */
 
GPIO_PORTB_DIR_R |= 0x0F; /* set row pins 3-0 as output */
GPIO_PORTB_DEN_R |= 0x0F; /* set row pins 3-0 as digital pins */

 
GPIO_PORTC_DIR_R = 0x00; /* set column pin 7-4 as input */
GPIO_PORTC_DEN_R |= 0xF0; /* set column pin 7-4 as digital pins */
GPIO_PORTC_PUR_R |=0xF0;	

}



char keypad_getkey(void){ /*Scan and detect key pressed */
	static char row, col;

	const unsigned char keymap[4][4] = {
{ '1', '2', '3', '='},
{ '4', '5', '6', 'B'},
{ '7', '8', '9', 'C'},
{ '*', '0', '#', 'D'},

}; 

/* If a key is pressed, it gets here to find out which key. */
/* Although it is written as an infinite loop, it will take one of the breaks or return in one pass.*/
while (1)
{
	/*
if(PWM1_3_GENA_R == 0)
	{
	count_pwm++;
	
	}

	*//*
	uart_Reciever_VAR = UART0_Receiver();
	itoa(uart_Reciever_VAR,ARR2);		
		LCD_PutString(&display2_,ARR2 , 1, 9);*/
row = 0;
GPIO_PORTB_DATA_R = 0x0E; /* enable row 0 */
SysTick_Wait10us(1);
 /* wait for signal to settle */
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0) 
	break;
 
row = 1;
GPIO_PORTB_DATA_R = 0x0D; /* enable row 1 */
	SysTick_Wait10us(1);
/* wait for signal to settle */
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0)
break;
 
row = 2;
GPIO_PORTB_DATA_R = 0x0B; /* enable row 2 */
SysTick_Wait10us(1); /* wait for signal to settle */
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0)
break;

row = 3;
GPIO_PORTB_DATA_R = 0x07; /* enable row 3 */
SysTick_Wait10us(1);
 /* wait for signal to settle */
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0)
break;

}
 
/* gets here when one of the rows has key pressed */
if (col == 0xE0) return keymap[row][0]; /* key in column 0 */
if (col == 0xD0) return keymap[row][1]; /* key in column 1 */
if (col == 0xB0) return keymap[row][2]; /* key in column 2 */
if (col == 0x70) return keymap[row][3]; /* key in column 3 */

return 'N'; /* just to be safe */

}
unsigned char keypad_kbhit(void)
{
int col;
 
/* check to see any key pressed */
GPIO_PORTB_DATA_R = 0; /* enable all rows */
col = GPIO_PORTC_DATA_R & 0xF0; /* read all columns */
if (col == 0xF0)
return 'M'; /* no key pressed */
else
return 'X'; /* a key is pressed */
}



void delay(unsigned long millisecs){
  unsigned long count;
  while(millisecs > 0 ) 
		{ 
    count = 333333; 
    while (count > 0) { 
      count--;
    } 
    millisecs--;
  }
}
