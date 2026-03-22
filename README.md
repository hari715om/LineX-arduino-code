# LineX-Bot | Arduino Firmware

This repository contains the core firmware for the **LineX-Bot** robotic platform. It provides a robust engine for autonomous navigation and real-time remote control via Bluetooth.

## Core Functionality

* **Dual-Mode Operation:** Seamlessly toggle between **Autonomous Line-Following** and **Manual Telemetry** modes.
* **Precision Navigation:** Multi-state sensor mapping prevents overshooting on sharp curves and maintains center-track alignment.
* **Auto-Recovery System:** If the line is lost, the robot utilizes directional memory to pivot and re-acquire the track automatically.
* **Torque Management:** Built-in stall protection ensures consistent motor performance at varying battery voltages.
* **Smart Initialization:** In Auto-Mode, the robot remains idle until a black line is detected, and stops automatically when the track ends.

## Hardware Stack

* **MCU:** Arduino Uno (ATmega328P)
* **Motor Driver:** L298N Dual H-Bridge
* **Sensors:** 3x TCRT5000 IR Sensors
* **Communication:** HC-05 Bluetooth Module (9600 Baud)

## 📡 Control Protocol (Bluetooth)

The firmware handles the following character triggers:

| Trigger | Action |
| :--- | :--- |
| `A` / `M` | Toggle Auto / Manual Mode |
| `F` / `B` | Forward / Backward |
| `L` / `R` | Left / Right Pivot |
| `S` | Emergency Stop |
| `1`, `2`, `3` | Speed Presets (Low, Mid, High) |

## Setup

1. Clone the repository.
2. Open `LineX-arduino-code.ino` in the Arduino IDE.
3. Select **Board: Arduino Uno** and **Port: [Your Port]**.
4. Upload to the microcontroller.

