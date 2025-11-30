# Sub-0 Cooling Controller

This project implements an intelligent cooling control system that bridges a Linux PC and an Arduino. It monitors the PC's CPU temperature in real-time via a Python script and automates a cooling relay based on specific thermal thresholds and safety checks.

## 🌟 Features

* **PC-to-Arduino Communication:** A Python script reads the CPU temperature from the Linux system kernel and transmits it to the Arduino via Serial.
* **Hysteresis Cooling Logic:**
    * **Turn ON:** Cooling activates when the CPU reaches **60°C**.
    * **Turn OFF:** Cooling deactivates when the CPU drops to the **Ambient Room Temperature**.
* **Safety Mechanisms:**
    * **Hot Side Protection:** Automatically cuts power if the cooler's hot side exceeds **60.0°C**.
    * **Failsafe:** Disables cooling if the sensor disconnects (-127.0 error) or if the PC temperature is reading 0 (connection lost).
* **OLED Dashboard:** Displays Room Temp, CPU Temp, Trigger Threshold, and System Status (IDLE/COOLING/!HOT!).

## 🛠 Hardware Requirements

* **Microcontroller:** Arduino (Uno/Nano or compatible)
* **Display:** 128x64 SSD1306 OLED Display (I2C)
* **Sensors:** 2x DS18B20 Temperature Sensors (OneWire)
* **Actuator:** Relay Module
* **Connection:** USB Cable for Serial communication

## 🔌 Wiring & Pinout

| Component | Arduino Pin | Notes |
| :--- | :--- | :--- |
| **Relay Module** | **Pin 7** | Active LOW (LOW = ON) |
| **Ambient Sensor** | **Pin 6** | DS18B20 Data |
| **Cooler Sensor** | **Pin 5** | DS18B20 Data (Hot Side Safety) |
| **OLED Display** | **I2C (SDA/SCL)** | Connected via Wire library |

## 💻 Software Dependencies

### Arduino Libraries
Install the following via the Arduino Library Manager:
* `OneWire`
* `DallasTemperature`
* `Adafruit_GFX`
* `Adafruit_SSD1306`

### Python Requirements
The monitoring script requires `pyserial`:
```bash
pip install pyserial
```
    *Note: The script reads from `/sys/class/thermal/thermal_zone0/temp`. Ensure your user has permissions to read this file.*
## 🚀 Installation & Usage

### 1. Arduino Setup
1.  Connect hardware according to the pinout above.
2.  Open `arduino.ino`.
3.  Upload the code to your Arduino board.

### 2. PC Setup (Linux)
1.  Ensure you have `python3` installed.
2.  Edit `monitor.py` if your Arduino is not on `/dev/ttyUSB0`.
3.  Run the monitor script:
    ```bash
    python3 monitor.py
    ```
    *Note: The script reads from `/sys/class/thermal/thermal_zone0/temp`. Ensure your user has permissions to read this file.*
