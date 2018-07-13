#include <c8051f020.h>
#include "pwm.h"
#include "temp.h"
#include "smbus.h"
#include "acc.h"

//this?
#define LCD 0x3E
//or this?
//#define LCD 0x7C

bit sendToLCD= 0;
bit getTemp= 0;
bit getPositions= 0;

unsigned char time_lcd= 0;
unsigned char time_temp= 0;

unsigned char cmd_lcd[8] = {0x38, 0x39, 0x14, 0x74, 0x54, 0x6F, 0x0C, 0x01};
unsigned char cmd_acc[4] = {0x06, 0x00, 0x01, 0x03};
//unsigned char clean_lcd[]= {0x00, 0x01};

unsigned char clean_lcd= 0x80;
unsigned char enter= 0xC0;

/******************************************************************************
 * INIT
 *****************************************************************************/

void init() {
	EA = 0;
	WDTCN = 0xde; // disable watchdog timer
	WDTCN = 0xad;
	OSCICN |= 0x04; // Disable missing clock detector and set
					// internal osc at 2 MHz as the clock source

	XBR0 = 0x05; // Route SMBus to GPIO pins through crossbar, enable UART0
				 // in order to properly connect pins.
	XBR1= 0x00;
	XBR2 = 0x40; // Enable crossbar and weak pull-ups

	SMB0CN = 0x44; // Enable SMBus with ACKs on acknowledge cycle
	SMB0CR = -80;  // SMBus clock rate = 100kHz.
	SCON0= 0x00; //UART0
	EIE1 |= 2;	 // SMBus interrupt enable
	SM_Busy = 0;   // Free SMBus for first transfer.

	P0MDOUT |= 0x40; // Set P1.6 to push-pull
	EIE2 |= 0x20;

	EA = 1; // Global interrupt enable
}

/******************************************************************************
 * INIZIALIZZAZIONE TIMER 4
 *****************************************************************************/
void init_t4(void) {
	// Timer 4 (100ms, always)
	RCAP4H = 0xb8; // Reload value, 47150
	RCAP4L = 0x2e;
	TH4 = 0xb8; // Starting value, 47150
	TL4 = 0x2e;
	EIE2 |= 0x04;  // Enable Timer 4 interrupt
	T4CON |= 0x04; // Start Timer 4
	// End Timer 4
}

/******************************************************************************
 * TIMER 4
 *****************************************************************************/
void t4(void) interrupt 16 {
	// ISR lanciata ogni 100ms
	time_lcd++;
	time_temp++;
	getPositions= 1; // alzo flag per la lettura degli angoli

	if(time_lcd == 3) { // passati 300ms
		sendToLCD= 1; // flag per invio dati all'LCD
		time_lcd= 0;
	}

	if(time_temp == 10) { // passato 1s
		getTemp= 1; // flag per lettura della temperatura
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
	init_t4();
	pwmMain(); //file pwm.c
	SM_Send(ACCEL, cmd_acc, 4, ACC_SEND);

	while(1){
		if(getPositions) {
			getPositions= 0;
			accMain(); //file acc.c
		}

		if(sendToLCD) {
			sendToLCD= 0;
			SM_Send(LCD, &clean_lcd, 1, COM);
			SM_Send(LCD, acc_line, 14, DAT);
			SM_Send(LCD, &enter, 1, COM);
			SM_Send(LCD, tempToWrite, 4, DAT);
		}

		if(getTemp) {
			getTemp= 0;
			tempMain(); //file temp.c
		}
	}
}