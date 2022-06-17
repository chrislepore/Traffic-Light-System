#include "stm32f4xx.h"

#define RS 0x01     /* PA0 mask for LCD reg select */
#define EN 0x02     /* PA1 mask for LCD enable */

void delayMs(int n);
void urbanTraffic(void);
void ruralTraffic(void);
void urbanTraffic_c(void);
void ruralTraffic_c(void);
void redRed(void);
void yNS_rEW(void);
void yEW_rNS(void);
void crosswalk(int button);

void LCD_HUD(void);
int cursor_update(int max, int c);
void LCD_print(char data[]);
void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);

void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void TIM2_IRQHandler(void);

int static mode = 0; 		//controls mode of traffic light
int static cursor = 0;	//controls LCD menu
int static stage = 0;		// controls menu operation
int static step = 0;		//controls menu operation step
int static G = 5;				//green light delay
int static Y = 2;				//yellow light delay
int static Ns_cross = 0;		//NS crosswalk regestor
int static Ew_cross = 0;		//EW crosswalk regestor
int static tod_enable = 0;  //change at time of day enable
int static mode_1;					//change at time of day mode 1
int static mode_2;					//change at time of day mode 2

int main(void) {
		LCD_init();
		GPIOB->ODR = 0x40; //dash
	
    while(1) 
		{
				if(mode != 3 || mode != 3) {Ns_cross = 0; Ew_cross = 0; GPIOB->ODR = 0x40;} //rest crosswalk register if not in use
				if((GPIOC -> IDR >> 11)	& 0x1) //kill switch
				{
					switch(mode) 
					{
						case 0: LCD_HUD();
										urbanTraffic();
										break;
						case 1: LCD_HUD();
										ruralTraffic();
										break;
						case 2: LCD_HUD();
										urbanTraffic_c();
										break;
						case 3: LCD_HUD();
										ruralTraffic_c();
										break;
						case 4: LCD_HUD();
										yEW_rNS();
										break;
						case 5: LCD_HUD();
										yNS_rEW();
										break;
						case 6:	LCD_HUD();
										redRed();
										break;
						default: urbanTraffic();
										break;
					}
				}
				else {LCD_HUD(); GPIOA->ODR = 0x00000240; TIM2->CNT = 0;} //sets interupt counter to 0 and LEDs to solid red
			
				delayMs(500);
				LCD_command(1);
    }
}

