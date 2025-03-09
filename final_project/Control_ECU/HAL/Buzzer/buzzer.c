/*
 * buzzer.c
 *
 *  Created on: Oct 8, 2024
 *      Author: Hayam
 */

#include "../../MCAL/GPIO/gpio.h"
#include "../../std_types.h"
#include "buzzer.h"


/*Initializes the buzzer pin direction and turn off the buzzer.*/
void Buzzer_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID , PIN_OUTPUT);
	Buzzer_off();
}
/* Activates the buzzer. */
void Buzzer_off(void)
{
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID, LOGIC_LOW);
}
/*Deactivates the buzzer.*/
void Buzzer_on(void)
{
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID, LOGIC_HIGH);
}
