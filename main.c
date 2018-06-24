#include <c8051f020.h>
#include "pwm.h"
#include "temp.h"
#include "smbus.h"

//this?
#define LCD 0x3E
//or this?
//#define LCD 0x7C

bit sendToLCD= 0;
bit getTemp= 0;
bit getPositions= 0;

int time_lcd= 0;
int time_temp= 0;

unsigned char cmd_lcd[8] = {0x38, 0x39, 0x14, 0x74, 0x54, 0x6F, 0x0C, 0x01};

/******************************************************************************
 * INIT
 *****************************************************************************/

void init() {
	EA = 0;
	WDTCN = 0xde; // disable watchdog timer
	WDTCN = 0xad;
	OSCICN &= 0x14; // Disable missing clock detector and set
					// internal osc at 2 MHz as the clock source

	XBR0 = 0x05; // Route SMBus to GPIO pins through crossbar, enable UART0
				 // in order to properly connect pins.

	XBR2 = 0x40; // Enable crossbar and weak pull-ups

	SMB0CN = 0x44; // Enable SMBus with ACKs on acknowledge cycle
	SMB0CR = -80;  // SMBus clock rate = 100kHz.
	EIE1 |= 2;	 // SMBus interrupt enable
	//SM_BUSY = 0;   // Free SMBus for first transfer.

	P0MDOUT |= 0x40; // Set P1.6 to push-pull
	EIE2 |= 0x20;

	// Timer 2 (100 ms)
/*	RCAP2H = 0xb8; // Reload value, 47150
	RCAP2L = 0x2e;
	TH2 = 0xb8; // Starting value, 47150
	TL2 = 0x2e;
	IE |= 0x20; // Enable Timer 2 interrupt (ET2)
*/	// End Timer 2

	// Timer 3 (dim)
	TMR3H = 0xff; // Starting value, 65280
	TMR3L = 0x00;
	TMR3RLH = 0xff; // Reload value, 65280
	TMR3RLL = 0x00;
	EIE2 |= 0x01; // Enable Timer 3 interrupt (ET3)

	TMR3CN |= 0x04; // Start Timer 3 (TR3)
	// End Timer 3

	// Timer 4 (100ms, always)
	RCAP4H = 0xb8; // Reload value, 47150
	RCAP4L = 0x2e;
	TH4 = 0xb8; // Starting value, 47150
	TL4 = 0x2e;
	EIE2 |= 0x04;  // Enable Timer 4 interrupt
	T4CON |= 0x04; // Start Timer 4
	// End Timer 4

	EA = 1; // Global interrupt enable
}

/******************************************************************************
 * TIMER 4
 *****************************************************************************/
void t4(void) interrupt 16 {
	time_lcd++;
	time_temp++;
	getPositions= 1;

	if(time_lcd == 3) {
		sendToLCD= 1;
		time_lcd= 0;
	}

	if(time_temp == 10) {
		getTemp= 1;
		time_temp= 0;
	}

	T4CON &= 0x7F;
}

/******************************************************************************
 * PROCEDURA DI MAIN
 *****************************************************************************/
void main(void) {
	init();
	SM_Send(LCD, cmd_lcd, 8, COM);

	pwmMain();

	if(getPositions) {
		//DO STUFF FOR "QUEL DIAVOLO DI ACCELEROMETRO"
	}

	if(sendToLCD) {
		//TODO:
		//SEND ACC DATA
		SM_Send(LCD, tempToWrite, 4, DAT);
	}

	if(getTemp) {
		tempMain();
	}
}