# GroceryPad
## Overview

GroceryPad is a compact display for shopping lists, to-do lists, reminders, and more. The on-board ESP32 microcontroller acts as a display driver, as well as hosts an HTTP server on the local network. Data sent to the server in the form of an HTTP POST is parsed and sent to the 3.7" Epaper module.

GroceryPad is initially designed to sync with a list managed in the Reminders app of an iOS device. In this configuration, the iOS device sends a scheduled HTTP POST with the contents of the reminder/list to the server hosted by the GroceryPad device. 

**Recent Changes**
- Custom designed PCB for compact form-factor
- Models and OrcaSlicer project for 3d printed housing

### Hardware Requirements
- [ESP32-C3 Supermini microcontroller](https://www.espboards.dev/esp32/esp32-c3-super-mini/)
- [WeAct 3.7" Epaper module](https://github.com/WeActStudio/WeActStudio.EpaperModule)
- [HiLetgo TP4056 USB-C Lithium Battery Charging Module](https://www.amazon.com/HiLetgo-Lithium-Charging-Protection-Functions/dp/B07PKND8KG)
- [3.7V Rechargeable Lithium Polymer battery](https://www.amazon.com/dp/B0CNLPK1F8?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1)
- 3mm indicator LED
- Smartphone (for automatic list syncing)

### Software Requirements
Code is uploaded to the board via ArduinoIDE, which can be downloaded here: [Download ArduinoIDE](https://www.arduino.cc/en/software/)

To connect with the ESP32 board, you must also install the ESP32 board. Instructions are available on the [espressif website](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)

For the automatic syncing with a smartphone list, an additional app must be used to schedule the sending of the HTTP POST. iPhones can use the Shortcuts app, which is installed by default on all devices running iOS 13 or later. For Android, I recommend [Tasker](https://play.google.com/store/apps/details?id=net.dinglisch.android.taskerm&hl=en_US). Steps for setting this up are found under the **Setup** section.

## Setup
For the GroceryPad device to function, a connection to a local WiFi network is required. Included with the Arduino sketch is a `ssid_options.h` file. This header file can be used to define the SSID and password variables the ESP32 will use to connect to WiFi. Fill in the `ssid` and `password` strings with your local WiFi details. *Note: The ESP32 is only compatible with 2.4 GHz WiFi.*

Once the `ssid_options.h` file is filled in, the code can be compiled and loaded onto the microcontroller. If the connection is successfull, the microcontroller will print a "WiFi Connected" message to the console. Along with the success message, the IP address will also be printed. Take note of this IP address, as it will be required in sending the HTTP POST messages.

### iOS Shortcuts Configuration
Setting up the Shortcut to periodically send the HTTP POST is not complicated, but can take some time to figure out if you are new to workflow automations or Apple Shortcuts specifically. Here is provided a simple walkthrough of setting up the Shortcut.

Items necessary to creating the Shortcut:
- Apple Shortcuts app is installed
- The IP address of the ESP32 HTTP server
- A Reminder list on the same iOS device

**Step 1**

**Sending messages manually** with curl

Messages can also be sent to the device manually using a linux-style terminal and the `curl` command. Basic syntax is

`curl -X POST http://<ESP IP ADDRESS>/reminder -H "Content-Type: application/json" -d '<MESSAGE CONTENT>'`
