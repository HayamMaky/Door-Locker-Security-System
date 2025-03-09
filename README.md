# Dual Microcontroller-Based Door Locker Security System Using Password Authentication

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Components](#hardware-components)
- [Operation Steps](#operation-steps)
- [System Requirements](#system-requirements)
- [Drivers Requirements](#drivers-requirements)
- [Installation & Usage](#installation--usage)
- [Contributors](#contributors)

---

## Project Overview
This project implements a secure door locker system using two microcontrollers: the HMI_ECU and the Control_ECU. The system utilizes password authentication, stores data in external EEPROM, and integrates additional components including an LCD, a 4x4 keypad, a PIR sensor, an H-bridge motor driver, and a buzzer. Users interact with the system through the LCD and keypad, while the door operation (unlocking and locking) is controlled based on password verification and motion detection.

---

## Features
- **Password Protection**: Users set and verify a 5-digit password stored in external EEPROM.
- **LCD and Keypad Interface**: Facilitates user interaction for entering and managing passwords.
- **UART Communication**: Data exchange between HMI_ECU and Control_ECU.
- **EEPROM Storage**: Secure storage of passwords and system data.
- **Motorized Door Control**: The door is unlocked/locked using an H-bridge driven motor.
- **Buzzer Alert**: Activated for failed password attempts and system alerts.
- **PIR Motion Sensor**: Detects motion to hold the door open.
- **Password Change Option**: Allows users to update the password after verification.
- **Security Lock**: The system locks for one minute after three consecutive incorrect password attempts.

---

## Hardware Components

### HMI_ECU
- **LCD (8-bit mode)**
  - RS: PC0
  - E (Enable): PC1
  - Data Pins (D0-D7): PA0 to PA7
- **Keypad (4x4)**
  - Rows: PB0, PB1, PB2, PB3
  - Columns: PB4, PB5, PB6, PB7
- **UART Communication**
  - TXD: Connected to RXD of Control_ECU
  - RXD: Connected to TXD of Control_ECU

### Control_ECU
- **External EEPROM (I2C Communication)**
  - SCL: PC0
  - SDA: PC1
- **Buzzer**
  - Connected to PC7
- **H-bridge Motor Driver**
  - Input 1: PD6
  - Input 2: PD7
  - Enable1: PB3/OC0
- **Motor (for Door Control)**
  - Connected through the H-bridge motor driver
- **PIR Motion Sensor**
  - Connected to PC2

---

## Operation Steps

### Step 1: Create a System Password
- The LCD displays: "Please Enter Password".
- The user enters a 5-digit password (each digit is masked with `*`) and presses the enter button.
- The system then prompts: "Please re-enter the same Pass".
- The user re-enters the password.
- The HMI_ECU sends both passwords via UART to the Control_ECU.
- If the passwords match, the system stores the password in EEPROM and moves to the main options.
- If they do not match, the process repeats.

### Step 2: Main Options
- The LCD continuously displays the main system options.

### Step 3: Open Door
- The LCD prompts: "Please Enter Password".
- The user enters the password and presses the enter button.
- The HMI_ECU sends the password to the Control_ECU for verification.
- If the password is correct:
  - The motor rotates clockwise for 15 seconds to unlock the door and the LCD displays "Door is Unlocking".
  - The door remains open as long as the PIR sensor detects motion, with the LCD showing "Wait for people to Enter".
  - Once no motion is detected, the motor rotates anti-clockwise for 15 seconds to lock the door, and the LCD displays "Door is Locking".
- If the password is incorrect, proceed to Step 5.

### Step 4: Change Password
- The LCD displays: "Please Enter Password".
- The user enters the current password and presses the enter button.
- If the password matches the one stored in EEPROM, the system repeats Step 1 to set a new password.

### Step 5: Handling Failed Attempts
- If the password is incorrect during door opening (Step 3) or password change (Step 4), the user is prompted to re-enter the password.
- After three consecutive incorrect attempts:
  - The buzzer is activated for one minute.
  - The LCD displays an error message for one minute.
  - The system locks, and no keypad input is accepted during this period.
- After one minute, the system returns to the main options.

---

## System Requirements
- **System Frequency**: 8 MHz
- **Microcontroller**: ATmega32
- **Architecture**:
  - HMI_ECU
  - Control_ECU

---

## Drivers Requirements

### GPIO Driver
- Use the same GPIO driver for both ECUs.

### UART Driver
- Use the same UART driver for both ECUs.
- The `UART_init` function must accept a pointer to a configuration structure:
  ```c
  typedef struct {
      UART_BitDataType bit_data;
      UART_ParityType parity;
      UART_StopBitType stop_bit;
      UART_BaudRateType baud_rate;
  } UART_ConfigType;

  void UART_init(const UART_ConfigType * Config_Ptr);