void LCD_HUD(void)
{
	char static modec[6] = {"6MODE:"}; 	
	char static p_back[3] = {"3<-"}; 
	char static p_front[3] = {"3->"};
	char static mc[13] = {"=MODE_CHANGE:"};
	char static td[12] = {"<TIME_DELAY:"};
	
	char static gy[4][3] = {"2G","2Y","3G:","3Y:"};
	char static operations[3][12] = {"<MODE_CHANGE",";TIME_DELAY","5TOD:"};
	char static modes[7][10] = {"6URBAN","6RURAL","8URBAN/c","8RURAL/c",":Y_EW/R_NS",":Y_NS/R_EW",":BLINK_RED"};
	char static tod[3][6] = {"4M1:","4M2:","6TIME:"};
	char static tod_en[2][8] = {"7ENABLE","8DISABLE"};
	char static tod_count[8][3] = {"25","310","315","320","325","330","335","340"};
	
	switch(stage)
	{
		case 0: LCD_print(modec); 	//base menu
						LCD_data(' ');
						LCD_print(modes[mode]);
						LCD_command(192); //next line
						if((~(GPIOC -> IDR >> 9)	& 0x1) || (~(GPIOC -> IDR >> 10)	& 0x1))	cursor = cursor_update(2, cursor); //call cursor update
						if(cursor != 0) LCD_print(p_back);
						LCD_print(operations[cursor]);
						if(cursor == 2) LCD_print(tod_en[((~tod_enable)&0x1)]);
						if(cursor != 2) LCD_print(p_front);
						if(~(GPIOC -> IDR >> 8)	& 0x1) {stage = cursor+1; cursor = 0;	delayMs(1000);}				//stage 0 menu controller
						break;
						
		case 1: LCD_print(mc);											//mode change menu
						LCD_command(192); //next line
						if((~(GPIOC -> IDR >> 9)	& 0x1) || (~(GPIOC -> IDR >> 10)	& 0x1)) cursor = cursor_update(6, cursor); //call cursor update
						if(cursor != 0) LCD_print(p_back);
						LCD_print(modes[cursor]);
						if(cursor != 6) LCD_print(p_front);
						if(~(GPIOC -> IDR >> 8)	& 0x1) {mode = cursor; cursor = 0; stage = 0; delayMs(1000);}    //stage 1 menu controller
						break;
						
		case 2: LCD_print(td); LCD_print(gy[0]); LCD_data(G|0x30); LCD_data(','); LCD_print(gy[1]); LCD_data(Y|0x30);		//time delay change
						LCD_command(192); //next line
						if(step == 0) LCD_print(gy[2]); else LCD_print(gy[3]);
						LCD_data(' ');
						if((~(GPIOC -> IDR >> 9)	& 0x1) || (~(GPIOC -> IDR >> 10)	& 0x1)) cursor = cursor_update(9, cursor); //call cursor update
						if(cursor != 0) LCD_print(p_back);
						LCD_data(cursor + 0x30); LCD_data(' '); LCD_data('S');
						if(cursor != 9) LCD_print(p_front);
						if(step == 0 && (~(GPIOC -> IDR >> 8)	& 0x1)) {G = cursor; cursor = 0; step = 1; delayMs(1000);}										//stage 2 menu controller
						else if (step == 1 && (~(GPIOC -> IDR >> 8)	& 0x1)) {Y = cursor; cursor = 0; step = 0; stage = 0; delayMs(1000);}
						break;
		case 3: LCD_print(operations[2]); 							//time of day mode change
						LCD_command(192); //next line
						if(step == 1) LCD_print(tod[0]); 
						if(step == 2) LCD_print(tod[1]); 
						switch(step)
						{
							case 0: if((~(GPIOC -> IDR >> 9)	& 0x1) || (~(GPIOC -> IDR >> 10)	& 0x1)) cursor = cursor_update(1, cursor); //select mode 1 , call cursor update
											if(cursor != 0) LCD_print(p_back);
											LCD_print(tod_en[cursor]);
											if(cursor != 1) LCD_print(p_front);
											break;
							case 1:
							case 2: if((~(GPIOC -> IDR >> 9)	& 0x1) || (~(GPIOC -> IDR >> 10)	& 0x1)) cursor = cursor_update(6, cursor);	//select mode 2 , call cursor update
											if(cursor != 0) LCD_print(p_back);
											LCD_print(modes[cursor]);
											if(cursor != 6) LCD_print(p_front);
											break;
							case 3: LCD_print(tod[2]);
											if((~(GPIOC -> IDR >> 9)	& 0x1) || (~(GPIOC -> IDR >> 10)	& 0x1)) cursor = cursor_update(7, cursor); //select time , call cursor update
											if(cursor != 0) LCD_print(p_back);
											LCD_print(tod_count[cursor]); LCD_data(' '); LCD_data('S');  
											if(cursor != 7) LCD_print(p_front);
											break;
						} // stage 3 menu controller
						if(step == 0 && (~(GPIOC -> IDR >> 8)	& 0x1) && cursor == 0) {cursor = 0; step = 1; tod_enable = 1; delayMs(1000);} //ENABLE time interup 
						else if(step == 0 && (~(GPIOC -> IDR >> 8)	& 0x1) && cursor == 1) {cursor = 0; tod_enable = 0; stage = 0; delayMs(1000);} //DISABLE time interup 
						else if(step == 1 && (~(GPIOC -> IDR >> 8)	& 0x1)) {mode_1 = cursor; mode = cursor; cursor = 0; step = 2; delayMs(1000);}
						else if(step == 2 && (~(GPIOC -> IDR >> 8)	& 0x1)) {mode_2 = cursor; cursor = 0; step = 3; delayMs(1000);}
						else if(step == 3 && (~(GPIOC -> IDR >> 8)	& 0x1)) {TIM2->ARR = (((5*cursor)+5)*1000)-1; cursor = 0; step = 0; stage = 0; delayMs(1000);}
						break;		
	}
}

