#ifndef __TIMER_H
#define __TIMER_H

/* init_timer.c */
extern uint32_t init_timer(uint8_t, uint32_t);
extern void enable_timer(uint8_t);
extern void disable_timer(uint8_t);
extern void reset_timer(uint8_t);
extern uint32_t init_timer_PWM(uint8_t, uint32_t, uint32_t);

/* IRQ_timer.c */
extern void TIMER0_IRQHandler (void);
extern void TIMER1_IRQHandler (void);
extern void TIMER2_IRQHandler (void);
extern void TIMER3_IRQHandler (void);

#endif