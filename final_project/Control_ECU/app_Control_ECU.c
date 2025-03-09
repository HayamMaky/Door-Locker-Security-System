/*
 * app.c
 *
 *  Created on: Nov 7, 2024
 *      Author: Hayam
 */
#include"HAL/PIR/pir.h"
#include"HAL/Motor/dc_motor.h"
#include"HAL/Buzzer/buzzer.h"
#include"HAL/EEPROM/external_eeprom.h"
#include"MCAL/UART/uart.h"
#include <avr/interrupt.h>
#include"MCAL/Timer/timer.h"
#include"MCAL/I2C/twi.h"
#include <util/delay.h>
/***********************************************************/
#define PASSWORD_LENGTH      5
#define UNLOCK_DURATION      15  /* Seconds for motor rotation*/
#define BUZZER_DURATION      60  /* 1 minute in seconds*/
#define CORRECT_PASS         1
#define WRONG_PASS           0
#define PASS_LOADING         1
#define PASS_IN              2
#define PASS_UPDATE          3
#define ALARM_ON             4
#define start_address        0x0005
#define PEOPLE_ON            1
#define PEOPLE_OFF           0

/************************************************************/
uint8 seconds;
uint8 pass_1[PASSWORD_LENGTH]={0};
uint8 pass_2[PASSWORD_LENGTH]={0};
uint8 seconds;
uint8 counter=0;
uint8 pass_state=0;
uint8 state = 0;
uint8 flag = 1;
uint8 status =0;
uint8 peopleIN=0;
/************************************************************/
void timer_handler(void);
/************************************************************/
int main()
{
	UART_ConfigType uart_seting={_8_DATA_BITS,NO_PARITY,ONE_STOP_BIT,9600};
	UART_init(&uart_seting);
	/********************************************************/
	Buzzer_init();
	PIR_init();
	/********************************************************/
	TWI_ConfigType i2c_cfg = {0b00000001, 400000};
	TWI_init(&i2c_cfg);
	/********************************************************/
	DcMotor_Init();

	/********************************************************/
	sei();
	Timer_ConfigType time1_setting = {0, 31250, TIMER1, F_CPU_256, COMPARE_MODE};
	Timer_init(&time1_setting);
	Timer_setCallBack(timer_handler, TIMER1);
	/*********************************************************/

	while(1)
	{
		state = UART_recieveByte();
		if(state == PASS_LOADING)
		{
			for(counter = 0; counter< PASSWORD_LENGTH; ++counter)
			{
				pass_1[counter] = UART_recieveByte();
			}
			for(counter = 0; counter< PASSWORD_LENGTH; ++counter)
			{
				pass_2[counter] = UART_recieveByte();
			}
			for(counter = 0; counter< PASSWORD_LENGTH; ++counter)
			{
				if(pass_1[counter] != pass_2[counter])
				{
					UART_sendByte(WRONG_PASS);
					break;
				}
			}
			if(counter==PASSWORD_LENGTH)
			{
				UART_sendByte(CORRECT_PASS);
				for(counter =0;counter<PASSWORD_LENGTH; ++counter)
				{
					status =EEPROM_writeByte(start_address + counter, pass_1[counter]);
					_delay_ms(20);
				}
			}

		}
		else if(state == PASS_IN)
		{
			for(counter = 0;counter< PASSWORD_LENGTH; ++counter)
			{
				pass_1[counter] = UART_recieveByte();
			}
			for(counter = 0;counter< PASSWORD_LENGTH; ++counter)
			{
				status = EEPROM_readByte(start_address + counter, &pass_2[counter]);
				_delay_ms(20);
			}
			for(counter = 0;counter< PASSWORD_LENGTH; ++counter)
			{
				if(pass_1[counter] != pass_2[counter])
				{
					UART_sendByte(WRONG_PASS);
					break;
				}
			}
			if(counter == PASSWORD_LENGTH)
			{
				UART_sendByte(CORRECT_PASS);
				DcMotor_Rotate(CW, 100);
				seconds = 0;
				while(seconds < UNLOCK_DURATION);
				DcMotor_Rotate(Stop, 0);
				do{
					peopleIN = PIR_getState();
					UART_sendByte(PEOPLE_ON);
				 }while(peopleIN);
				UART_sendByte(PEOPLE_OFF);
				DcMotor_Rotate(A_CW, 100);
				seconds = 0;
				while(seconds < UNLOCK_DURATION);
				DcMotor_Rotate(Stop, 0);
			}
		}
		else if(state ==  PASS_UPDATE )
		{
			for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
			{
				pass_1[counter] = UART_recieveByte();
			}
			for(counter = 0;counter< PASSWORD_LENGTH; ++counter)
			{
				status = EEPROM_readByte(start_address + counter , &pass_2[counter]);
				_delay_ms(10);
			}
			for(counter= 0; counter< PASSWORD_LENGTH; ++counter)
			{
				if(pass_1[counter] != pass_2[counter])
				{
					UART_sendByte(WRONG_PASS);
					break;
				}
			}
			if(counter == PASSWORD_LENGTH)
			{
				UART_sendByte(CORRECT_PASS);
				for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
				{
					pass_1[counter] = UART_recieveByte();
				}
				for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
				{
					pass_2[counter] = UART_recieveByte();
				}
				for(counter= 0; counter< PASSWORD_LENGTH; ++counter)
				{
					if(pass_1[counter] != pass_2[counter])
					{
						UART_sendByte(WRONG_PASS);
						break;
					}
				}
				if(counter ==PASSWORD_LENGTH )
				{
					UART_sendByte(CORRECT_PASS);
					for(counter= 0; counter< PASSWORD_LENGTH; ++counter)
					{
						status =EEPROM_writeByte(start_address + counter, pass_1[counter]);
						_delay_ms(10);
					}
				}
			}
		}
		else if (state == ALARM_ON)
		{
			Buzzer_on();
			seconds = 0;
			while(seconds < BUZZER_DURATION);
			Buzzer_off();
		}



	}
}
void timer_handler(void)
{
	seconds++;
}
