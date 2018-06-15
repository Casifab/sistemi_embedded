#include <c8051f020.h>
#include "pwm.h"
#define SPENTO 0
#define ACCESO 1

sbit Bottone = P3^7;
sbit Led = P0^6;
unsigned long int timer;
unsigned char Lumi;
sfr16 RCAP2 = 0xCA; // Timer2 capture/reload
sfr16 Timer2 = 0xCC;	// Timer2
sbit Over = T2CON^7;
int modalita; //modalita' configurazione. se il valore e' 1 = attiva. 0 viceversa.
int direzione = 1; 
bit Statusled;

void init(void){
    EA = 0; //Disabilita interrupt
    
    OSCICN = 0x04; //2Mhz

    //Disabilita watchdog timer
    WDTCN = 0x04;
    WDTCN = 0xAD;

    XBR0 = 0x00;
    XBR1 = 0x00;
    XBR2 = 0x40;

    EIE1 |= 0x02;
    
    P1MDOUT = 0x040; //Setta il pin come out

    EA = 1;

    Led = 0; //led spento
}
void timer0(void) {
	TMOD |= 0x02;
	CKCON |= 0x04;
	TL0 = 0xFF; // valore iniziale
	TH0 = 0x00; // valore da cui ripartire dopo overflow
	ET0 = 1;
	TR0 = 1; // faccio partire timer 0
}

void interrupt_timer0() interrupt 1 { 
	//onda duty-cycle tramite timer
	TF0 = 0;
	TR0 = 0; // stoppo timer 0

	if (Led == ACCESO) {
		Led = SPENTO;  // spengo led
		TL0 = Lumi; // imposto duty-cycle
	}
	else {
		Led = ACCESO; // accendo led
		TL0 = 0xFF - Lumi; // imposto duty-cycle
	}

	TR0 = 1;
}

void init_button(void) {
	P3IF= 0x00;
	EIE2|= 0x20;
	Led = SPENTO;
	Statusled = SPENTO;
}


void pwm() {
	modalita = 0;
	Statusled = ACCESO;
	Lumi = 128;
	direzione= 1;
}

void main(void) {
	init();
	init_button();
	pwm();
	timer0();

	while(1);
}