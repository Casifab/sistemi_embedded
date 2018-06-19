#include <c8051f020.h>
#include "pwm.h"

#define ON 1
#define OFF 0

sbit Button= P3^7;
sbit Led= P0^6;

unsigned long int timer;
sfr16 RCAP2 = 0xCA; // Timer2 capture/reload
sfr16 T2 = 0xCC;	// Timer2
sbit Over = T2CON^7;

int mode;	 // 1 = modalità configurazione, 0 viceversa
int direzione = 1; //1 avanti, -1 indietro
unsigned char Lumi;
bit ledStatus;

void init(void) {
	EA = 0; //disabilita interrupt

	OSCICN = 0x04; //2Mhz

	//disabilita watchdog timer
	WDTCN = 0xDE;
	WDTCN = 0xAD;

	XBR0 = 0x00;
	XBR1 = 0x00;
	XBR2 = 0x40;

	EIE1 |= 0x02;

	P0MDOUT = 0x040; //setta pin come out

	EA = 1;

	Led= 0;
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

	if (Led == ON) {
		Led = OFF;  // spengo led
		TL0 = Lumi; // imposto duty-cycle
	}
	else {
		Led = ON; // accendo led
		TL0 = 0xFF - Lumi; // imposto duty-cycle
	}

	TR0 = 1;
}

void timer2(unsigned int counts) { //timer lampeggio led
	T2CON = 0x00;
	RCAP2 = counts; // Init reload values in the Capture registers
	T2 = 0xFFFF;	// count register set to reload immediately when the first clock occurs
	IE |= 0x20;		// IE.5, Enable Timer 2 interrupts (ET2)
	T2CON |= 0x04;
}

void interrupt_timer2(void) interrupt 5 { 
	// lanciato ogni 10ms
	T2CON &= ~(0x80);
	if (mode == 0) {
		//no modalità config
		timer++; // incremento contatore
		if (timer == 100) {
			// se sono arrivato a 1s
			mode = 1; // entro in modalità configurazione
			TR0 = 1;	   // faccio ripartire timer 0
			timer = 0;	 // azzero contatore
		}
	}
	else {
		// se sono in modalità configurazione
		Lumi = Lumi + direzione; // modifico luminosità
		if (Lumi >= 255 || Lumi <= 0) {
			// se la luminosità è fuori range
			direzione = -direzione; // cambio direzione
		}
	}
}

void init_button(void) {
	P3IF= 0x00;
	EIE2|= 0x20;
	Led= OFF;
	ledStatus= OFF;
}

void click_button(void) interrupt 19 {
	P3IF &= 0x7F;
	if(P3IF == 0x00) {
		P3IF= 0x08;
		timer2(63500);
		timer= 0;
	}
	else {
		P3IF= 0x00;
		//stoppo il timer 0
		TF0 = 0;
		TR0 = 0;
		T2CON &= ~(0x80);
		T2CON &= ~(0x04);
		if(mode == 0) {
			//no modalità config
			if(ledStatus == ON) {
				//setto led spento
				Led= OFF;
				ledStatus= OFF;
			}
			else {
				//accendo led e riavvio timer
				TR0= 1;
				Led= ON;
				ledStatus= ON;
			}
		}
		else {
			TR0= 1;
			Led= ON;
			ledStatus= ON;
			mode= 0;
		}
	}
}

void pwm() {
	mode= 0;
	ledStatus= ON;
	Lumi= 128;
	direzione= 1;
}

void main(void) {
	init();
	init_button();
	pwm();
	timer0();

	while(1);
}