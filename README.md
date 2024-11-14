# Universal Traffic Intersection Controller

This project introduces a Universal Traffic Intersection Controller designed for versatile use in various traffic settings, including urban, rural, and crosswalk scenarios. Implemented with the STM32F446RE microcontroller and Nucleo-64 board, the controller is configurable to seven operational modes, managed through an LCD menu interface. This project aims to enhance understanding of embedded systems, interrupts, and real-time traffic control applications.

## Features

- **Seven Modes of Operation:**
  - **Urban**: Standard traffic light sequence.
  - **Rural**: Includes car sensors for adaptive timing.
  - **Urban with Crosswalk**: Adds crosswalk timing and countdown.
  - **Rural with Crosswalk**: Integrates both car sensors and crosswalk timing.
  - **Flashing Yellow East-West, Red North-South**.
  - **Flashing Yellow North-South, Red East-West**.
  - **Flashing Red for Both Directions**.

- **LCD Menu Interface**:
  - Allows mode selection and real-time changes to timing delays.
  - Enables scheduled mode transitions based on time-of-day.
  - Contains a kill-switch to safely stop traffic sequences while adjusting settings.

- **Crosswalk Countdown Display**:
  - Uses a 7-segment display to show the countdown for pedestrian crossing during crosswalk-enabled modes.

- **Hardware Design**:
  - Ports PA4 to PA9 are allocated for LED traffic lights.
  - PB0 to PB7 drive the 7-segment display.
  - Ports PC4 to PC7 are connected to the LCD display data pins.
  - Other ports include kill-switch, car sensors, and crosswalk triggers.

## Hardware and Circuit Design

The hardware configuration uses the following components:
- **Microcontroller**: STM32F446RE on a Nucleo-64 board.
- **Display**: LCD and 7-segment display for interactive menu and countdown.
- **LEDs**: Represent traffic lights, with resistors to prevent overcurrent.

### Circuit Diagram
Below are block diagrams illustrating the setup of the traffic light controller circuit:


![hardware block diagram](https://raw.githubusercontent.com/chrislepore/Traffic-Light-System/refs/heads/main/Images/Hardware%20Diagram.PNG)

## Software and Flowcharts

The software is programmed in C, leveraging interrupts for efficient control over the traffic light sequences. The project was expanded to include ARM Assembly code for certain functions, deepening understanding of low-level programming.

### ARM Assembly code (Example)

```asm
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
			
```

### Traffic Light Control Flow
The program starts by initializing ports, clocks, the LCD, and interrupts. Based on the current mode, the controller executes the corresponding traffic light sequence.

### Crosswalk and Sensor Integration
The rural mode incorporates car sensors, keeping the green light active until no vehicle is detected. Crosswalks, managed by user-input buttons, interrupt the cycle to initiate a countdown on the 7-segment display.

### Flowcharts

![Traffic Light Control](https://raw.githubusercontent.com/chrislepore/Traffic-Light-System/refs/heads/main/Images/TrafficLight%20Control%20Flowchart.PNG)

## Mode Switching

The system can switch modes based on a real-time schedule. A SysTick interrupt handles this, triggering mode changes without interrupting an ongoing traffic light cycle.

## Verification and Testing

The system was tested on a breadboard circuit and successfully validated for all modes. Below is an image of the complete circuit setup:

![breadboard circuit image](https://raw.githubusercontent.com/chrislepore/Traffic-Light-System/refs/heads/main/Images/Breadboard.jpg)

![Nucleo-64 board](https://raw.githubusercontent.com/chrislepore/Traffic-Light-System/refs/heads/main/Images/stm32%20DevBoard.jpg)

## Conclusion

The Universal Traffic Intersection Controller offers a robust, adaptable solution for diverse traffic environments. Through this project, coding in C with interrupts and ARM Assembly for embedded systems was further explored, improving understanding of the STM32 architecture and port utilization.

## Acknowledgments

This project was guided by materials from:

1. *Mazidi, M. A., Chen, S., & Ghaemi, E.* STM32 ARM Programming for Embedded Systems Using C Language with STM32 Nucleo. Mazidi, 2018.
2. *Mazidi, M. A., Naimi, S., & Chen, S.* ARM Assembly Language Programming & Architecture. MicroDigitalEd, 2016.
