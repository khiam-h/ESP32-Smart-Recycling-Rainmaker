# Smart Recycling IoT System (ESP32 + Espressif RainMaker)

An IoT recycling bin / system prototype using **ESP32 (C)** with cloud connectivity via **Espressif RainMaker**.


## Features
- Device provisioning (RainMaker)
- Cloud-connected telemetry (e.g., bin status / sensor readings)
- Remote control / configuration (via RainMaker app)
- Ultrasonic sensor 

## Tech stack
- ESP32 (C / ESP-IDF)
- Espressif RainMaker
- Wi-Fi connectivity + MQTT/RainMaker transport

## How to flash
### Prerequisites
- ESP-IDF 
- ESP32 board support installed

## Notes
- This repo intentionally excludes credentials and tokens.
- If you fork, keep `config.h` local and untracked.

## Next improvements
- Add OTA updates
- Add local fallback mode when cloud is unavailable
- Improve sensor debouncing / filtering
