#ifndef PWM_H
#define PWM_H
#define ACCESO 1
#define SPENTO 0

void init(void);

void timer0(void);

void interrupt_timer0();

void init_button(void);

void pwm();

void main(void);

//Definizioni di pwm.c

#endif
