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

void SMBUS_ISR(void) interrupt 7 {
    switch (SMB0STA) {
    
        case SMB_START:
        
        case SMB_RP_START:
            SMB0DAT = Slave;
            break;
        
        case SMB_MTADDACK:
            STA = 0;
            if (SM_Mode == COM || SM_Mode == DAT) {
                SMB0DAT = SM_Mode;
            }
            else {
                SMB0DAT = *DataWrite;
                DataWrite++;
                DataLen--;
            }
            break;
        
        case SMB_MTDBACK:
            if (SM_Mode == ACC_READ) {
                Slave |= READ;
                DataLen = 3;
                STA = 1;
            }
            else if (DataLen) {
                SMB0DAT = *DataWrite;
                DataWrite++;
                DataLen--;
            }
            else {
                STO = 1;
                SM_Busy = 0;
            }
            break;

        case SMB_MRADDACK:
            STA = 0;
            break;
        
        case SMB_MRDBACK:
            if (DataLen) {
                *DataRead = SMB0DAT;
                DataRead++;
                DataLen--;
            }
            else {
                AA = 0;
            }
            break;
        
        default:
            STO = 1;
            SM_Busy = 0;
            break;
    }
    SI = 0;
}