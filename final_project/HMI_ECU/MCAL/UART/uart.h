 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "../../std_types.h"
/******************************************************************************/
/* Define data types */
typedef enum {
    _5_DATA_BITS = 5,
    _6_DATA_BITS,
    _7_DATA_BITS,
    _8_DATA_BITS,
    _9_DATA_BITS
} UART_BitDataType;

/* define parity type */
typedef enum {
    NO_PARITY,
    EVEN_PARITY,
    ODD_PARITY
} UART_ParityType;

/* number of stop bits: 1 or 2*/
typedef enum {
    ONE_STOP_BIT,
    TWO_STOP_BITS
} UART_StopBitType;

typedef uint32 UART_BaudRateType;

typedef struct {
    UART_BitDataType bit_data;
    UART_ParityType parity;
    UART_StopBitType stop_bit;
    UART_BaudRateType baud_rate;
} UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
