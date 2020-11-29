;****************************************************************************************
;TimerConfig.s
;
;This is an assembly file function that initializes the green LED for the purposes of this
;lab.
;
;
;The Green LED is on PE8 (GPIO E, pin 8), so first the clock in enabled, along with TIM1
;being enabled.  PE8 is set to alternative mode, the AFR8 is set to alternative
;function, speed is set to high, no pull up no pull down, and push pull output type.
;
;****************************************************************************************
	
	
	
	INCLUDE core_cm4_constants.s
	INCLUDE stm32l476xx_constants.s


	AREA	TimerConfig, CODE, READONLY
	EXPORT TimerConfig ; make TimerConfig visible
	ENTRY
	
	
__TimerConfig PROC
	
	;Enable clock to GPIO port E
	LDR r2, =RCC_BASE
	LDR r1, [r2, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOEEN
	STR r1, [r2, #RCC_AHB2ENR]
	
	;Enable TIM1
	LDR r2, =RCC_BASE
	LDR r1, [r2, #RCC_APB2ENR]
	ORR r1, r1, #RCC_APB2ENR_TIM1EN
	STR r1, [r2, #RCC_APB2ENR]
	
	;use r0 for GPIO E base
	LDR r0, =GPIOE_BASE
	
	;;Set up GPIO E MODER TYPE for GREEN LED to alternative (10)
	LDR r1, [r0, #GPIO_MODER]
	LDR r2, =0x00020000
	LDR r3, =0xFFFCFFFF
	AND r1, r1, r3
	ORR r1, r1, r2
	STR r1, [r0, #GPIO_MODER]
	
	
	;;Set up GPIO E AFR[1] TYPE for GREEN LED
	LDR r1, [r0, #0x24]
	LDR r2, =0x00000001
	LDR r3, =0xFFFFFFF0
	AND r1, r1, r3
	ORR r1, r1, r2
	STR r1, [r0, #0x24]
	
	;set up GPIOE PUPDR to no pull up no pull down (00)
	LDR r1, [r0, #GPIO_PUPDR]
	LDR r2, =0xFFFCFFFF
	LDR r3, =0x00000000
	AND r1,r1,r2
	ORR r1,r1, r3
	STR r1, [r0, #GPIO_PUPDR]
	
	;set up GPIOE OTYPE as Push Pull (0)
	LDR r1, [r0, #GPIO_OTYPER]
	LDR r2, =0xFEFF
	LDR r3, =0x0000
	AND r1,r1,r2
	ORR r1,r1, r3
	STR r1, [r0, #GPIO_OTYPER]
	
	;set up GPIOE SPEED as high (10)
	LDR r1, [r0, #GPIO_OSPEEDR]
	LDR r2, =0xFFFCFFFF
	LDR r3, =0x00020000
	AND r1,r1,r2
	ORR r1,r1,r3
	STR r1, [r0, #GPIO_OSPEEDR]
		
	
	ENDP	
		
	END