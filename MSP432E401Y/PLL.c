// PLL.c
// Runs on TM4C1294
// A software function to change the bus frequency using the PLL.
// Daniel Valvano
// April 9, 2014

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
#include <stdint.h>
#include "PLL.h"

// The #define statement PSYSDIV in PLL.h
// initializes the PLL to the desired frequency.

// bus frequency is 480MHz/(PSYSDIV+1) = 480MHz/(3+1) = 120 MHz
// IMPORTANT: See Step 6) of PLL_Init().  If you change something, change 480 MHz.
// see the table at the end of this file

#define SYSCTL_RIS_R            (*((volatile uint32_t *)0x400FE050))
#define SYSCTL_RIS_MOSCPUPRIS   0x00000100  // MOSC Power Up Raw Interrupt
                                            // Status
#define SYSCTL_MOSCCTL_R        (*((volatile uint32_t *)0x400FE07C))
#define SYSCTL_MOSCCTL_PWRDN    0x00000008  // Power Down
#define SYSCTL_MOSCCTL_NOXTAL   0x00000004  // No Crystal Connected
#define SYSCTL_RSCLKCFG_R       (*((volatile uint32_t *)0x400FE0B0))
#define SYSCTL_RSCLKCFG_MEMTIMU 0x80000000  // Memory Timing Register Update
#define SYSCTL_RSCLKCFG_NEWFREQ 0x40000000  // New PLLFREQ Accept
#define SYSCTL_RSCLKCFG_USEPLL  0x10000000  // Use PLL
#define SYSCTL_RSCLKCFG_PLLSRC_M                                              \
                                0x0F000000  // PLL Source
#define SYSCTL_RSCLKCFG_PLLSRC_MOSC                                           \
                                0x03000000  // MOSC is the PLL input clock
                                            // source
#define SYSCTL_RSCLKCFG_OSCSRC_M                                              \
                                0x00F00000  // Oscillator Source
#define SYSCTL_RSCLKCFG_OSCSRC_MOSC                                           \
                                0x00300000  // MOSC is oscillator source
#define SYSCTL_RSCLKCFG_PSYSDIV_M                                             \
                                0x000003FF  // PLL System Clock Divisor
#define SYSCTL_MEMTIM0_R        (*((volatile uint32_t *)0x400FE0C0))
#define SYSCTL_DSCLKCFG_R       (*((volatile uint32_t *)0x400FE144))
#define SYSCTL_DSCLKCFG_DSOSCSRC_M                                            \
                                0x00F00000  // Deep Sleep Oscillator Source
#define SYSCTL_DSCLKCFG_DSOSCSRC_MOSC                                         \
                                0x00300000  // MOSC
#define SYSCTL_PLLFREQ0_R       (*((volatile uint32_t *)0x400FE160))
#define SYSCTL_PLLFREQ0_PLLPWR  0x00800000  // PLL Power
#define SYSCTL_PLLFREQ0_MFRAC_M 0x000FFC00  // PLL M Fractional Value
#define SYSCTL_PLLFREQ0_MINT_M  0x000003FF  // PLL M Integer Value
#define SYSCTL_PLLFREQ0_MFRAC_S 10
#define SYSCTL_PLLFREQ0_MINT_S  0
#define SYSCTL_PLLFREQ1_R       (*((volatile uint32_t *)0x400FE164))
#define SYSCTL_PLLFREQ1_Q_M     0x00001F00  // PLL Q Value
#define SYSCTL_PLLFREQ1_N_M     0x0000001F  // PLL N Value
#define SYSCTL_PLLFREQ1_Q_S     8
#define SYSCTL_PLLFREQ1_N_S     0
#define SYSCTL_PLLSTAT_R        (*((volatile uint32_t *)0x400FE168))
#define SYSCTL_PLLSTAT_LOCK     0x00000001  // PLL Lock

