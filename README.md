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

## Capturing Images from the ESP32-CAM

A simple Python script `capture_client.py` is provided to grab JPEG images from
the camera's `/capture` endpoint at a fixed frame rate. You need the
`requests` package installed (`pip install requests`).

Example usage to capture at 10&nbsp;fps:

```bash
python capture_client.py 192.168.0.123 --fps 10 --output images
```

This saves sequential frames under the `images` directory until you stop the
script with `Ctrl+C`.

## Classifying Images with a PyTorch Model

Use `flame_classifier.py` to load your own PyTorch model and classify images as either `flame` or `no_flame`. The model file **must** contain a serialized `torch.nn.Module` saved with `torch.save(model, path)` rather than just the model's `state_dict`. Loading a plain state dictionary will raise an error. Save the file locally, for example as `model.pth`.

```python
from flame_classifier import load_flame_classifier

# Load the model
classify = load_flame_classifier('model.pth')

# Predict an image
label = classify('path/to/image.jpg')
print(label)  # prints 'flame' or 'no_flame'
```

This requires `torch`, `torchvision`, and `Pillow` to be installed.

## Real-time Capture and Classification

`capture_and_classify.py` combines the above capture logic with the flame
classifier so that each frame is immediately labeled. Install the required
packages (`requests`, `torch`, `torchvision`, `Pillow`) and run:

```bash
python capture_and_classify.py 192.168.0.123 --fps 10 --model model.pth --output frames
```

The script saves frames to the given directory and prints the predicted label
(`flame` or `no_flame`) for each image until you stop it with `Ctrl+C`.

## Flask Authentication with Firebase

`firebase_auth_server.py` demonstrates how to verify Firebase ID tokens in a Flask server. Install dependencies first:

```bash
pip install firebase-admin flask
```

Update the script with the path to your Firebase service account key JSON file, then start the server:

```bash
python firebase_auth_server.py
```

Clients can send a POST request to `/login` with a JSON body containing an `idToken` obtained from Firebase Authentication. The server verifies the token and returns the user's UID on success.
