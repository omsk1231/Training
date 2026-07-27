# ESP32 MQTT CO₂ Monitoring System

## Overview

This project is an ESP32-based IoT application that measures CO₂ concentration, temperature, and humidity using the SCD40 sensor. The collected data is formatted as JSON and published to an MQTT broker over Wi-Fi, enabling real-time monitoring through any MQTT client.

## Features

- Measures CO₂, temperature, and humidity
- Connects to a Wi-Fi network
- Publishes sensor data to an MQTT broker
- Sends data in JSON format
- Modular and easy-to-maintain code structure

## Hardware

- ESP32 Development Board
- Sensirion SCD40 CO₂ Sensor

## Software

- ESP-IDF v5.1.4
- Arduino as an ESP-IDF Component
- ESP-IDF MQTT Client

## Project Structure

- **main.cpp** – Entry point of the application
- **system.cpp** – Coordinates all modules
- **wifi.cpp** – Handles Wi-Fi connection
- **sensor.cpp** – Reads data from the SCD40 sensor
- **json.cpp** – Creates the JSON payload
- **mqtt_wrapper.cpp** – Handles MQTT communication

## Data Format

```json
{
  "co2": 520,
  "temperature": 27.15,
  "humidity": 49.60
}
```

## Applications

- Indoor Air Quality Monitoring
- Smart Buildings
- Environmental Monitoring
- IoT-Based Data Logging
- Home and Industrial Automation

## Author

**Om Kamble**
M.Sc. Electronics Science