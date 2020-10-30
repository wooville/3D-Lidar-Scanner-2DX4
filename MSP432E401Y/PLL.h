// PLL.h
// Runs on TM4C1294
// A software function to change the bus frequency using the PLL.
// Daniel Valvano
// March 27, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Program 4.6, Section 4.3
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Program 2.10, Figure 2.37

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// The #define statement PSYSDIV initializes
// the PLL to the desired frequency.
#define PSYSDIV 15
// bus frequency is 480MHz/(PSYSDIV+1) = 480MHz/(15+1) = 30 MHz
// IMPORTANT: See Step 6) of PLL_Init().  If you change something, change 480 MHz.
// IMPORTANT: You can use the 10-bit PSYSDIV value to generate an extremely slow
// clock, but this may cause the debugger to be unable to connect to the processor
// and return it to full speed.  If this happens, you will need to erase the whole
// flash or somehow slow down the debugger.  You should also insert some mechanism
// to return the processor to full speed such as a key wakeup or a button pressed
// during reset.  The debugger starts to have problems connecting when the system
// clock is about 600,000 Hz or slower; although inserting a failsafe mechanism
// will completely prevent this problem.
// IMPORTANT: Several peripherals (including but not necessarily limited to ADC,
// USB, Ethernet, etc.) require a particular minimum clock speed.  You may need to
// use the 16 MHz PIOSC as the clock for these modules instead of the PLL.

// configure the system to get its clock from the PLL
void PLL_Init(void);

/*
PSYSDIV  SysClk (Hz)
  3     120,000,000
  4      96,000,000
  5      80,000,000
  7      60,000,000
  9      48,000,000
 15      30,000,000
 19      24,000,000
 29      16,000,000
 39      12,000,000
 79       6,000,000
*/
