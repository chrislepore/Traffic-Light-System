; Assembly program using Assembly to toggle the green LED (LD2 -> PA5)
;   on STM32F446RE Nucleo64 board at 1 Hz
;
            EXPORT  __Vectors
            EXPORT  Reset_Handler
            AREA    vectors, CODE, READONLY

__Vectors   DCD     0x10010000  ; 0x20008000    ; Top of Stack
            DCD     Reset_Handler               ; Reset Handler

RCC_AHB1ENR equ 0x40023830
GPIOC_MODER equ 0x40020800
GPIOC_ODR   equ 0x40020814
	
GPIOC_PUPDR equ 0x4002080C
GPIOC_IDR   equ 0x40020810

            AREA    PROG, CODE, READONLY
Reset_Handler
            ldr     r4, =RCC_AHB1ENR    ; enable GPIOC clock
			mov	    r5, #4
            str     r5, [r4]
            
            ldr     r4, =GPIOC_PUPDR    
            ldr     r5, =0x00550000    	; set pin to input mode (pull up resistor)		  PC 8 - 11 
            str     r5, [r4]
			
			ldr		r4, =GPIOC_MODER
			ldr		r5, =0x00005555		; set pin to output mode 		PC 0 - 7
			str		r5, [r4]
			
			ldr 	r0, =GPIOC_IDR	;input port
			ldr		r1, =GPIOC_ODR	;output port
			ADR		r8, SevenSeg
			
again		ldr 	r3, [r0] 		; read switches
			LSR		r3, r3, #8		; shifts input bits 8 to the right
			bic 	r3, r3, #0xF0	; isolate PC8 - PC11 , clear PC0 - PC7
			ldrb 	r4, [r8, r3]	; gets value from array
			strb 	r4, [r1]		; stores value in output
			b		again		
			
SevenSeg 	DCB 	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
			
			end
			