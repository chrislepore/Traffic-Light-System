//make a array for each number
// import value from the dip switch
// put dip value in array brackekts and set output equal to it

#include "stm32f4xx.h"

int main(void) {
    RCC->AHB1ENR =  4;                 /* enable GPIOC clock */

    //GPIOC->MODER &= ~0x00000C00;        /* clear pin mode */
		GPIOC->PUPDR =  0x00550000;        /* set pin to input mode 		  PC 8 - 11 */
    GPIOC->MODER =  0x00005555;        /* set pin to output mode 		PC 0 - 7 */
		
		unsigned int lookup[16] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
		while(1) {
			int i = ((GPIOC->IDR >> 8) & 0x0F);
			GPIOC->ODR = lookup[i];
    }	
}
