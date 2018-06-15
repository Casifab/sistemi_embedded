#ifndef SMBUS_H
#define SMBUS_H

#define WRITE 0xFE
#define READ 0x01
#define COM 0x00
#define DAT 0x40

void SM_Send(unsigned char chip_select, unsigned char *src, unsigned short len, unsigned char mode);

void SM_Receive(unsigned char chip_select, unsigned char *dest, unsigned short len);

void SW_routine(void);

#endif