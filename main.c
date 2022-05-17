#include "HAL.h"
#include "LCD.h"
#include "Strings.h"
#include "Keypad.h"
#include "HAL_PWM.h"
#include "UART.h"
#define PWM_PERIOD		3200
#define AMODE 1

 // Holds the content and edit state for one display.
typedef struct {
	
	char line1[17];
	char line2[17];
	
	uint8_t editing;
	uint8_t upperOn;
	
} DisplayContent_t;

// Switch bit-band aliases
static volatile uint32_t* heartbeatLED_;

// Display contexts.
//static LCDDisplay_t display1_;
//static
 LCDDisplay_t display2_;

// Display content.
static DisplayContent_t content1_;
static DisplayContent_t content2_;

// Indicates which of the two display is currently active (i.e. cursor on, taking input).
static int activeDisplay_ = 0;

// Call when there is a catastophic problem.
void ErrHandler(void)
{
	while (1) {}
}

// Examines the state of the 5-position switch moves the cursor.
// Resets the displays to there default content.
void ResetContent(void) ;

void ResetContent(void) 
{
	content1_.editing = 0;
	content2_.editing = 0;
	
	strncpy(content2_.line1, "Enter Mtr Speed ", 16);
	strncpy(content2_.line2, "       RPM      ", 16);
	
	
	LCD_PutString(&display2_, content2_.line1, 0, 0);	
	LCD_PutString(&display2_, content2_.line2, 1, 0);
	
	LCD_SetCursorPosition(&display2_, 0, 0);

	// This will cause the cursor to be enabled on whichever display
	//	is selected (when display switch is processed).
	activeDisplay_ = 0;
	
}
// Causes the displays to side scroll as if they are one, 32-character display.
void ScrollMarquee(void);
void ScrollMarquee(void)
{
	static int marqueeCounter = 0;
	
	// Shift the content every N intervals of the timer.
	marqueeCounter = (++marqueeCounter) % 3;
	
	if (marqueeCounter == 0) {
		
		char line1[33];
		char line2[33];
	
		// Copy the line content into single character arrays.
		for (int i = 0; i < 16; i++) {
			line1[i] = content1_.line1[i];
			line1[16 + i] = content2_.line1[i];
			line2[i] = content1_.line2[i];
			line2[16 + i] = content2_.line2[i];
		}
		
		// Shift/copy the lines back into their original arrays. 
		for (int i = 0; i < 16; i++) {
			
			int index1 = (i + 1) % 32;
			content1_.line1[i] = line1[index1];
			content1_.line2[i] = line2[index1];
			
			int index2 = (i + 17) % 32;
			content2_.line1[i] = line1[index2];
			content2_.line2[i] = line2[index2];
			
		}
		
		LCD_PutString(&display2_, content2_.line1, 0, 0);
		LCD_PutString(&display2_, content2_.line2, 1, 1);
		
	}
	
}
// Check the state of all switches and take actions accordingly.
// Initialize the hardware and peripherals...

void timer0A_delayMs(int ttime) {

  SYSCTL_RCGCTIMER_R |= 1;       //enable clock to Timer Block0

  TIMER0_CTL_R = 0;              //disable Timer before initialization

  TIMER0_CFG_R = 0X04;           //16-bit option

  TIMER0_TAMR_R = 0X01;          //one-shot mode and down-counter

  TIMER0_TAILR_R = 16000 * ttime - 1;    //Timer A interval load value register

  TIMER0_ICR_R = 0X1;            //clear the TimerA timeout flag. 

  TIMER0_CTL_R = 0X01;           //enable Timer A after initialization


}



static int InitHardware(void)
{

//	__disable_irq();
	
	PLL_Init80MHz();

	if (SysTick_Init() < 0) {
		return -1;
	}
		GPIO_EnableDO(PORTF, PIN3, DRIVE_2MA, PULL_DOWN);
	heartbeatLED_ = GPIO_GetBitBandIOAddress(&PINDEF(PORTF, PIN3));
	
	// Display 2 is 16x1.  Using a 4-bit bus, only need to set data pins 4-7.
	LCDPinConfig_t pinConfig2;
	pinConfig2.dataPins[4] = PINDEF(PORTB, PIN5);	
	pinConfig2.dataPins[5] = PINDEF(PORTE, PIN1);
	pinConfig2.dataPins[6] = PINDEF(PORTE, PIN2);
	pinConfig2.dataPins[7] = PINDEF(PORTE, PIN3);
	pinConfig2.rsPin = PINDEF(PORTA, PIN7);
	pinConfig2.rwPin = PINDEF(PORTA, PIN6);
	pinConfig2.enablePin = PINDEF(PORTA, PIN4);
	pinConfig2.dataLen = LCD_DATALEN_4;
	pinConfig2.initByInstruction = 0;
	if (LCD_Initialize(&display2_, &pinConfig2, 2, 16) < 0) {
		return -1;
	}
		
	ResetContent();
	uart_init();
	// Calls back every 100ms (with 80MHz bus clock).
//	Timer_EnableTimerPeriodic(TIMER2, 800000, 5, TimerCallback);
	
	
	
	
	__enable_irq();
	
	return 0;
	
}



