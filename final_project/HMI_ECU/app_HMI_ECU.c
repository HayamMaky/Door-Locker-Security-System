/*
 * app_HMI_ECU.c
 *
 *  Created on: Oct 31, 2024
 *      Author: Hayam
 */
#include"HAL/Keypad/keypad.h"
#include"HAL/LCD/lcd.h"
#include <util/delay.h>
#include"MCAL/UART/uart.h"
#include"MCAL/Timer/timer.h"
#include <avr/interrupt.h>
/**********************************************************/
#define PASSWORD_LENGTH      5
#define MAX_ATTEMPTS         3
#define CORRECT_PASS         1
#define WRONG_PASS           0
#define PASS_LOADING         1
#define PASS_IN              2
#define PASS_UPDATE          3
#define ALARM_ON             4
#define PEOPLE_ON            1
#define PEOPLE_OFF           0

/**********************************************************/
uint8 pass_1[PASSWORD_LENGTH];
uint8 pass_2[PASSWORD_LENGTH];
uint8 seconds;
uint8 counter=0;
uint8 pass_state=0;
uint8 option = 0;
uint8 enter_fail = 0;
uint8 update_fail = 0;
uint8 people_state = 0;
/**********************************************************/
void timer_handler(void);
/**********************************************************/
int main()
{

	LCD_init();

	/**************************************************/
	UART_ConfigType uart_seting={_8_DATA_BITS,NO_PARITY,ONE_STOP_BIT,9600};
	UART_init(&uart_seting);
	/**************************************************/
	sei();
	Timer_ConfigType time1_setting = {0, 31250, TIMER1, F_CPU_256, COMPARE_MODE};
	Timer_init(&time1_setting);
	Timer_setCallBack(timer_handler, TIMER1);
	/* Create a System Password first time */

	while(pass_state != CORRECT_PASS)
	{
		/* receive the password from the user*/
		UART_sendByte(PASS_LOADING);
		LCD_clearScreen();
		LCD_displayString("Plz Enter Pass:");
		LCD_moveCursor(1, 0);
		for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
		{
			pass_1[counter] = KEYPAD_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(500);
		}
		while(KEYPAD_getPressedKey() != 'e');
		_delay_ms(500);
		LCD_clearScreen();
		LCD_displayString("Plz re-enter the");
		LCD_displayStringRowColumn(1, 0, "same pass: ");
		for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
		{
			pass_2[counter] = KEYPAD_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(500);
		}
		while(KEYPAD_getPressedKey() != 'e');
		_delay_ms(500);
		/* send the two passwords to the control ecu */
		for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
		{
			UART_sendByte(pass_1[counter]);
		}
		for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
		{
			UART_sendByte(pass_2[counter]);
		}
		pass_state = UART_recieveByte();
		if(pass_state == WRONG_PASS)
		{
			LCD_clearScreen();
		}
	}

	while(1)
	{
		LCD_clearScreen();
		LCD_displayString("+ : OPEN DOOR");
		LCD_displayStringRowColumn(1, 0, "- : CHANGE PASS");
		do{
			option = KEYPAD_getPressedKey();
			_delay_ms(500);
		}while(option != '+' && option != '-');
		if(option == '+')
		{
			UART_sendByte(PASS_IN);
			LCD_clearScreen();
			LCD_displayString("Plz enter old");
			LCD_displayStringRowColumn(1, 0, "pass: ");
			for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
			{
				pass_1[counter] = KEYPAD_getPressedKey();
				LCD_displayCharacter('*');
				_delay_ms(500);
			}
			while(KEYPAD_getPressedKey() != 'e');
			_delay_ms(500);

			for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
			{
				UART_sendByte(pass_1[counter]);
			}
			pass_state = UART_recieveByte();
			if(pass_state == CORRECT_PASS)
			{
				enter_fail = 0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 1, "Door Unlocking");
				LCD_displayStringRowColumn(1, 3, "Please Wait");
				seconds = 0;
				while(seconds < 15);
				LCD_clearScreen();
				people_state = UART_recieveByte();
				while(people_state == PEOPLE_ON)
				{
					LCD_displayStringRowColumn(0, 0, "Wait for People");
					LCD_displayStringRowColumn(1, 3, "to Enter");
					people_state = UART_recieveByte();
				}
				if(people_state == PEOPLE_OFF)
				{
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 2, "Door Locking");
					seconds = 0;
					while(seconds < 15);
				}
			}
			else if(pass_state == WRONG_PASS)
			{
				++enter_fail;
				if(enter_fail == 3)
				{
					UART_sendByte(ALARM_ON);
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 1, "System LOCKED");
					LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
					seconds = 0;
					while(seconds < 60);
					enter_fail = 0;
				}
				else
				{
					UART_sendByte(PASS_IN);
					LCD_clearScreen();
					LCD_displayString("Plz enter old");
					LCD_displayStringRowColumn(1, 0, "pass: ");
					for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
					{
						pass_1[counter] = KEYPAD_getPressedKey();
						LCD_displayCharacter('*');
						_delay_ms(500);
					}
					while(KEYPAD_getPressedKey() != 'e');
					_delay_ms(500);
					for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
					{
						UART_sendByte(pass_1[counter]);
					}
					pass_state = UART_recieveByte();
					if(pass_state == CORRECT_PASS)
					{
						enter_fail = 0;
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 1, "Door Unlocking");
						LCD_displayStringRowColumn(1, 3, "Please Wait");
						seconds = 0;
						while(seconds < 15);
						LCD_clearScreen();
						people_state = UART_recieveByte();
						while(people_state == PEOPLE_ON)
						{
							LCD_displayStringRowColumn(0, 0, "Wait for People");
					        LCD_displayStringRowColumn(1, 3, "to Enter");
							people_state = UART_recieveByte();
						}
						if(people_state == PEOPLE_OFF)
						{
							LCD_clearScreen();
							LCD_displayStringRowColumn(0, 2, "Door Locking");
							seconds = 0;
							while(seconds < 15);
						}
					}
					else if(pass_state == WRONG_PASS)
					{
						++enter_fail;
						if(enter_fail == 3)
						{
							UART_sendByte(ALARM_ON);
							LCD_clearScreen();
							LCD_displayStringRowColumn(0, 1, "System LOCKED");
							LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
							seconds = 0;
							while(seconds < 60);
							enter_fail = 0;
						}
						else
						{
							UART_sendByte(PASS_IN);
							LCD_clearScreen();
							LCD_displayString("Plz enter old");
							LCD_displayStringRowColumn(1, 0, "pass: ");
							for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
							{
								pass_1[counter] = KEYPAD_getPressedKey();
								LCD_displayCharacter('*');
								_delay_ms(500);
							}
							while(KEYPAD_getPressedKey() != 'e');
							_delay_ms(500);
							for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
							{
								UART_sendByte(pass_1[counter]);
							}
							pass_state = UART_recieveByte();
							if(pass_state == CORRECT_PASS)
							{
								enter_fail = 0;
								LCD_clearScreen();
								LCD_displayStringRowColumn(0, 1, "Door Unlocking");
								LCD_displayStringRowColumn(1, 3, "Please Wait");
								seconds = 0;
								while(seconds < 15);
								LCD_clearScreen();
								people_state = UART_recieveByte();
								while(people_state == PEOPLE_ON)
								{
									LCD_displayStringRowColumn(0, 0, "Wait for People");
									LCD_displayStringRowColumn(1, 3, "to Enter");
									people_state = UART_recieveByte();
								}
								if(people_state == PEOPLE_OFF)
								{
									LCD_clearScreen();
									LCD_displayStringRowColumn(0, 2, "Door Locking");
									seconds = 0;
									while(seconds < 15);
								}
							}
							else if(pass_state == WRONG_PASS)
							{
								++enter_fail;
								if(enter_fail == 3)
								{
									UART_sendByte(ALARM_ON);
									LCD_clearScreen();
									LCD_displayStringRowColumn(0, 1, "System LOCKED");
									LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
									seconds = 0;
									while(seconds < 60);
									enter_fail = 0;
								}
								else
								{
									break;
								}
							}
						}
					}
				}
			}
		}
		else if(option == '-')
		{
			UART_sendByte(PASS_UPDATE);
			LCD_clearScreen();
			LCD_displayString("Plz enter old");
			LCD_displayStringRowColumn(1, 0, "pass: ");
			for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
			{
				pass_1[counter] = KEYPAD_getPressedKey();
				LCD_displayCharacter('*');
				_delay_ms(500);
			}
			while(KEYPAD_getPressedKey() != 'e');
			_delay_ms(500);
			for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
			{
				UART_sendByte(pass_1[counter]);
			}
			pass_state = UART_recieveByte();
			if(pass_state == CORRECT_PASS)
			{
				update_fail = 0;
				do
				{
					/* receive the password from the user*/
					UART_sendByte(PASS_LOADING);
					LCD_clearScreen();
					LCD_displayString("Plz Enter Pass:");
					LCD_moveCursor(1, 0);
					for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
					{
						pass_1[counter] = KEYPAD_getPressedKey();
						LCD_displayCharacter('*');
						_delay_ms(500);
					}
					while(KEYPAD_getPressedKey() != 'e');
					_delay_ms(500);
					LCD_clearScreen();
					LCD_displayString("Plz re-enter the");
					LCD_displayStringRowColumn(1, 0, "same pass: ");
					for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
					{
						pass_2[counter] = KEYPAD_getPressedKey();
						LCD_displayCharacter('*');
						_delay_ms(500);
					}
					while(KEYPAD_getPressedKey() != 'e');
					_delay_ms(500);
					/* send the two passwords to the control ecu */
					for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
					{
						UART_sendByte(pass_1[counter]);
					}
					for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
					{
						UART_sendByte(pass_2[counter]);
					}
					pass_state = UART_recieveByte();
					if(pass_state == CORRECT_PASS)
					{
						LCD_clearScreen();
					}
					}while(pass_state != CORRECT_PASS);
			}
			else if(pass_state == WRONG_PASS)
			{
				++update_fail;
				if(update_fail == 3)
				{
					UART_sendByte(ALARM_ON);
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 1, "System LOCKED");
					LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
					seconds = 0;
					while(seconds < 60);
					update_fail = 0;
				}
				else
				{
					UART_sendByte(PASS_UPDATE);
					LCD_clearScreen();
					LCD_displayString("Plz enter old");
					LCD_displayStringRowColumn(1, 0, "pass: ");
					for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
					{
						pass_1[counter] = KEYPAD_getPressedKey();
						LCD_displayCharacter('*');
						_delay_ms(500);
					}
					while(KEYPAD_getPressedKey() != 'e');
					_delay_ms(500);
					for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
					{
						UART_sendByte(pass_1[counter]);
					}
					pass_state = UART_recieveByte();
					if(pass_state == CORRECT_PASS)
					{
						update_fail = 0;
						do
						{
							/* receive the password from the user*/
							UART_sendByte(PASS_LOADING);
							LCD_clearScreen();
							LCD_displayString("Plz Enter Pass:");
							LCD_moveCursor(1, 0);
							for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
							{
								pass_1[counter] = KEYPAD_getPressedKey();
								LCD_displayCharacter('*');
								_delay_ms(500);
							}
							while(KEYPAD_getPressedKey() != 'e');
							_delay_ms(500);
							LCD_clearScreen();
							LCD_displayString("Plz re-enter the");
							LCD_displayStringRowColumn(1, 0, "same pass: ");
							for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
							{
								pass_2[counter] = KEYPAD_getPressedKey();
								LCD_displayCharacter('*');
								_delay_ms(500);
							}
							while(KEYPAD_getPressedKey() != 'e');
							_delay_ms(500);
							/* send the two passwords to the control ecu */
							for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
							{
								UART_sendByte(pass_1[counter]);
							}
							for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
							{
								UART_sendByte(pass_2[counter]);
							}
							pass_state = UART_recieveByte();
							if(pass_state == CORRECT_PASS)
							{
								LCD_clearScreen();
							}
							}while(pass_state != CORRECT_PASS);
					}
					else if(pass_state == WRONG_PASS)
					{
						++update_fail;
						if(update_fail == 3)
						{
							UART_sendByte(ALARM_ON);
							LCD_clearScreen();
						    LCD_displayStringRowColumn(0, 1, "System LOCKED");
							LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
							seconds = 0;
							while(seconds < 60);
						    update_fail = 0;
						}
						else
						{
							UART_sendByte(PASS_UPDATE);
							LCD_clearScreen();
							LCD_displayString("Plz enter old");
							LCD_displayStringRowColumn(1, 0, "pass: ");
							for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
							{
								pass_1[counter] = KEYPAD_getPressedKey();
								LCD_displayCharacter('*');
								_delay_ms(500);
							}
							while(KEYPAD_getPressedKey() != 'e');
							_delay_ms(500);
							for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
							{
								UART_sendByte(pass_1[counter]);
							}
							pass_state = UART_recieveByte();
							if(pass_state == CORRECT_PASS)
							{
								update_fail = 0;
								do
								{
									/* receive the password from the user*/
									UART_sendByte(PASS_LOADING);
									LCD_clearScreen();
									LCD_displayString("Plz Enter Pass:");
									LCD_moveCursor(1, 0);
									for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
									{
										pass_1[counter] = KEYPAD_getPressedKey();
										LCD_displayCharacter('*');
										_delay_ms(500);
									}
									while(KEYPAD_getPressedKey() != 'e');
									_delay_ms(500);
									LCD_clearScreen();
									LCD_displayString("Plz re-enter the");
									LCD_displayStringRowColumn(1, 0, "same pass: ");
									for(counter = 0; counter < PASSWORD_LENGTH; ++counter)
									{
										pass_2[counter] = KEYPAD_getPressedKey();
										LCD_displayCharacter('*');
										_delay_ms(500);
									}
									while(KEYPAD_getPressedKey() != 'e');
									_delay_ms(500);
									/* send the two passwords to the control ecu */
									for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
									{
										UART_sendByte(pass_1[counter]);
									}
									for(counter= 0; counter < PASSWORD_LENGTH; ++counter)
									{
										UART_sendByte(pass_2[counter]);
									}
									pass_state = UART_recieveByte();
									if(pass_state == CORRECT_PASS)
									{
										LCD_clearScreen();
									}
								}while(pass_state != CORRECT_PASS);
							}
							else if(pass_state == WRONG_PASS)
							{
								++update_fail;
								if(update_fail == 3)
								{
									UART_sendByte(ALARM_ON);
									LCD_clearScreen();
									LCD_displayStringRowColumn(0, 1, "System LOCKED");
								    LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
									seconds = 0;
									while(seconds < 60);
									enter_fail = 0;
								}
								else
								{
									break;
								}

							}
						}
					}
				}
			}
		}
	}
}






/**************************************************************************/
void timer_handler(void)
{
	seconds++;
}





