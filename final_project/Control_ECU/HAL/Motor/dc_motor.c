/*
 * dc_motor.c
 *
 *  Created on: Oct 8, 2024
 *      Author: Hayam
 */

#include "dc_motor.h"
#include "../../MCAL/GPIO/gpio.h"
#include "../../std_types.h"
#include "../../MCAL/PWM/pwm_timer0.h"


/* Initializes the DC motor by setting the direction for the motor pins and stopping the
motor at the beginning.*/
void DcMotor_Init(void)
{
	/*setting the direction for the motor pins*/
	GPIO_setupPinDirection(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_ENABLE_PORT_ID, MOTOR_ENABLE_PIN_ID, PIN_OUTPUT);

	/*stopping the motor at the beginning*/
	GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_ENABLE_PORT_ID, MOTOR_ENABLE_PIN_ID, LOGIC_HIGH);
}

/*Controls the motor's state (Clockwise/Anti-Clockwise/Stop) and adjusts the speed based
on the input duty cycle. */
void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{
	switch(state)
	{
	case CW:
		GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	case A_CW:
		GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_HIGH);
		break;
	case Stop:
		GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	}
	 /*Motor speed in percentage (0 to 100%)*/
	PWM_Timer0_Start(speed);

}

