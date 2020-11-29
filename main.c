//**********************************************************************//
//main.c
//
//Embedded Systems Software - Lab #7
//Modified by: Mitch Verbuyst
//						  March 23, 2018
//
//
//
//This lab uses Pulse Width Modulation (PWM) to control the brightness 
//of an LED on the STM board, but using TIM1.
//
//This main.c file controls the program.  First, TimerConfig() is called.
//This is an assembly file function that sets up the intialization for
//the GREEN LED (PE8).
//
//Next, TimeConfig() sets up the Timer 1 in a C function.
//
//Finally, an infinite loop is used to periodically change the brightness
//of the Green LED.
//
//The "something cool" for this lab was to use the PWM to control the tones
//in a small speaker.  The PWM was used to create a short song.
//
//GPIOE -> PE8 (Green LED)
//Clock Speed: MSI (16 MHz)
//**********************************************************************//

//Libraries
#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "TimerConfig.h"

//Protoypes
void TimeConfig(void);
extern void TimerConfig(void);
void playtone(int freq);
void SysTick_Initialize(uint32_t ticks);
void SysTick_Handler(void);
void Delay(uint32_t nTime);
void Clock_Init();
void Minecraft(void);
void JoyStick(void);


#define A4 77
#define B4 69
#define C4 130
#define D4 116
#define E4 103
#define F4 97
#define G4 86
#define JS_MASK 0xFFFFF300
#define JS_PUP 0x8AA



//Constants
volatile int32_t TimeDelay;

int main(void){

	int brightness = 1;
	int stepsize = 1;
	int i;
	uint32_t ticks = 1000;

	
	TimerConfig(); //configure green LED (in assembly)
	TimeConfig(); //configure timer 1
	JoyStick();
	Clock_Init();
	SysTick_Initialize(ticks);
	
	
#define LED 1
#ifdef LED 
	while(1){
		
		if ((brightness >= 999) || brightness <= 0){
			stepsize = -stepsize; //reverse direction
		}
		
		brightness += stepsize; //change brightness
		
		TIM1->CCR1 = brightness; //change duty cycle of channel 1 outputs
		
		Delay(1);
		for (i = 0; i<1000; i++); //short delay
		
	}
#endif
	
//#define Stick 1
#ifdef Stick
	while(1){
		//play A4 when center button is pushed
		if((GPIOA->IDR & 0x1) == 0x1){
			playtone(A4);
			while ((GPIOA->IDR &0x1) != 0x00);
		}
		
		//Play B4 when left button is pushed
		if((GPIOA->IDR & 0x2) == 0x2){
			playtone(B4);
			while ((GPIOA->IDR &0x2) != 0x00);
		}
		
		//PLay C4 when right button is pushed
		if((GPIOA->IDR & 0x4) == 0x4){
			playtone(C4);
			while ((GPIOA->IDR &0x4) != 0x00);
		}
		
		//Play D4 when up is pushed
		if((GPIOA->IDR & 0x8) == 0x8){
			playtone(D4);
			while ((GPIOA->IDR &0x8) != 0x00);
		}
			//Play E4 when up is pushed
		if((GPIOA->IDR & 0x20) == 0x20){
			playtone(E4);
			while ((GPIOA->IDR &0x20) != 0x00);
		}
	}
#endif
	
}

void TimeConfig(void)
{	
	//Counting Direction up
	TIM1->CR1 &= ~TIM_CR1_DIR;
	
	//Prescaler
	TIM1->PSC = 39;
	
	//Auto Reload
	TIM1->ARR = 999;
	
	//Clear output compare mode bits for channel 1
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	//Select PWM mode 1 output on channel 1
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	//Output 1 preload enable
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	
	//Select output polarity
	TIM1->CCER &= ~TIM_CCER_CC1NP;
	
	//enable complementary output of channel 1
	TIM1->CCER |= TIM_CCER_CC1NE;
	
	//Main output enable (MOE)
	TIM1->BDTR |= TIM_BDTR_MOE;
	
	//Output compare register for channel 1
	TIM1->CCR1 = 500;
	
	//Enable Counter
	TIM1->CR1 |= TIM_CR1_CEN;
	
}

void playtone(int freq)
{
	
	TIM1->ARR = freq;
	TIM1->CCR1 = freq/2;
	
}

void SysTick_Initialize(uint32_t ticks)
{
	SysTick->CTRL = 0; //disable systick 
	
	SysTick->LOAD = ticks - 1; //set reload register
	
	//set interrupt priority of systick to least urgency
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	
	SysTick->VAL = 0; //reset the systick counter value
	
	//Select processor clock 0 (external clock)
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
	
	//Enables systick interrupt, 1= Enable, 0=Disable
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	//Enable Systick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
}

void SysTick_Handler(void)
{
	if(TimeDelay > 0)
		TimeDelay--;
}

void Delay(uint32_t nTime)
{
	TimeDelay = nTime;
	while(TimeDelay !=0);
}

void Clock_Init()
{

	//set MSI as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	
	//Wait for MSIRDY to be set
	while((RCC->CR & RCC_CR_MSIRDY) == 0);
	
	//Set MSIRGSEL to 1 so that MSIRANGE bits in RCC_CR are used
	RCC->CR &= ~RCC_CR_MSIRGSEL;
	RCC->CR |= RCC_CR_MSIRGSEL;
	
	//Set MSIRANGE to 8 MHz
	RCC->CR &= ~RCC_CR_MSIRANGE;
	RCC->CR |= RCC_CR_MSIRANGE_7;
	
	//Wait for MSIRDY to be set
	while((RCC->CR & RCC_CR_MSIRDY) == 0);
	
}


void Minecraft(void)
{
	playtone(A4);
	Delay(1000);
	
	playtone(G4);
	Delay(1000);
	
	playtone(D4);
	Delay(1000);
	
	playtone(F4);
	Delay(1000);
	
	playtone(A4);
	Delay(1000);
	
	playtone(G4);
	Delay(1000);
	
	playtone(D4);
	Delay(1000);
	
	playtone(F4);
	Delay(1000);
	
}

void JoyStick(void)
{
	
	
	//Enable the clock to GPIO Port A
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	//GPIO Mode: Input(00)
	GPIOA->MODER &= JS_MASK;
	
	//GPIOA Push-Pull: pull-down(10)
	GPIOA->PUPDR &= JS_MASK;
	GPIOA->PUPDR |= JS_PUP;
	
}