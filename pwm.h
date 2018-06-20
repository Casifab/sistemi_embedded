#ifndef PWM_H
#define PWM_H

#define ON 1
#define OFF 0

void initp(void);

void timer0(void);

void interrupt_timer0();

void timer2(unsigned int counts);

void interrupt_timer2(void);

void init_button(void);

void click_button(void);

void pwm();

void pwmMain(void);

#endif