void urbanTraffic(void)
{
	GPIOA->ODR =  0x00000210;  /* turn on NSG, EWR */
	delayMs(G*1000);
	GPIOA->ODR = 0x00000220;  /* turn on NSY, EWR */
	delayMs(Y*1000);
	GPIOA->ODR = 0x000000C0;  /* turn on NSR, EWG */
	delayMs(G*1000);
	GPIOA->ODR = 0x00000140;  /* turn on NSR, EWY */
	delayMs(Y*1000);
}
void ruralTraffic(void)
{
	GPIOA->ODR =  0x00000210;  /* turn on NSG, EWR */
	delayMs(G*1000);
	
	while(((GPIOC -> IDR)  & 0x1)){}  //NS sensor  

	GPIOA->ODR = 0x00000220;  /* turn on NSY, EWR */
	delayMs(Y*1000);
	GPIOA->ODR = 0x000000C0;  /* turn on NSR, EWG */
	delayMs(G*1000);

	while(((GPIOC -> IDR >>	1)	& 0x1)){}  // EW sensor 

	GPIOA->ODR = 0x00000140;  /* turn on NSR, EWY */
	delayMs(Y*1000);
}
void urbanTraffic_c(void)
{
	GPIOA->ODR =  0x00000210;  /* turn on NSG, EWR */
	delayMs(G*1000);
	GPIOA->ODR = 0x00000220;  /* turn on NSY, EWR */
	delayMs(Y*1000);
	
	crosswalk(Ns_cross);	Ns_cross = 0;	
	if(Ns_cross == 0 && Ew_cross == 0) GPIOB->ODR = 0x40; else GPIOB->ODR = 0xC0; 
	
	GPIOA->ODR = 0x000000C0;  /* turn on NSR, EWG */
	delayMs(G*1000);
	GPIOA->ODR = 0x00000140;  /* turn on NSR, EWY */
	delayMs(Y*1000);
	
	crosswalk(Ew_cross);	Ew_cross = 0;
	if(Ns_cross == 0 && Ew_cross == 0) GPIOB->ODR = 0x40; else GPIOB->ODR = 0xC0;
}
void ruralTraffic_c(void)
{
	GPIOA->ODR =  0x00000210;  /* turn on NSG, EWR */
	delayMs(G*1000);
	
	while(((GPIOC -> IDR)  & 0x1)){}  //NS sensor  

	GPIOA->ODR = 0x00000220;  /* turn on NSY, EWR */
	delayMs(Y*1000);
		
	crosswalk(Ns_cross);	Ns_cross = 0; 
	if(Ns_cross == 0 && Ew_cross == 0) GPIOB->ODR = 0x40; else GPIOB->ODR = 0xC0;
		
	GPIOA->ODR = 0x000000C0;  /* turn on NSR, EWG */
	delayMs(G*1000);

	while(((GPIOC -> IDR >>	1)	& 0x1)){}  // EW sensor 

	GPIOA->ODR = 0x00000140;  /* turn on NSR, EWY */
	delayMs(Y*1000);
		
	crosswalk(Ew_cross);	Ew_cross = 0; 
	if(Ns_cross == 0 && Ew_cross == 0) GPIOB->ODR = 0x40; else GPIOB->ODR = 0xC0;
}
void redRed(void)
{
	GPIOA->ODR = 0x00000240;	/* turn on NSR, EWR */
	delayMs(500);
	GPIOA->ODR = 0x00000000; /* turn off lights */
	delayMs(500);
}
void yNS_rEW(void)
{
	GPIOA->ODR = 0x00000220; /* turn on NSY, EWR */
	delayMs(500);
	GPIOA->ODR = 0x00000000; /* turn off lights */
	delayMs(500);
}
void yEW_rNS(void)
{
	GPIOA->ODR = 0x00000140;  /* turn on NSR, EWY */
	delayMs(500);
	GPIOA->ODR = 0x00000000; /* turn off lights */
	delayMs(500);
}
void crosswalk(int button)
{
	unsigned int lookup[16] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71}; // 0 - F array
	if(button == 1)
	{
		for(int i=5;i>=0;i--)
		{
			GPIOB->ODR = lookup[i]; // output counter to 7seg
			redRed();
		}
	}
}
/* initialize ports then initialize LCD controller */
void LCD_init(void) 
{
    PORTS_init();

    delayMs(20);                /* LCD controller reset sequence */
    LCD_nibble_write(0x30, 0);
    delayMs(5);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);

    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delayMs(1);
    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);          /* move cursor right */
    LCD_command(0x01);          /* clear screen, move cursor to home */
    LCD_command(0x0F);          /* turn on display, cursor blinking */
}
void PORTS_init(void) 
{
		__disable_irq();						// global disable IRQs
		
    RCC->AHB1ENR =  7;             /* enable GPIOA, GPIOB and GPIOC clock */
    RCC->APB2ENR = 0x4000;				 /* enable TIM2 clock*/
	
		GPIOC->PUPDR &= ~0x00FF00FF; /* clear pin mode */
		GPIOC->MODER &= ~0x0000FF00; /* clear pin mode */
		GPIOA->MODER &= ~0x000FFF0F; /* clear pin mode */
		GPIOB->MODER &= ~0x0000FFFF; /* clear pin mode */
	
		GPIOC->PUPDR =  0x00550055;		/* INPUT: PC0-PC3, PC8-PC11 set to input for mode sensors, cross buttons, menu controls, kill switch */
		GPIOC->MODER =  0x00005500;   /* set pin output mode for LCD PC4-PC7 for D4-D7*/
		GPIOA->MODER =  0x00055505;   /* LED:	set pin A4 - A9 to output mode for lights, PA0-RS, PA1-En for LCD */
		GPIOB->MODER =  0x00005555;	  /* 7-seg:	set pin to output mode PB 0 - 7 */
		GPIOA->BSRR  =  0x00020000;   /* turn off EN */
	
		SYSCFG->EXTICR[0] &= ~0xFF00; // clear ports
		SYSCFG->EXTICR[0] |= 0x2200; //select PORTS_init C for EXTI 2 and 3
		EXTI->IMR |= 0xC;            //unmask EXTI 2 and 3
		EXTI->RTSR |= 0xC;					// set rising edge trigger
		NVIC_EnableIRQ(8); //enable IRQ8 -- EXTI2
		NVIC_EnableIRQ(9); //enable IRQ9 -- EXTI3

		RCC->APB1ENR |= 1;		 			//enables interupt counter
		TIM2->PSC = 16000-1;				//reduces microcontroller clock speed
		TIM2->ARR = (10*1000)-1;  	// sets the count number for interupt
		TIM2->CR1 = 1;							//enable counter
		TIM2->DIER |= 1;						//enable UIE
		NVIC_EnableIRQ(TIM2_IRQn);	//enable interupt in NVIC

		__enable_irq();   // global disable IRQs
}

