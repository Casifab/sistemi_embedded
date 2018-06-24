#ifndef TEMP_H
#define TEMP_H

//this?
#define TER 0x48
//or this?
//#define TER 0x90

extern unsigned char temp[2];
extern unsigned int tempFinal;
extern unsigned char tempToWrite[4];

void ShiftTemp(void);

void Divide(void);

void tempMain(void);

#endif
