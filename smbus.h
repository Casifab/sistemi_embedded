#ifndef SMBUS_H
#define SMBUS_H

#define WRITE 0xFE
#define READ 0x01
#define COM 0x00
#define DAT 0x40

extern bit SM_Busy = 0;
extern unsigned char Slave = 0;
extern unsigned char SM_Mode = 0;
extern unsigned char *DataWrite;
extern unsigned char *DataRead;
extern unsigned short DataLen = 0;

void SM_Send(unsigned char chip_select, unsigned char *src, unsigned short len, unsigned char mode);

void SM_Receive(unsigned char chip_select, unsigned char *dest, unsigned short len);

void SW_routine(void);

#endif