# ESP32 Modbus RTU Voltmeter Interface

## Overview

This project demonstrates Modbus RTU communication between an ESP32 and a digital voltmeter using an RS485 interface.

The ESP32 acts as the **Modbus Master**, while the voltmeter acts as the **Modbus Slave**.

The ESP32 communicates with an onboard RS485 transceiver through UART. The RS485 transceiver provides the differential **A and B** communication lines used to connect to the voltmeter.

The program reads Input Registers from the voltmeter, combines pairs of 16-bit Modbus registers, decodes them as 32-bit floating-point values, and displays both the raw register data and decoded measurements through the serial monitor.

---

## Communication Architecture

```text
ESP32
  |
  | UART
  | TX = GPIO17
  | RX = GPIO16
  |
  v
Onboard RS485 Transceiver
  |
  | A / B
  |
  v
Digital Voltmeter
(Modbus RTU Slave)
```

Only the RS485 **A and B** lines are externally connected between the ESP32 board and the voltmeter.

---

## Modbus Configuration

| Parameter | Value |
|---|---|
| Protocol | Modbus RTU |
| ESP32 Role | Master |
| Voltmeter Role | Slave |
| Slave ID | 2 |
| Baud Rate | 9600 |
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |
| Function Code | 04 - Read Input Registers |
| Start Address | 0 |
| Register Count | 16 |
| UART | UART2 |
| UART TX | GPIO17 |
| UART RX | GPIO16 |

---

## Register Reading

The ESP32 requests 16 Input Registers starting from address 0.

```text
Register 0
Register 1
Register 2
...
Register 15
```

Each Modbus register contains **16 bits (2 bytes)**.

The meter measurements are interpreted as 32-bit floating-point values. Therefore, two consecutive 16-bit registers are combined to obtain one measurement.

```text
Register 0  + Register 1  -> Value 1
Register 2  + Register 3  -> Value 2
Register 4  + Register 5  -> Value 3
Register 6  + Register 7  -> Value 4
Register 8  + Register 9  -> Value 5
Register 10 + Register 11 -> Value 6
Register 12 + Register 13 -> Value 7
Register 14 + Register 15 -> Value 8
```

The actual meaning and availability of each value depend on the voltmeter's register map.

---

## Data Decoding

A Modbus register is 16 bits, while an IEEE-754 `float` is 32 bits.

Therefore, the program combines two registers:

```text
        32-bit measurement

+----------------+----------------+
|   High Word    |    Low Word    |
|    16 bits     |     16 bits    |
+----------------+----------------+
```

For the current implementation:

```text
First Register  -> Low Word
Second Register -> High Word
```

The program combines them into a 32-bit value and then interprets the resulting bit pattern as an IEEE-754 floating-point value.

This word order is based on the current meter testing and can be changed if further hardware testing shows a different register format.

---

## Modbus Request

The ESP32 sends a Modbus RTU request containing:

```text
Slave ID
   |
Function Code
   |
Starting Register Address
   |
Number of Registers
   |
CRC16
```

For this project:

```text
Slave ID       = 2
Function Code  = 04
Start Address  = 0
Register Count = 16
```

The CRC16 is automatically calculated before transmitting the request.

---

## Response Processing

After sending the request, the ESP32:

1. Waits for the Modbus slave response.
2. Checks the Slave ID.
3. Checks the Function Code.
4. Checks the returned byte count.
5. Verifies the Modbus CRC16.
6. Extracts the 16-bit registers.
7. Combines register pairs.
8. Converts the combined data into floating-point values.
9. Prints the raw and decoded data.

---

## Project Structure

```text
R485 - RTU/
|
├── CMakeLists.txt
├── README.md
|
└── main/
    ├── CMakeLists.txt
    ├── main.cpp
    ├── modbus.cpp
    └── modbus.h
```

### `main.cpp`

Contains the main application flow:

```text
Initialize Modbus
      |
Read registers
      |
Decode measurements
      |
Display data
      |
Wait 1 second
      |
Repeat
```

### `modbus.h`

Contains:

- Modbus configuration
- UART pin configuration
- Data structures
- Public Modbus function declarations

### `modbus.cpp`

Contains:

- UART initialization
- Modbus RTU request generation
- CRC16 calculation
- Response validation
- Register extraction
- 32-bit float decoding
- Data display functions

---


## Hardware Connection

Connect the RS485 lines as follows:

```text
ESP32 RS485 Board        Voltmeter
-----------------        ---------
A        ---------------- A
B        ---------------- B
```

The UART connection between GPIO17/GPIO16 and the RS485 transceiver is assumed to be implemented onboard.

---

## Current Test Configuration

The communication settings were identified during initial testing using `mbpoll` on Linux.

The meter responded using:

```text
Slave ID : 2
Baud     : 9600
Format   : 8N1
Function : 04
```

The current ESP32 implementation uses the same communication parameters.

---

