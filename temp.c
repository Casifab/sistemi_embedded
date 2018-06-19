#include <c8051f020.h>

#define LCD 0x3E
#define TER 0x48
#define WRITE 0xFE
#define READ 0x01
#define COM 0x00
#define DAT 0x40

sbit Led= P0^6;

unsigned char cmd_lcd[8]= {0x38,0x39,0x14,0x74,0x54,0x6F,0x0C,0x01};
unsigned char tempWrite[4]= {0x41, 0x41, 0xDF, 0x43};
//unsigned char tempWrite[4]= {0x41,0x41,0x41,0x41};

unsigned char temp[2];
unsigned int tempFinal= 0;
unsigned int valRead= 0;

bit SM_Busy = 0;
unsigned char Slave = 0;
unsigned char SM_Mode = 0;
unsigned char *DataWrite;
unsigned char *DataRead;
unsigned short DataLen = 0;
//unsigned char enter= 0xC0;

void init(void) {
    WDTCN = 0xde;   // disable watchdog timer
    WDTCN = 0xad;
    //OSCICN |= 0x03; // Set internal oscillator to highest setting
                    // (16 MHz)
    XBR0 = 0x05;    // Route SMBus to GPIO pins through crossbar
    XBR2 = 0x40;    // Enable crossbar and weak pull-ups
    SMB0CN = 0x44;  // Enable SMBus with ACKs on acknowledge
                    // cycle
    SMB0CR = -80;   // SMBus clock rate = 100kHz.
    EIE1 |= 2;      // SMBus interrupt enable
    EA = 1;         // Global interrupt enable
    SM_Busy = 0;    // Free SMBus for first transfer
    
    P0MDOUT= 0X040;
    Led= 1;
}

void SM_Send(unsigned char chip_select, unsigned char *src, unsigned short len, unsigned char mode) {
    while (SM_Busy);
    SM_Busy = 1;
    SMB0CN = 0x44;
    Slave = (chip_select << 1) & WRITE;
    SM_Mode = mode;
    DataWrite = src;
    DataLen = len;
    STO = 0;
    STA = 1;
}

void SM_Receive(unsigned char chip_select, unsigned char *dest, unsigned short len) {
    while(SM_Busy);
    SM_Busy = 1;
    SMB0CN = 0x44;
    Slave = (chip_select << 1) | READ;
    DataRead = dest;
    DataLen = len;
    STO = 0;
    STA = 1;
    while(SM_Busy);
}

void SW_routine(void) interrupt 7 {
    switch(SMB0STA) {

        case 0x08:
            SMB0DAT= Slave;
            STA= 0;
            break;

        case 0x18:
            if(SM_Mode == COM || SM_Mode == DAT) {
                SMB0DAT= SM_Mode;
            }
            else {
                SMB0DAT= *DataWrite;
                DataWrite++;
                DataLen--;
            }
            break;

        case 0x28:
            if(DataLen > 0) {
                SMB0DAT= *DataWrite;
                DataWrite++;
                DataLen--;
            }
            else {
                STO= 1;
                SM_Busy= 0;
            }
            break;

        case 0x40:
            STA= 0;
            break;

        case 0x50:
            if(DataLen > 0) {
                *DataRead= SMB0DAT;
                DataRead++;
                DataLen--;
            }
            else {
                AA= 0;
            }
            break;

        default:
            STO= 1;
            SM_Busy= 0;
    }
    SI= 0;
}

void ShiftTemp(void) {
    int tt= (int)temp[0];
    int t2= (int)temp[1];
    tt= tt << 8;
    tt= tt & 0xFF00;
    t2= t2 & 0x00FF;
    tt= tt | t2;
    tt= tt >> 7;
    tempFinal= tt;
}

void Divide(void) {
    int dec= tempFinal/10;
    int unit= tempFinal%10;
    tempWrite[0]= dec | 0x30;
    tempWrite[1]= unit | 0x30;
}

void main(void) {
    init();

    while(1) {
        SM_Receive(TER, temp, 2);
        ShiftTemp();
        Divide();
        SM_Send(LCD, cmd_lcd, 8, COM);
        SM_Send(LCD, tempWrite, 4, DAT);
    }
}