//Spin motor in one direction by giving IN1 and IN2 signals to L298N
void Turn_oneDirection(void);
void Turn_oneDirection(void)
{
SYSCTL_RCGCGPIO_R |= 0x01;   /* enable clock to PORTF */
	   GPIO_PORTA_DIR_R |= (1<<3)|(1<<2);            /* pin digital */
     GPIO_PORTA_DEN_R|= (1<<3)|(1<<2);            /* pin digital */
     GPIO_PORTA_DATA_R |=(1<<2);
		 GPIO_PORTA_DATA_R &=~(1<<3);
}

//Spin motor in other direction by giving IN1 and IN2 signals to L298N
void Turn_OtherDirection(void);
void Turn_OtherDirection(void)
{
SYSCTL_RCGCGPIO_R |= 0x01;   /* enable clock to PORTF */
	   GPIO_PORTA_DIR_R |= (1<<3)|(1<<2);            /* pin digital */
     GPIO_PORTA_DEN_R |= (1<<3)|(1<<2);            /* pin digital */
     GPIO_PORTA_DATA_R |=(1<<3);
		 GPIO_PORTA_DATA_R &=~(1<<2);
}
void PWM_init(void);
void PWM_init(void)
{/* Clock setting for PWM and GPIO PORT */
SYSCTL_RCGCPWM_R |= 2;       /* Enable clock to PWM1 module */
SYSCTL_RCGCGPIO_R|= 0x20;  /* Enable system clock to PORTF */
SYSCTL_RCC_R |= (1<<20);    /* Enable System Clock Divisor function  */
SYSCTL_RCC_R |= 0x000E0000;
/* Use pre-divider valur of 64 and after that feed clock to PWM1 module*/
 /* Setting of PF2 pin for M1PWM6 channel output pin */
	GPIO_PORTF_AFSEL_R  |= (1<<2);          /* PF2 sets a alternate function */
GPIO_PORTF_PCTL_R &= ~0x00000F00; /*set PF2 as output pin */

	GPIO_PORTF_PCTL_R |= 0x00000500; /* make PF2 PWM*/

	GPIO_PORTF_DEN_R |= (1<<2);          /* set PF2 as a digital pin */
    
    PWM1_3_CTL_R &= ~(1<<0);   /* Disable Generator 3 counter */
  
	PWM1_3_CTL_R &= ~(1<<1);   /* select down count mode of counter 3*/

PWM1_3_GENA_R = 0x0000008C;  /* Set PWM output when counter reloaded and clear when matches PWMCMPA */
	PWM1_3_LOAD_R = 40000;     /* set load value for 50Hz 16MHz/64 = 250kHz and (250KHz/5000) */

PWM1_3_CMPA_R = 20000;     /* set duty cyle to 50%*/

 PWM1_3_CTL_R = 1;           /* Enable Generator 3 counter */

PWM1_ENABLE_R = 0x40;      /* Enable PWM1 channel 6 output */

}
void pwm_set_duty(uint32_t duty);
void pwm_set_duty(uint32_t duty){

PWM1_3_CMPA_R = duty-1;

}

static volatile uint32_t counter=0 ;

