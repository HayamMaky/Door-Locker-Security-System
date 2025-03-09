/*
 * timer.h
 *
 *  Created on: Nov 6, 2024
 *      Author: Hayam
 */

#ifndef TIMER_H_
#define TIMER_H_
/********************************************/
#include <avr/io.h>
/********************************************/
/* Timer ID Type */
typedef enum {
    TIMER0,
    TIMER1,
    TIMER2
} Timer_ID_Type;

/* Timer Clock Type */
typedef enum {
    NO_CLOCK,
    F_CPU_CLOCK,
    F_CPU_8,
    F_CPU_64,
    F_CPU_256,
    F_CPU_1024
} Timer_ClockType;

/* Timer Mode Type */
typedef enum {
    NORMAL_MODE,
    COMPARE_MODE
} Timer_ModeType;

/* Timer Configuration Structure */
typedef struct {
    uint16_t timer_InitialValue;
    uint16_t timer_compare_MatchValue;
    Timer_ID_Type timer_ID;
    Timer_ClockType timer_clock;
    Timer_ModeType timer_mode;
} Timer_ConfigType;

/***********************************************************/

void Timer_init(const Timer_ConfigType * Config_Ptr);
void Timer_deInit(Timer_ID_Type timer_type);
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID);

/*************************************************************/



#endif /* TIMER_H_ */
