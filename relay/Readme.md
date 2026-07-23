# ESP-IDF Relay Control

## Overview

This is my second ESP-IDF project created to learn how to control relay modules using the ESP32.

In this project, two GPIO pins are used as inputs. Based on the input state, the corresponding relay is turned ON or OFF.

---

## Features

- Control two relay modules
- Read digital input using GPIO
- Turn relay ON and OFF based on input
- Uses ESP-IDF framework

---

## Hardware Required

- ESP32 Development Board
- 2-Channel Relay Module
- Jumper Wires
- USB Cable

---

## Software Required

- ESP-IDF
- Visual Studio Code
- Git

---

## Working

- GPIO 5 is used as Input 1.
- GPIO 18 is used as Input 2.
- GPIO 14 controls Relay 1.
- GPIO 13 controls Relay 2.

If Input 1 becomes LOW, Relay 1 turns ON.

If Input 1 becomes HIGH, Relay 1 turns OFF.

Similarly, Input 2 controls Relay 2.

---

## Pin Configuration

| GPIO Pin | Function |
|----------|----------|
| GPIO 5 | Input 1 |
| GPIO 18 | Input 2 |
| GPIO 14 | Relay 1 Output |
| GPIO 13 | Relay 2 Output |

---

## Folder Structure

```
relay/
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
IN1=0  RELAY1=1 | IN2=1  RELAY2=0
IN1=1  RELAY1=0 | IN2=0  RELAY2=1
```

---

## What I Learned

- Reading GPIO inputs
- Controlling GPIO outputs
- Relay interfacing with ESP32
- Basic ESP-IDF programming

---

## Author

Om Kamble