#include "smbus.h"
#include <c8051f020.h>

bit SM_Busy = 0;
unsigned char Slave = 0;
unsigned char SM_Mode = 0;
unsigned char *DataWrite;
unsigned char *DataRead;
unsigned short DataLen = 0;

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
    while (SM_Busy);
    SM_Busy = 1;
    SMB0CN = 0x44;
    Slave = (chip_select << 1) | READ;
    DataRead = dest;
    DataLen = len;
    STO = 0;
    STA = 1;
    while (SM_Busy);
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