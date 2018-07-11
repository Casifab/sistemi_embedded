#include <c8051f020.h>
#include "temp.h"
#include "smbus.h"

unsigned char temp[2];
unsigned int tempFinal= 0;
unsigned char tempToWrite[4]= {0x00, 0x00, 0xDF, 0x43};

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
    tempToWrite[0]= dec | 0x30;
    tempToWrite[1]= unit | 0x30;
}

void tempMain(void) {
    SM_Receive(TER, temp, 2);
    ShiftTemp();
    Divide();
}