// configure the system to get its clock from the PLL
void PLL_Init(void){ uint32_t timeout;
  // 1) Once POR has completed, the PIOSC is acting as the system clock.  Just in case
  //    this function has been called previously, be sure that the system is not being
  //    clocked from the PLL while the PLL is being reconfigured.
  SYSCTL_RSCLKCFG_R &= ~SYSCTL_RSCLKCFG_USEPLL;
  // 2) Power up the MOSC by clearing the NOXTAL bit in the SYSCTL_MOSCCTL_R register.
  // 3) Since crystal mode is required, clear the PWRDN bit.  The datasheet says to do
  //    these two operations in a single write access to SYSCTL_MOSCCTL_R.
  SYSCTL_MOSCCTL_R &= ~(SYSCTL_MOSCCTL_NOXTAL|SYSCTL_MOSCCTL_PWRDN);
  //    Wait for the MOSCPUPRIS bit to be set in the SYSCTL_RIS_R register, indicating
  //    that MOSC crystal mode is ready.
  while((SYSCTL_RIS_R&SYSCTL_RIS_MOSCPUPRIS)==0){};
  // 4) Set both the OSCSRC and PLLSRC fields to 0x3 in the SYSCTL_RSCLKCFG_R register
  //    at offset 0x0B0.
  //    Temporarily get run/sleep clock from 25 MHz main oscillator.
  SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R&~SYSCTL_RSCLKCFG_OSCSRC_M)+SYSCTL_RSCLKCFG_OSCSRC_MOSC;
  //    PLL clock from main oscillator.
  SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R&~SYSCTL_RSCLKCFG_PLLSRC_M)+SYSCTL_RSCLKCFG_PLLSRC_MOSC;
  // 5) If the application also requires the MOSC to be the deep-sleep clock source,
  //    then program the DSOSCSRC field in the SYSCTL_DSCLKCFG_R register to 0x3.
  //    Get deep-sleep clock from main oscillator (few examples use deep-sleep; optional).
  SYSCTL_DSCLKCFG_R = (SYSCTL_DSCLKCFG_R&~SYSCTL_DSCLKCFG_DSOSCSRC_M)+SYSCTL_DSCLKCFG_DSOSCSRC_MOSC;
  // 6) Write the SYSCTL_PLLFREQ0_R and SYSCTL_PLLFREQ1_R registers with the values of
  //    Q, N, MINT, and MFRAC to configure the desired VCO frequency setting.
  //    ************
  //    The datasheet implies that the VCO frequency can go as high as 25.575 GHz
  //    with MINT=1023 and a 25 MHz crystal.  This is clearly unreasonable.  For lack
  //    of a recommended VCO frequency, this program sets Q, N, and MINT for a VCO
  //    frequency of 480 MHz with MFRAC=0 to reduce jitter.  To run at a frequency
  //    that is not an integer divisor of 480 MHz, change this section.
  //    fVC0 = (fXTAL/(Q + 1)/(N + 1))*(MINT + (MFRAC/1,024))
  //    fVCO = 480,000,000 Hz (arbitrary, but presumably as small as needed)
#define FXTAL 25000000  // fixed, this crystal is soldered to the Connected Launchpad
#define Q            0
#define N            4  // chosen for reference frequency within 4 to 30 MHz
#define MINT        96  // 480,000,000 = (25,000,000/(0 + 1)/(4 + 1))*(96 + (0/1,024))
#define MFRAC        0  // zero to reduce jitter
  //    SysClk = fVCO / (PSYSDIV + 1)