void TIM2_IRQHandler(void)
{
	TIM2->SR = 0;
	if(((GPIOC -> IDR >> 11)	& 0x1) && tod_enable == 1) // if kill switch is off and time of day change is enabled
	{
		if(mode == mode_1) mode = mode_2; //switch modes
		else mode = mode_1;
	}
}

void EXTI2_IRQHandler(void)
{
	if(mode == 2 || mode == 3) //PC1 Ns_cross
	{
		Ns_cross = 1;
		GPIOB->ODR = 0xC0;			//turn on registor and 7 seg dp LED
	}
	EXTI->PR = 0x4;
}

void EXTI3_IRQHandler(void)
{
	if(mode == 2 || mode == 3) //PC2 Ew_cross
	{
		Ew_cross = 1;
		GPIOB->ODR = 0xC0; 			//turn on registor and 7 seg dp LED
	}
	EXTI->PR = 0x8;
}

void LCD_nibble_write(char data, unsigned char control) 
{
    /* populate data bits */
    GPIOC->BSRR = 0x00F00000;       /* clear data bits */
    GPIOC->BSRR = data & 0xF0;      /* set data bits */

    /* set R/S bit */
    if (control & RS)
        GPIOA->BSRR = RS;
    else
        GPIOA->BSRR = RS << 16;

    /* pulse E */
    GPIOA->BSRR = EN;
    delayMs(0);
    GPIOA->BSRR = EN << 16;
}

void LCD_command(unsigned char command) 
{
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */

    if (command < 4)
        delayMs(2);         /* command 1 and 2 needs up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}

void LCD_data(char data) 
{
    LCD_nibble_write(data & 0xF0, RS);      /* upper nibble first */
    LCD_nibble_write(data << 4, RS);        /* then lower nibble */

    delayMs(1);
}

void LCD_print(char data[])
{
	int size = data[0] - 0x30;		//use first character as size
	for(int i=1;i<size;i++)			
	{
		if(data[i] != 0)
		{
			LCD_data(data[i]);		//print each character
			delayMs(30);
		}else break;
	}
}

int cursor_update(int max, int c)
{
	if(c == 0 && (~(GPIOC -> IDR >> 10)	& 0x1)) 
	{
		return max;
	}
	else if(c != 0 && (~(GPIOC -> IDR >> 10)	& 0x1))
	{
		return c-1;
	}
	else if(c != max && (~(GPIOC -> IDR >> 9)	& 0x1))
	{
		return c+1;
	}
	else if(c == max && (~(GPIOC -> IDR >> 9)	& 0x1))
	{
		return 0;
	}
}
/* 16 MHz SYSCLK */
void delayMs(int n) 
{
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

