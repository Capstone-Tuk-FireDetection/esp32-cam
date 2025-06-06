# ESP32-CAM Example

This repository provides an Arduino sketch for the AI Thinker ESP32-CAM module.

To use the sketch, copy the `esp32-cam` directory to your Arduino sketchbook so that the IDE can find it.

## Required Tools
- Arduino IDE 1.8 or newer
- ESP32 board package installed via the **Boards Manager** (Espressif Systems).
  Select the "AI Thinker ESP32-CAM" board before uploading.

## Sketch Location
The main sketch is located in [`esp32-cam/CameraWebServer`](esp32-cam/CameraWebServer). Open `CameraWebServer.ino` with the Arduino IDE.

## Library Installation
Two libraries are provided under the `libraries` folder:
- `Adafruit_Unified_Sensor`
- `DHT_sensor_library`

Copy the folders inside `libraries` into the `libraries` subfolder of your Arduino sketchbook. You can find the path to the sketchbook (and its `libraries` subdirectory) in **File > Preferences** within the Arduino IDE. If you already have these libraries installed, you can skip copying them.

## Wi-Fi Configuration
Configure your Wi-Fi credentials in [`wifi_config.h`](esp32-cam/CameraWebServer/wifi_config.h). Edit the following lines:

```c
#define WIFI_SSID      "YOUR_SSID"
#define WIFI_PASSWORD  "YOUR_PASSWORD"
```

Ensure this file is stored in the same directory as `CameraWebServer.ino` before compiling.
