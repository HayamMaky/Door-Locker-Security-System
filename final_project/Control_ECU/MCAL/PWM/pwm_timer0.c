/*
 * pwm_timer0.c
 *
 *  Created on: Oct 7, 2024
 *      Author: Hayam
 */

# include <avr/io.h>
# include "../../MCAL/GPIO/gpio.h"
# include "pwm_timer0.h"
# include "../../common_macros.h"

void PWM_Timer0_Start(uint8 duty_cycle)
{
 /*Initializes Timer0 in fast PWM mode --> WGM00 = 1, WGM01 = 1 .*/
	SET_BIT(TCCR0,WGM00);
	SET_BIT(TCCR0,WGM01);
/* Prescaler: F_CPU/64--> CS00 = 1, CS01 = 1, CS02 = 0*/
	SET_BIT(TCCR0,CS00);
	SET_BIT(TCCR0,CS01);
 /* Non-inverting mode --> COM00  = 0, COM01 = 1*/
	SET_BIT(TCCR0,COM01);
 /* The function configures OC0 as the output pin. */
	GPIO_setupPinDirection(PORTD_ID,PIN3_ID , PIN_OUTPUT);
/* Duty_cycle: Percentage (0 to 100%) representing the PWM duty cycle.*/
	OCR0 = (duty_cycle * 255) / 100;
}