#define SYSCLK (FXTAL/(Q+1)/(N+1))*(MINT+MFRAC/1024)/(PSYSDIV+1)
  SYSCTL_PLLFREQ0_R = (SYSCTL_PLLFREQ0_R&~SYSCTL_PLLFREQ0_MFRAC_M)+(MFRAC<<SYSCTL_PLLFREQ0_MFRAC_S) |
                      (SYSCTL_PLLFREQ0_R&~SYSCTL_PLLFREQ0_MINT_M)+(MINT<<SYSCTL_PLLFREQ0_MINT_S);
  SYSCTL_PLLFREQ1_R = (SYSCTL_PLLFREQ1_R&~SYSCTL_PLLFREQ1_Q_M)+(Q<<SYSCTL_PLLFREQ1_Q_S) |
                      (SYSCTL_PLLFREQ1_R&~SYSCTL_PLLFREQ1_N_M)+(N<<SYSCTL_PLLFREQ1_N_S);
  SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR;       // turn on power to PLL
  SYSCTL_RSCLKCFG_R |= SYSCTL_RSCLKCFG_NEWFREQ;      // lock in register changes
  // 7) Write the SYSCTL_MEMTIM0_R register to correspond to the new clock setting.
  //    ************
  //    Set the timing parameters to the main Flash and EEPROM memories, which
  //    depend on the system clock frequency.  See Table 5-12 in datasheet.
  if(SYSCLK < 16000000){
    // FBCHT/EBCHT = 0, FBCE/EBCE = 0, FWS/EWS = 0
    SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R&~0x03EF03EF) + (0x0<<22) + (0x0<<21) + (0x0<<16) + (0x0<<6) + (0x0<<5) + (0x0);
  } else if(SYSCLK == 16000000){
    // FBCHT/EBCHT = 0, FBCE/EBCE = 1, FWS/EWS = 0
    SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R&~0x03EF03EF) + (0x0<<22) + (0x1<<21) + (0x0<<16) + (0x0<<6) + (0x1<<5) + (0x0);
  } else if(SYSCLK <= 40000000){
    // FBCHT/EBCHT = 2, FBCE/EBCE = 0, FWS/EWS = 1
    SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R&~0x03EF03EF) + (0x2<<22) + (0x0<<21) + (0x1<<16) + (0x2<<6) + (0x0<<5) + (0x1);
  } else if(SYSCLK <= 60000000){
    // FBCHT/EBCHT = 3, FBCE/EBCE = 0, FWS/EWS = 2
    SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R&~0x03EF03EF) + (0x3<<22) + (0x0<<21) + (0x2<<16) + (0x3<<6) + (0x0<<5) + (0x2);
  } else if(SYSCLK <= 80000000){
    // FBCHT/EBCHT = 4, FBCE/EBCE = 0, FWS/EWS = 3
    SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R&~0x03EF03EF) + (0x4<<22) + (0x0<<21) + (0x3<<16) + (0x4<<6) + (0x0<<5) + (0x3);
  } else if(SYSCLK <= 100000000){
    // FBCHT/EBCHT = 5, FBCE/EBCE = 0, FWS/EWS = 4
    SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R&~0x03EF03EF) + (0x5<<22) + (0x0<<21) + (0x4<<16) + (0x5<<6) + (0x0<<5) + (0x4);
  } else if(SYSCLK <= 120000000){
    // FBCHT/EBCHT = 6, FBCE/EBCE = 0, FWS/EWS = 5
    SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R&~0x03EF03EF) + (0x6<<22) + (0x0<<21) + (0x5<<16) + (0x6<<6) + (0x0<<5) + (0x5);
  } else{
    // A setting is invalid, and the PLL cannot clock the system faster than 120 MHz.
    // Skip the rest of the initialization, leaving the system clocked from the MOSC,
    // which is a 25 MHz crystal.
    return;
  }
  // 8) Wait for the SYSCTL_PLLSTAT_R register to indicate that the PLL has reached
  //    lock at the new operating point (or that a timeout period has passed and lock
  //    has failed, in which case an error condition exists and this sequence is
  //    abandoned and error processing is initiated).
  timeout = 0;
  while(((SYSCTL_PLLSTAT_R&SYSCTL_PLLSTAT_LOCK) == 0) && (timeout < 0xFFFF)){
    timeout = timeout + 1;
  }
  if(timeout == 0xFFFF){
    // The PLL never locked or is not powered.
    // Skip the rest of the initialization, leaving the system clocked from the MOSC,
    // which is a 25 MHz crystal.
    return;
  }
  // 9)Write the SYSCTL_RSCLKCFG_R register's PSYSDIV value, set the USEPLL bit to
  //   enabled, and set the MEMTIMU bit.
  SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R&~SYSCTL_RSCLKCFG_PSYSDIV_M)+(PSYSDIV&SYSCTL_RSCLKCFG_PSYSDIV_M) |
                       SYSCTL_RSCLKCFG_MEMTIMU |
                       SYSCTL_RSCLKCFG_USEPLL;
}


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
