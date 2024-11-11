; Assembly program using Assembly to toggle the green LED (LD2 -> PA5)
;   on STM32F446RE Nucleo64 board at 1 Hz
;
            EXPORT  __Vectors
            EXPORT  Reset_Handler
            AREA    vectors, CODE, READONLY

__Vectors   DCD     0x10010000  ; 0x20008000    ; Top of Stack
            DCD     Reset_Handler               ; Reset Handler

RCC_AHB1ENR equ 0x40023830
GPIOA_MODER equ 0x40020000
GPIOA_ODR   equ 0x40020014
	
GPIOA_PUPDR equ 0x4002000C
GPIOA_IDR   equ 0x40020010

            AREA    PROG, CODE, READONLY
Reset_Handler
            ldr     r4, =RCC_AHB1ENR    ; enable GPIOA clock
			mov	    r5, #1
            str     r5, [r4]
            
            ldr     r4, =GPIOA_MODER    
            ldr     r5, =0x00055500    	; set pin A4 - A9 to output mode 
            str     r5, [r4]
			
			ldr		r4, =GPIOA_PUPDR
			ldr		r5, =0x00000005		; sets A0 and A1 to pull up resistor
			str		r5, [r4]
			

L1          ldr     r4, =GPIOA_ODR
            mov     r5, #0x00000210     	; turn on NSG/EWR
            str     r5, [r4]
			
			mov     r0, #5000
            bl      delay
			
			
L2			ldr 	r4, =GPIOA_IDR
			ldr		r5, [r4]
			ANDS	r4, r5, #1
			BNE		L2
			
            
            ldr     r4, =GPIOA_ODR
            mov     r5, #0x00000220    	; turn on NSY/EWR
            str     r5, [r4]
            
			mov     r0, #2000
            bl      delay
			
			ldr     r4, =GPIOA_ODR
            mov     r5, #0x000000C0     	; turn on NSR/EWG
            str     r5, [r4]
            
			mov     r0, #5000
            bl      delay
            
L3			ldr 	r4, =GPIOA_IDR
			ldr		r5, [r4]
			ANDS 	r4, r5, #2
			BNE		L3
			
            ldr     r4, =GPIOA_ODR
            mov     r5, #0x00000140    	; turn on NSR/EWY
            str     r5, [r4]
            
			mov     r0, #2000
            bl      delay
			
            b       L1                 ; loop forever

; delay milliseconds in R0
delay       ldr     r1, =5325
DL1         subs    r1, r1, #1
            bne     DL1
            subs    r0, r0, #1
            bne     delay
            bx      lr
            
            end   
			