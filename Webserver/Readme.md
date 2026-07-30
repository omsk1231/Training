# ESP32 Webserver-Based CO₂ Monitoring and Relay Control System

---

## Project Information

**Author:** Om Kamble  
**Project Type:** Embedded Systems Training Project  
**Platform:** ESP32  
**Framework:** ESP-IDF 5.1.4  
**Programming Language:** C/C++  
**Development Environment:** Visual Studio Code  
**Date:** July 2026  

---

## Overview

This project implements an ESP32-based webserver for real-time CO₂ monitoring, GPIO-based relay/LED control, Wi-Fi configuration, and persistent data storage using NVS.

The ESP32 hosts a local web dashboard that allows the user to:

- Monitor real-time CO₂ concentration from an SCD41 sensor.
- Control two relay and LED channels.
- Monitor the current logical state of each control channel.
- Count how many times each relay has been switched ON.
- Store relay/LED states in NVS.
- Restore the previous relay/LED states after ESP32 power loss or restart.
- Store Wi-Fi credentials in NVS.
- Change Wi-Fi credentials from the web dashboard.
- Retain Wi-Fi credentials after restart.
- Control the same relay/LED channels using physical GPIO inputs.

The project is developed using ESP-IDF and follows a modular `.h` and `.cpp` source-file architecture.


---

# Features

## 1. Real-Time CO₂ Monitoring

The system interfaces with an SCD41 CO₂ sensor over I2C.

The sensor is connected using:

| Signal | ESP32 GPIO |
|---|---:|
| SDA | GPIO 21 |
| SCL | GPIO 22 |

The ESP32 periodically reads CO₂ concentration from the sensor and displays the latest valid value on the web dashboard.

The dashboard updates the displayed value periodically without intentionally resetting the displayed CO₂ value between valid measurements.


---

## 2. Web Dashboard

The ESP32 runs an HTTP webserver.

After the ESP32 connects to Wi-Fi, the dashboard can be opened using the IP address assigned to the ESP32.

Example:

    http://192.168.x.x

The dashboard provides:

- CO₂ concentration display
- Control 1 ON/OFF buttons
- Control 2 ON/OFF buttons
- Current logical states
- Relay 1 ON counter
- Relay 2 ON counter
- Wi-Fi SSID input
- Wi-Fi password input
- Save and connect option for changing Wi-Fi


---

# 3. GPIO, Relay and LED Control

Two independent control channels are implemented.

## Control 1

    Physical Input : GPIO 5
    Relay 1        : GPIO 14
    LED 1          : GPIO 2

## Control 2

    Physical Input : GPIO 18
    Relay 2        : GPIO 13
    LED 2          : GPIO 15


---

# 4. Hardware Logic

The project uses different electrical logic for the control inputs, relays and LEDs.

## Physical Control Inputs

GPIO 5 and GPIO 18 use inverse input logic:

    Hardware HIGH -> Idle
    Hardware LOW  -> Active

Therefore, a HIGH-to-LOW transition can be interpreted as activation of the corresponding physical control.


## Relay Logic

The relays are active LOW.

    GPIO LOW  -> Relay ON
    GPIO HIGH -> Relay OFF


## LED Logic

The LEDs are active HIGH.

    GPIO HIGH -> LED ON
    GPIO LOW  -> LED OFF


## Logical Control Behaviour

For Control 1:

    Logical ON
        |
        +---- GPIO14 LOW  -> Relay 1 ON
        |
        +---- GPIO2 HIGH  -> LED 1 ON


    Logical OFF
        |
        +---- GPIO14 HIGH -> Relay 1 OFF
        |
        +---- GPIO2 LOW   -> LED 1 OFF


For Control 2:

    Logical ON
        |
        +---- GPIO13 LOW  -> Relay 2 ON
        |
        +---- GPIO15 HIGH -> LED 2 ON


    Logical OFF
        |
        +---- GPIO13 HIGH -> Relay 2 OFF
        |
        +---- GPIO15 LOW  -> LED 2 OFF

This abstraction allows the dashboard to display normal logical ON/OFF states even though the relay hardware itself uses active-LOW logic.


---

# 5. Relay ON Counters

The system maintains separate ON counters for both relays.

The counter increases only when the corresponding logical control changes from:

    OFF -> ON

For example:

    Relay 1 ON Count = 8

means Relay 1 has previously transitioned from OFF to ON eight times.

Turning the relay OFF does not increment the counter.

Repeatedly requesting ON while it is already ON also does not intentionally increment the counter.


---

# 6. NVS Persistent Storage

ESP32 Non-Volatile Storage (NVS) is used as persistent storage.

The system stores:

- Relay 1 ON count
- Relay 2 ON count
- Control 1 state
- Control 2 state
- Wi-Fi SSID
- Wi-Fi password

Because NVS is stored in flash memory, this information can survive a normal ESP32 restart or power cycle.


---

# 7. Relay/LED State Restoration

Whenever a logical control state changes, its new state is saved in NVS.

Example:

    Control 1 = ON

The corresponding logical state is stored.

If power is removed while Control 1 is ON, the ESP32 can read the saved state during the next startup.

Startup sequence:

    ESP32 Power ON
          |
          v
      Initialize NVS
          |
          v
    Read saved states
          |
          v
    Restore outputs
          |
          v
    Relay 1 ON
    LED 1 ON

Restoring an already saved ON state does not intentionally increment the relay ON counter.


---

# 8. Wi-Fi Credential Storage

Wi-Fi credentials are also stored in NVS.

During startup:

    ESP32 Boot
        |
        v
    Read Wi-Fi credentials from NVS
        |
        +---- Credentials available
        |          |
        |          v
        |     Connect using saved Wi-Fi
        |
        +---- Credentials unavailable
                   |
                   v
              Use default Wi-Fi

