//make a array for each number
// import value from the dip switch
// put dip value in array brackekts and set output equal to it

#include "stm32f4xx.h"

void delayMs(int n);

int main(void) {
    RCC->AHB1ENR =  4;                 /* enable GPIOC clock */
		//GPIOC->PUPDR =  0x00550000;        /* set pin to input mode 	  PC 8 - 11 */
    GPIOC->MODER =  0x00005555;        /* set pin to output mode 		PC 0 - 7 */
		
		unsigned int lookup[16] = {0x00,0x3F,0x00,0x3F,0x00,0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x6F};
		int i;
		while(1){
			for(i=15;i>0;i--)
			{
				GPIOC->ODR = lookup[i];
				delayMs(500);
			}
    }	
}

/* 16 MHz SYSCLK */
void delayMs(int n) {
    int i;

    /* Configure SysTick */
    SysTick->LOAD = 16000;  /* reload with number of clocks per millisecond */
    SysTick->VAL = 0;       /* clear current value register */
    SysTick->CTRL = 0x5;    /* Enable the timer */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNTFLAG is set */
            { }
    }
    SysTick->CTRL = 0;      /* Stop the timer (Enable = 0) */
}
