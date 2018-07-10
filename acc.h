#ifndef ACC_H
#define ACC_H

#define ACC 0x4C

//Angles look up table
code int ALUT[] = {
    //Angoli positivi
	  0,   3,   5,   8,  11,  14,  16,  19,  22,  25,  28,
	 31,  34,  38,  41,  45,  49,  53,  58,  63,  70,  80,
    //Maggiori di 80 / Shaken
	 90,  90,  90,  90,  90,  90,  90,  90,  90,  90,
    //Minori di -80 / Shaken
	 90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,
    //Angoli negativi (aggiungere - e controllare conversione)
	 80,  70,  63,  58,  53,  49,  45,  41,  38,  34,  31,
	 28,  25,  22,  19,  16,  14,  11,   8,   5,   3
};

extern int med_x;
extern int med_y;
extern int med_z;




#endif