This avoids the need to re-enter or recompile Wi-Fi credentials after every normal restart.


---

# 9. Changing Wi-Fi from Dashboard

The dashboard provides fields for:

- SSID
- Password

When new credentials are submitted:

    Dashboard
        |
        v
    Receive SSID + Password
        |
        v
    Save credentials in NVS
        |
        v
    Disconnect current Wi-Fi
        |
        v
    Apply new credentials
        |
        v
    Connect to new Wi-Fi
        |
        v
    Receive new IP address

After switching networks, the ESP32 may receive a different IP address.

The client device normally needs network reachability to the ESP32's new local IP address in order to access the locally hosted dashboard.


---



# 11. Module Description

## main.cpp

Main application entry point.

Responsibilities:

- Initialize NVS
- Initialize GPIO
- Initialize CO₂ sensor
- Initialize Wi-Fi
- Start HTTP webserver
- Start physical GPIO monitoring task


## CO2 Module

Files:

    CO2/CO2.h
    CO2/CO2.cpp

Responsibilities:

- Initialize I2C
- Initialize SCD41
- Start periodic measurement
- Read CO₂ measurement
- Perform CRC validation
- Provide CO₂ data to the webserver


## GPIO Module

Files:

    gpio/gpio.h
    gpio/gpio.cpp

Responsibilities:

- Configure GPIO5 and GPIO18 as control inputs
- Configure relay outputs
- Configure LED outputs
- Convert logical state into appropriate hardware levels
- Handle dashboard control requests
- Handle physical GPIO control
- Maintain relay ON counters
- Save logical states to NVS
- Restore saved output states


## Storage Module

Files:

    storage/storage.h
    storage/storage.cpp

Responsibilities:

- Initialize NVS
- Store relay ON counters
- Read relay ON counters
- Store logical output states
- Restore logical output states
- Store Wi-Fi credentials
- Read Wi-Fi credentials


## Wi-Fi Module

Files:

    wifi/wifi.h
    wifi/wifi.cpp

Responsibilities:

- Initialize ESP32 Wi-Fi station mode
- Read saved credentials from NVS
- Connect to Wi-Fi
- Handle connection/disconnection events
- Receive new Wi-Fi credentials
- Save new credentials
- Reconnect using new credentials


## Webserver Module

Files:

    webserver/webserver.h
    webserver/webserver.cpp

Responsibilities:

- Start HTTP server
- Serve HTML dashboard
- Provide CO₂/status API
- Receive relay control commands
- Receive Wi-Fi configuration requests
- Update dashboard data


---

# 12. Programming Concepts Demonstrated

This project demonstrates several important embedded C/C++ concepts.


## Source and Header File Structure

Function declarations and public interfaces are placed in `.h` files.

Function implementations are placed in `.cpp` files.

Example:

    gpio.h   -> declarations
    gpio.cpp -> implementations


## Comments and Documentation

The source code uses:

- Section comments
- Function descriptions
- Hardware logic explanations
- Doxygen-style comments


## Variables and Functions

The project demonstrates:

- Local variables
- Static variables
- Global/static module state
- Function parameters
- Function return values
- Function declarations
- Function definitions
- Function calls


## Structures

A structure is used to group GPIO-related information.

Example:

    typedef struct
    {
        output_state_t gpio1_state;
        output_state_t gpio2_state;

        uint32_t relay1_count;
        uint32_t relay2_count;

    } gpio_status_t;


## Enumerations

Enums are used to provide meaningful names for states.

Example:

    typedef enum
    {
        OUTPUT_OFF = 0,
        OUTPUT_ON  = 1

    } output_state_t;

Another enum identifies the relay used by the storage module.


## String Operations

Wi-Fi credential handling demonstrates C-style string operations including:

- Character arrays
- strncpy()
- strlen()
- NVS string storage
- NVS string retrieval


## Union

A union is not currently required by the implemented functionality.

It can be introduced later where the same raw memory needs to be interpreted in multiple formats, for example during Modbus register/data conversion.


---

# 13. HTTP API

The dashboard communicates with the ESP32 using HTTP endpoints.

## Read System Status

    GET /api/status

Returns information including:

- CO₂ concentration
- Control 1 state
- Control 2 state
- Relay 1 ON count
- Relay 2 ON count


## Write Control

    POST /api/control

Used to change the logical state of Control 1 or Control 2.


## Change Wi-Fi

    POST /api/wifi

Used to submit:

- New SSID
- New password


---



# 17. Accessing the Dashboard

After successful Wi-Fi connection, the ESP32 prints its IP address in the serial monitor.

Example:

    WIFI: Connected
    WIFI: ESP32 IP: 192.168.1.100

Open:

    http://192.168.1.100

The browser/device must have network connectivity to the ESP32's local IP address.


---



# 19. Hardware Summary

| Function | GPIO | Logic |
|---|---:|---|
| Control Input 1 | GPIO 5 | LOW = Active |
| Control Input 2 | GPIO 18 | LOW = Active |
| Relay 1 | GPIO 14 | LOW = ON |
| Relay 2 | GPIO 13 | LOW = ON |
| LED 1 | GPIO 2 | HIGH = ON |
| LED 2 | GPIO 15 | HIGH = ON |
| SCD41 SDA | GPIO 21 | I2C |
| SCD41 SCL | GPIO 22 | I2C |


---



## Platform

- ESP32
- ESP-IDF 5.1.4
- C/C++
- FreeRTOS
- HTTP Server
- NVS
- I2C
- Wi-Fi


---

## Purpose

This project was developed as an embedded systems training project to demonstrate practical ESP32 firmware architecture, peripheral interfacing, webserver development, persistent storage, GPIO control, Wi-Fi management, and modular C/C++ programming.