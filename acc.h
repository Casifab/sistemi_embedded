#ifndef ACC_H
#define ACC_H

#define ACCEL 0x4C

extern unsigned char acc_line[14];

void read_angles(void);

void med_angles(void);

void compose_line(void);

void accMain(void);

#endif
