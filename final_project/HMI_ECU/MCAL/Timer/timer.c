/*
 * timer.c
 *
 *  Created on: Nov 6, 2024
 *      Author: Hayam
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"  // Assuming you create a header file named Timer.h
#include <stdio.h>

/* Global pointers to hold callback functions for each timer */
static void (*g_Timer0_CallBack)(void) = NULL;
static void (*g_Timer1_CallBack)(void) = NULL;
static void (*g_Timer2_CallBack)(void) = NULL;

/* Function to initialize the Timer*/
void Timer_init(const Timer_ConfigType * Config_Ptr) {
    switch (Config_Ptr->timer_ID) {
        case TIMER0:
            /* Set Timer0 mode (Normal or Compare)*/
        	TCCR0=0;
            if (Config_Ptr->timer_mode == NORMAL_MODE) {
                TCCR0 |= (1 << FOC0);  /* no PWM*/
                TIMSK |= (1 << TOIE0); /*Enable overflow interrupt forTimer0 */
            } else if (Config_Ptr->timer_mode == COMPARE_MODE) {
            	TCCR0 |= (1 << FOC0);  /* no PWM*/
                TCCR0 |= (1 << WGM01) ;  /* CTC mode */
                OCR0 = Config_Ptr->timer_compare_MatchValue;  /* Set Compare match value */
                TIMSK |= (1 << OCIE0); /*Enable compare interrupt forTimer0 */
            }
            TCNT0 = Config_Ptr->timer_InitialValue;  /* Set initial value */
            TCCR0 |= Config_Ptr->timer_clock;        /* Set clock/prescaler*/
            break;

        case TIMER1:
        	/* Set Timer1 mode (Normal or Compare)*/
        	TCCR1A=0;
        	TCCR1B=0;
            if (Config_Ptr->timer_mode == NORMAL_MODE) {
                TCCR1A |= (1 << FOC1A) | (1 << FOC1B);  /*no PWM*/
                /* Normal mode:WGM10 = 0,WGM11 = 0,WGM12 = 0,WGM13 = 0 */
                TIMSK |= (1 << TOIE1); /*Enable overflow interrupt forTimer1 */
            } else if (Config_Ptr->timer_mode == COMPARE_MODE) {
            	TCCR1A |= (1 << FOC1A) | (1 << FOC1B);  /* no PWM*/
            	/* CTC mode:WGM10 = 0,WGM11 = 0,WGM12 = 1,WGM13 = 0 */
                TCCR1B |= (1 << WGM12);
                OCR1A = Config_Ptr->timer_compare_MatchValue;  /* Set Compare match value */
                TIMSK |= (1 <<OCIE1A); /*Enable compare interrupt forTimer1 */
            }
            TCNT1 = Config_Ptr->timer_InitialValue;  /*Set initial value*/
            TCCR1B |= Config_Ptr->timer_clock;       /*Set clock/prescaler*/
            break;

        case TIMER2:
            /* Set Timer2 mode (Normal or Compare)*/
        	TCCR2=0;
            if (Config_Ptr->timer_mode == NORMAL_MODE) {
                TCCR2 = (1 << FOC2);  /*no PWM*/
            } else if (Config_Ptr->timer_mode == COMPARE_MODE) {
                TCCR2 |= (1 << WGM21) | (1 << FOC2);  /* CTC mode & no PWM*/
                OCR2 = Config_Ptr->timer_compare_MatchValue;  /* Set Compare match value*/
                TIMSK |= (1 << OCIE2); /*Enable compare interrupt forTimer2 */
            }
            TCNT2 = Config_Ptr->timer_InitialValue;  /*Set initial value*/
            TCCR2 |= Config_Ptr->timer_clock;        /* Set clock/prescaler*/
            TIMSK |= (1 << TOIE2); /*Enable overflow interrupt forTimer2 */
            break;

        default:
            /*Invalid Timer ID*/
            break;
    }
}

/* Function to deinitialize the Timer*/
void Timer_deInit(Timer_ID_Type timer_type) {
    switch (timer_type) {
        case TIMER0:
            TCCR0 = 0;  /* Stop Timer0 */
            TIMSK &= ~(1 << TOIE0) & ~(1 << OCIE0);  /* Disable Timer0 interrupts */
            break;

        case TIMER1:
            TCCR1A = 0;
            TCCR1B = 0;  /* Stop Timer1 */
            TIMSK &= ~(1 << TOIE1) & ~(1 << OCIE1A);  /* Disable Timer1 interrupts*/
            break;

        case TIMER2:
            TCCR2 = 0;  /* Stop Timer2 */
            TIMSK &= ~(1 << TOIE2) & ~(1 << OCIE2);  /* Disable Timer2 interrupts*/
            break;

        default:
            /* Invalid Timer ID*/
            break;
    }
}

/*Function to set the callback function*/
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID) {
    switch (a_timer_ID) {
        case TIMER0:
            g_Timer0_CallBack = a_ptr;
            break;

        case TIMER1:
            g_Timer1_CallBack = a_ptr;
            break;

        case TIMER2:
            g_Timer2_CallBack = a_ptr;
            break;

        default:
            // Invalid Timer ID
            break;
    }
}

/* Timer0 Overflow ISR*/
ISR(TIMER0_OVF_vect) {
    if (g_Timer0_CallBack != NULL) {
        g_Timer0_CallBack();
    }
}

/* Timer0 Compare Match ISR*/
ISR(TIMER0_COMP_vect) {
    if (g_Timer0_CallBack != NULL) {
        g_Timer0_CallBack();
    }
}

/* Timer1 Overflow ISR*/
ISR(TIMER1_OVF_vect) {
    if (g_Timer1_CallBack != NULL) {
        g_Timer1_CallBack();
    }
}

/* Timer1 Compare Match ISR */
ISR(TIMER1_COMPA_vect) {
    if (g_Timer1_CallBack != NULL) {
        g_Timer1_CallBack();
    }
}

/* Timer2 Overflow ISR*/
ISR(TIMER2_OVF_vect) {
    if (g_Timer2_CallBack != NULL) {
        g_Timer2_CallBack();
    }
}

/* Timer2 Compare Match ISR*/
ISR(TIMER2_COMP_vect) {
    if (g_Timer2_CallBack != NULL) {
        g_Timer2_CallBack();
    }
}


