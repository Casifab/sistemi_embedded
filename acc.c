#include <c8051f020.h>
#include "acc.h"
#include "smbus.h"

//Angles look up table
code int ALUT[64] = {
    //Angoli positivi
    0, 3, 5, 8, 11, 14, 16, 19, 22, 25, 28,
    31, 34, 38, 41, 45, 49, 53, 58, 63, 70, 80,
    //Maggiori di 80 / Shaken
    90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
    //Minori di -80 / Shaken
    90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
    //Angoli negativi (aggiungere - e controllare conversione)
    80, 70, 63, 58, 53, 49, 45, 41, 38, 34, 31,
    28, 25, 22, 19, 16, 14, 11, 8, 5, 3};

int ang_read[3];
int med[3];
int buff_x[8];
int buff_y[8];
int buff_z[8];
int point= 0;

unsigned char acc_line[14]= {0x58, 0, 0, 0xDF, 0, 0x59, 0, 0, 0xDF, 0, 0x60, 0, 0, 0xDF};

void read_angles(void) {
    int src = 0x00;
	DataRead = ang_read;
	SM_Send(ACCEL, &src, 1, 0x01);
	while(SM_Busy);
	
	ang_read[0] = ALUT[ang_read[0] & 0x3F];
	ang_read[1] = ALUT[ang_read[1] & 0x3F];
	ang_read[2] = ALUT[ang_read[2] & 0x3F];

    if(point > 7) {
        point= 0;
        buff_x[point]= ang_read[0];
        buff_y[point]= ang_read[1];
        buff_z[point]= ang_read[2];
    }
    else {
        buff_x[point] = ang_read[0];
        buff_y[point] = ang_read[1];
        buff_z[point] = ang_read[2];
        point++;
    }
}

void med_angles(void) {
    int ii, sum_x= 0, sum_y= 0, sum_z= 0;
    for(ii= 0; ii < 8; ii++) {
        sum_x = sum_x + buff_x[ii];
        sum_y = sum_y + buff_y[ii];
        sum_z = sum_z + buff_z[ii];
    }

    med[0]= sum_x/8;
    med[1]= sum_y/8;
    med[2]= sum_z/8;
}

void compose_line(void) {
    acc_line[1]= 0x30 + (med[0]/10);
    acc_line[2] = 0x30 + (med[0] % 10);

    acc_line[6] = 0x30 + (med[1] / 10);
    acc_line[7] = 0x30 + (med[1] % 10);

    acc_line[11] = 0x30 + (med[2] / 10);
    acc_line[12] = 0x30 + (med[2] % 10);
}

void accMain(void) {
    read_angles();
    med_angles();
    compose_line();
}