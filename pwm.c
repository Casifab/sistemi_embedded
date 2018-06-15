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
    
    P0MDOUT = 0x040; //Setta il pin come out

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
	if (modalita == 0) {
		//no modalità config
		timer++; // incremento contatore
		if (timer == 100) {
			// se sono arrivato a 1s
			modalita = 1; // entro in modalità configurazione
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
		if(modalita == 0) {
			//no modalità config
			if(Statusled == ACCESO) {
				//setto led spento
				Led= SPENTO;
				Statusled= SPENTO;
			}
			else {
				//accendo led e riavvio timer
				TR0= 1;
				Led= ACCESO;
				Statusled= ACCESO;
			}
		}
		else {
			TR0= 1;
			Led= ACCESO;
			Statusled= ACCESO;
			modalita= 0;
		}
	}
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