void Encoder_init(void);
void Encoder_init(void)
{
  SYSCTL_RCGCGPIO_R |= (1<<5);   /* Set bit5 of RCGCGPIO to enable clock to PORTF*/
    
	 /* PORTF0 has special function, need to unlock to modify */

	GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register */
    GPIO_PORTF_CR_R = 0x01;           /* make PORTF0 configurable */
    GPIO_PORTF_LOCK_R = 0;            /* lock commit register */


    /*Initialize PF3 as a digital output, PF0 and PF4 as digital input pins */
	
    GPIO_PORTF_DIR_R &= ~(1<<4);  /* Set PF4 and PF0 as a digital input pins */
    GPIO_PORTF_DIR_R |= (1<<3);           /* Set PF3 as digital output to control green LED */
    GPIO_PORTF_DEN_R |= (1<<4)|(1<<3);/* make PORTF4-0 digital pins */
    GPIO_PORTF_PUR_R |= (1<<4);             /* enable pull up for PORTF4, 0 */
    
    /* configure PORTF4, 0 for falling edge trigger interrupt */
    GPIO_PORTF_IS_R  &= ~(1<<4);        /* make bit 4, 0 edge sensitive */
    GPIO_PORTF_IBE_R &=~(1<<4);         /* trigger is controlled by IEV */
    GPIO_PORTF_IEV_R &= ~(1<<4);        /* falling edge trigger */
    GPIO_PORTF_ICR_R |= (1<<4);          /* clear any prior interrupt */
    GPIO_PORTF_IM_R  |= (1<<4);          /* unmask interrupt */
    
    /* enable interrupt in NVIC and set priority to 3 */
   // NVIC->IP[30] = 3 << 5;     /* set interrupt priority to 3 */
   // NVIC->ISER[0] |= (1<<30);  /* enable IRQ30 (D30 of ISER[0]) */
     
		 
		 
		 NVIC_EnableIRQ_Custom(30,3);

}
volatile static uint8_t arr[5]= {0},RPM[5]={0};
static uint8_t Get_input_flag =0;
static uint32_t Required_speed = 1,factor = 0;
int uart_Reciever_VAR = 0;
int main()
{
  int i = 0 ;
  Encoder_init();
	//LCD_PutChar(&display2_,'P', 1, 2);
  keypad_Init();
  Turn_oneDirection();
	if (InitHardware() < 0) {
		ErrHandler();
	}
	PWM_init();
	while (1) {
		
	char x = keypad_getkey();

	LCD_PutChar(&display2_,x, 1, i);
/***************************************/
		itoa(count_pwm/12,RPM);		

	  LCD_PutString(&display2_,RPM, 1, 12);
		
	// Toggle the heartbeat LED...
				// Wait one second...
		SysTick_Wait10ms(50);
		if(Get_input_flag == AMODE)
		i++;		
uart_Reciever_VAR = UART0_Receiver();		
if(x == 'D')
{	
	i=0;	
	LCD_RawClearDisplay(&display2_);
	LCD_PutString(&display2_, content2_.line1, 0, 0);	
	LCD_PutString(&display2_, content2_.line2, 1, 0);

}
if((x== '1'&& Get_input_flag == 0)||uart_Reciever_VAR == '1')
{
	// 20% 
  //(100-30)*40000
	//32000
	//
pwm_set_duty(24000);
}
if((x== '2'&& Get_input_flag == 0)||uart_Reciever_VAR == '2')
{
pwm_set_duty(20000);
}

if((x== '3'&& Get_input_flag == 0)||uart_Reciever_VAR == '3')
{
pwm_set_duty(16000);
}
if((x== '4'&& Get_input_flag == 0)||uart_Reciever_VAR == '4')
{
pwm_set_duty(8000);
}
if((x== '5'&& Get_input_flag == 0)||uart_Reciever_VAR == '5')
{
pwm_set_duty(0);
}
if(x=='=')
{
Get_input_flag =1;
factor = 1000;
}
if(x=='B')
{
Get_input_flag =0;
			pwm_set_duty(Required_speed);	

}	
if(x == '*'){
		//pwm_set_duty(0);
PWM_init();
}
if(x == '#')
{
	char arr_2 [5]={0};
Required_speed += 8000;
pwm_set_duty(Required_speed);	
	itoa(Required_speed,arr_2);
		LCD_PutString(&display2_,arr_2,1,6);
}
if(x == 'C')
{
Required_speed -= 1000;	
pwm_set_duty(Required_speed);	
}
if(Get_input_flag == 1)
{
if(x != '='){
if(x >= '0' && x<= '9')
Required_speed += (x -'0') * factor ;
factor /= 10;
if(factor == 1)
	if(Required_speed >= 1000 &&Required_speed <= 9999 )
	{	
		itoa(Required_speed-1,arr);
		LCD_PutString(&display2_,arr,1,6);
	}
	
}
}

if(Get_input_flag == 0)
	Required_speed = 0;
}

}

void GPIOF_Handler(void);
void GPIOF_Handler(void)
{	
	counter++;
  if (GPIO_PORTF_MIS_R & 0x10) /* check if interrupt causes by PF4/SW1*/
    {   
      GPIO_PORTF_DATA_R ^= (1<<3);
      GPIO_PORTF_ICR_R |= 0x10; /* clear the interrupt flag */
     } 
/*
	if(count_pwm >= 50)
	{

	counter=0;
		count_pwm=0;
	}
  	
		 */
}

