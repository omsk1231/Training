# ESP32 SCD41 CO₂ Sensor Interface using ESP-IDF

## Overview

This project demonstrates how to interface the **Sensirion SCD41 CO₂ Sensor** with an **ESP32** using the **ESP-IDF framework** and the **Arduino component**.

The sensor measures:

- Carbon Dioxide (CO₂)
- Temperature
- Relative Humidity

The measured values are displayed on the serial monitor every 5 seconds.

---

## Features

- Reads CO₂ concentration (ppm)
- Reads Temperature (°C)
- Reads Relative Humidity (%RH)
- Uses I2C communication
- Built with ESP-IDF and Arduino

---

## Hardware Required

- ESP32 Development Board
- Sensirion SCD41 CO₂ Sensor


---

## Wiring

| SCD41 | ESP32 |
|--------|-------|
| VIN | 3.3V |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

---

## Software Requirements

- ESP-IDF 
- Arduino as ESP-IDF Component
- esp-idf-lib SCD4x Driver

---

## Project Structure

```
CO2/
│
├── main/
│   ├── main.cpp
│   └── CMakeLists.txt
│
├── managed_components/
│
├── sdkconfig
│
└── README.md
```

---

## Build the Project

Open the project directory and run:

```bash
idf.py build
```

---

## Flash the ESP32

Replace the serial port with your ESP32 port.

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## Example Output

```
================================
SCD41 CO2 Sensor Example
================================

CO2         : 421 ppm
Temperature : 26.18 C
Humidity    : 47.35 %

CO2         : 423 ppm
Temperature : 26.20 C
Humidity    : 47.42 %
```

---

## Components Used

- ESP32
- Sensirion SCD41
- ESP-IDF
- Arduino Component
- esp-idf-lib

---

## Learning Outcomes

This project helped me learn:

- ESP-IDF project structure
- Using Arduino with ESP-IDF
- I2C communication
- Interfacing environmental sensors
- Reading CO₂, temperature and humidity data
- Serial monitoring and debugging

---


## Author

**Om Kamble**

