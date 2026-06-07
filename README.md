# Smart Hydroponic Systems

An IoT-based hydroponics automation system that automatically controls AB Mix nutrient solution (A & B) using peristaltic pumps to maintain optimal PPM setpoints, monitored in real-time via Blynk dashboard.

## Features

- Automatic AB Mix nutrient control using 2 peristaltic pumps (Solution A & B)
- Progressive PPM setpoint that increases by 100 PPM every week up to week 4
- Real-time monitoring via Blynk IoT dashboard
- TDS Meter sensor for nutrient concentration measurement
- Water flow sensor to measure the volume of nutrient solution A & B dispensed into the reservoir
- NTP time synchronization for automatic weekly setpoint scheduling

## Technologies & Hardware

- **Microcontroller:** ESP32
- **Dashboard:** Blynk IoT
- **Sensors:** TDS Meter, Water Flow Sensor
- **Actuators:** 2x Peristaltic Pump (Nutrient A & B)
- **Language:** C++ (Arduino Framework)

## How It Works

1. ESP32 syncs time via NTP to track the weekly schedule
2. TDS Meter reads the current PPM (nutrient concentration) of the water
3. System compares the reading to the current weekly setpoint
4. If PPM is below setpoint, peristaltic pumps activate to dispense nutrient A and B into the reservoir
5. Water flow sensor measures the exact volume of nutrient A and B dispensed
6. If PPM reaches setpoint, pumps stop automatically
7. All data is sent to Blynk dashboard for real-time monitoring

## PPM Setpoint Schedule

- Week 1: 560 PPM
- Week 2: 660 PPM
- Week 3: 760 PPM
- Week 4: 840 PPM

Setpoint increases automatically every 7 days until week 4.

## PPM Calibration

Calibration uses a 3-point polynomial method for higher accuracy across the measurement range.

- Point 1: 0 PPM (pure water)
- Point 2: 500 PPM (standard solution)
- Point 3: 718 PPM (standard solution)

## System Testing Stages

All components were tested sequentially before full integration.

1. **NTP-Detection** — Verify NTP time synchronization
2. **Read-ADC** — Read raw ADC values from sensor
3. **PPM-Calibration** — Calibrate TDS sensor using 0, 500, and 718 PPM solutions
4. **Peristaltik-Test** — Test peristaltic pump A & B activation
5. **Waterflow-Test** — Test water flow sensor volume reading
6. **Blynk-Test** — Test Blynk dashboard connectivity and data display
7. **Final-Code** — Full system integration and simulation

## Simulation

Full system simulation was conducted over 4 minutes, where 1 minute represents 1 week and 7 data points were recorded per minute. PPM setpoint increases by 100 PPM each minute, simulating the weekly increment.

## Project Structure

Smart-Hydroponic-Systems/
├── NTP-Detection/ # Stage 1 — NTP time synchronization
├── Read-ADC/ # Stage 2 — ADC sensor reading
├── PPM-Calibration/ # Stage 3 — TDS polynomial calibration (0, 500, 718 PPM)
├── Peristaltik-Test/ # Stage 4 — Peristaltic pump testing
├── Waterflow-Test/ # Stage 5 — Water flow sensor testing
├── Blynk-Test/ # Stage 6 — Blynk dashboard testing
└── Final-Code/ # Stage 7 — Full system integration and simulation

```

## Getting Started

**Requirements**
- ESP32 board
- Arduino IDE
- Blynk IoT account
- Libraries: Blynk by Volodymyr Shymanskyy (install via Library Manager), WiFi.h (built-in ESP32), time.h (built-in ESP32)

**Steps**

1. Clone this repository
```

git clone https://github.com/andinicode/Smart-Hydroponic-Systems.git

````
2. Open the `Final-Code/` folder in Arduino IDE
3. Fill in your WiFi credentials and Blynk Auth Token
```cpp
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";
````

4. Upload the code to your ESP32
5. Open Blynk dashboard to monitor the system

## Author

**Andini Aisyah Herdiani**  
Fresh Graduate Electrical Engineering — Universitas Garut  
[LinkedIn](https://www.linkedin.com/in/andiniaisyahherdiani) | andiniaisyah663@gmail.com
