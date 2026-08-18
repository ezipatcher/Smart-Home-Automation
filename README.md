# Smart Home Automation System using ESP32

An IoT-based Smart Home Automation System built using an **ESP32** to automate home appliances, improve security, and provide intelligent control using sensors and actuators.

## Features

### 1. Temperature-Based Fan Control

* Uses a thermistor to monitor temperature.
* Turns ON an LED to simulate a fan when the temperature exceeds a threshold.
* Activates a buzzer for 2 seconds when the temperature becomes high.

### 2. Automatic Lighting System

* Uses an LDR to detect ambient light.
* Automatically turns ON the house light LED in darkness.
* Turns OFF the light when sufficient brightness is detected.

### 3. Intruder Alert System

* Uses a PIR motion sensor to detect movement.
* Flashes an LED when motion is detected.
* Activates a buzzer as an intruder alert.

### 4. Smart Door Control

* Uses a push button as a doorbell.
* A servo motor rotates 90° to open the door.
* A welcome LED flashes once.
* The servo returns to its initial position.

### 5. Security Mode

* A push button toggles security mode ON/OFF.
* When security mode is ON, PIR motion detection activates the red LED and buzzer.
* When security mode is OFF, motion is ignored.

## Hardware Components

* ESP32 Development Board
* Thermistor Module
* LDR Module
* PIR Motion Sensor
* Push Buttons
* Servo Motor
* LEDs
* Red LED
* Buzzers
* 220Ω Resistors
* Jumper Wires
* Breadboard

## System Architecture

```text
                  ┌─────────────────────┐
                  │        ESP32        │
                  │   Main Controller   │
                  └──────────┬──────────┘
                             │
       ┌─────────────┬───────┼────────┬─────────────┐
       │             │       │        │             │
  Thermistor        LDR     PIR    Push Button    Servo
       │             │       │        │             │
     Fan LED      Light LED  │    Door/Security   Door
       │             │       │     Control
    Buzzer          │     LED+Buzzer
                   │
                Lighting
```

## Project Structure

```text
Smart-Home-Automation-ESP32/
│
├── README.md
├── .gitignore
│
├── src/
│   ├── temperature_fan/
│   ├── automatic_lighting/
│   ├── intruder_alert/
│   ├── smart_door/
│   ├── security_mode/
│   └── main/
│
├── circuit/
│   └── circuit_diagram.png
│
├── images/
│   └── project_setup.jpg
│
└── docs/
    └── project_report.pdf
```

## Technologies Used

* ESP32
* Arduino IDE
* Embedded C/C++
* IoT
* Sensors and Actuators
* PWM Servo Control
* Digital and Analog Signal Processing

## Future Improvements

* Add Wi-Fi-based mobile control.
* Add a web dashboard for real-time monitoring.
* Add remote security notifications.
* Add energy-consumption monitoring.
* Integrate AI for predictive automation.
* Add voice control.
* Store sensor data in a cloud database.

## Author

**Rupam Gope**

Electronics and Communication Engineering Student

---

## License

This project is licensed under the MIT License.
