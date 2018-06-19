#ifndef TEMP_H
#define TEMP_H

#define LCD 0x3E
#define TER 0x48

extern unsigned char *cmd_lcd;
extern unsigned char *tempWrite;

extern unsigned char *temp;

void ShiftTemp(void);

void Divide(void);

#endif
