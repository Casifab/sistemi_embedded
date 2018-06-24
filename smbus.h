#ifndef SMBUS_H
#define SMBUS_H

#define WRITE 0xFE
#define READ 0x01
#define COM 0x00
#define DAT 0x40

#define ACC_READ 0x01
#define ACC_SEND 0x41

extern bit SM_Busy;
extern unsigned char Slave;
extern unsigned char SM_Mode;
extern unsigned char *DataWrite;
extern unsigned char *DataRead;
extern unsigned short DataLen;

void SM_Send(unsigned char chip_select, unsigned char *src, unsigned short len, unsigned char mode);

void SM_Receive(unsigned char chip_select, unsigned char *dest, unsigned short len);

void SW_routine(void);

#endif