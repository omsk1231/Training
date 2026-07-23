# ESP-IDF Temperature and Humidity Sensor

## Overview

This is my third ESP-IDF project created to learn how to interface the AHT20 temperature and humidity sensor with the ESP32 using I2C communication.

The ESP32 reads the temperature and humidity values from the sensor and displays them on the serial monitor.

---

## Features

- Read temperature from AHT20 sensor
- Read humidity from AHT20 sensor
- Uses I2C communication
- Display sensor values on Serial Monitor
- Uses ESP-IDF framework

---

## Hardware Required

- ESP32 Development Board
- AHT20 Temperature and Humidity Sensor
- Jumper Wires
- USB Cable

---

## Software Required

- ESP-IDF
- Visual Studio Code
- Git

---

## Pin Configuration

| ESP32 Pin | AHT20 Pin |
|-----------|-----------|
| GPIO 21 | SDA |
| GPIO 22 | SCL |
| 3.3V | VCC |
| GND | GND |

---

## Working

The ESP32 communicates with the AHT20 sensor using the I2C protocol.

The sensor measures temperature and humidity every second.

The measured values are displayed on the serial monitor.

---

## Folder Structure

```
tempreture/
├── main/
├── CMakeLists.txt
├── sdkconfig
├── README.md
└── .gitignore
```

---

## Build

```bash
idf.py build
```

---

## Flash

```bash
idf.py flash
```

---

## Monitor

```bash
idf.py monitor
```

---

## Serial Output

Example:

```text
---------------------------
Temperature : 28.35 C
Humidity    : 64.72 %
Raw Temp    : 701225
Raw Hum     : 678945
```

---

## What I Learned

- I2C communication with ESP32
- Reading sensor data using ESP-IDF
- Displaying sensor values on the Serial Monitor
- Basic AHT20 sensor interfacing

---

## Author

Om Kamble