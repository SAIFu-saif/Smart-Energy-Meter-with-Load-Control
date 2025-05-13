# Smart-Energy-Meter-with-Load-Control

This project is an embedded system-based smart energy meter that monitors real-time power consumption and controls the electrical load when consumption exceeds a threshold.

## Features
- Real-time voltage and current measurement
- Power (Watts) calculation
- LCD display for live data
- Relay-based load cutoff system
- Adjustable threshold for automatic load control

## Components Used
- Microcontroller: ATmega328P / Arduino Uno
- Current Sensor: ACS712
- Voltage Divider Circuit (for voltage sensing)
- Relay Module
- 16x2 LCD Display (I2C or parallel)
- Push Button (for setting/resetting threshold)
- Power Supply (5V/12V)
- Misc: Resistors, wires, PCB or breadboard

## Working Principle
1. The current sensor detects load current.
2. Voltage is sensed through a voltage divider.
3. Power = Voltage × Current is calculated in the microcontroller.
4. Data is shown on LCD.
5. If the calculated power exceeds a user-defined limit, the microcontroller turns off the relay to disconnect the load.  

## How to Use
1. Connect all components as per the circuit diagram.
2. Upload the code to the microcontroller.
3. Power the system.
4. The LCD will display voltage, current, and power.
5. If power exceeds the set limit, the relay disconnects the load.

## How it works
The Arduino reads analog data from sensors, calculates power, and controls a relay based on power threshold.

## Future Improvements
- Wi-Fi based remote monitoring (ESP8266/ESP32)
- Cloud logging
- Mobile app integration

