/*
 * pir.c
 *
 *  Created on: Nov 6, 2024
 *      Author: Hayam
 */
#include"pir.h"
#include "../../std_types.h"
#include "../../MCAL/GPIO/gpio.h"


/*Initializes the PIR sensor pin direction.*/
void PIR_init(void)
{
	GPIO_setupPinDirection(PIR_SENSOR_PORT_ID,  PIR_SENSOR_PIN_ID, PIN_INPUT);
}

/*Reads the value from the PIR sensor and returns it.*/
uint8 PIR_getState(void)
{
	return (GPIO_readPin(PIR_SENSOR_PORT_ID,  PIR_SENSOR_PIN_ID));
}

