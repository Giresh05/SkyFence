# SkyFence - LoRa GPS Tracker & Geofencing System 🌍

## Overview

**SkyFence** is a GPS tracking and geofencing system using **LoRa** communication. It consists of two primary components:

1. **LoRa GPS Sender**: Sends real-time GPS data via LoRa. 📡
2. **LoRa GPS Receiver with Web Interface**: Receives the GPS data and displays it on a web interface, showing if the tracked device is inside or outside the geofenced area. 🌐

## Features

- **LoRa GPS Tracking**: Real-time GPS data transmission using LoRa communication. 🌍
- **Geofencing**: Ability to check whether the device is inside or outside a predefined geofence. 🛑
- **Web Interface**: Displays live GPS data on a map and shows geofencing status (inside/outside). 💻
- **Leaflet.js**: Utilized for dynamic map rendering and visualization. 🗺️

## Components Used

- **LoRa Module** (e.g., SX1278 or similar) 📡
- **GPS Module** (e.g., NEO-6M GPS) 📍
- **ESP32** Microcontroller 🔧
- **WiFi Network** (for serving the web interface and sending real-time data) 🌐
- **Web Interface**: Built using **Leaflet.js** and **OpenStreetMap**. 🧭

## Installation

### 1. LoRa GPS Sender

**File Name**: `lora_gps_sender.ino` 📝

#### Hardware Connections:

| **LoRa Pin** | **ESP32 Pin** |
|--------------|---------------|
| SCK          | 5             |
| MISO         | 19            |
| MOSI         | 27            |
| SS           | 18            |
| RST          | 14            |
| DIO0         | 26            |

| **GPS Pin**  | **ESP32 Pin** |
|--------------|---------------|
| RX           | 16            |
| TX           | 17            |

Upload the `lora_gps_sender.ino` to your **ESP32**, and ensure proper wiring for the **GPS** and **LoRa** modules.

### 2. LoRa GPS Receiver + Web Server

**File Name**: `lora_gps_receiver_web.ino` 🖥️

This part handles receiving the GPS data over **LoRa** and serving a web interface.

#### Web Interface Features:
- **Live GPS Location**: Displayed on a map using **Leaflet.js**. 🗺️
- **Geofence Status**: Show if the GPS is **inside** or **outside** a geofence. 🛑

Update the **WiFi credentials** in the code and upload it to the **ESP32**. Once done, the ESP32 will serve a webpage with the live GPS data and geofencing status.

## Geofencing

The geofence has a **2000 meters radius** centered at:

- **Latitude**: 12.8292
- **Longitude**: 80.0482

**Haversine formula** is used to calculate the distance between the GPS coordinates and the geofence center to determine whether the GPS device is inside or outside the defined boundary. 📏

## Web Interface

The **web interface** displays the following:
- A **map** (using **OpenStreetMap** and **Leaflet.js**). 🗺️
- A **geofence circle** of **2000 meters**. 🛑
- **Real-time updates** of the GPS location and geofencing status. 🕒

## Troubleshooting

- **LoRa Connection Issues**: Verify the wiring of the **LoRa module**. Ensure you're using the correct **frequency** (433 MHz). 📡
- **GPS Lock**: Ensure the **GPS module** has a clear view of the sky and a good satellite fix. 🌌
- **Web Page Issues**: Check the **WiFi connection** on the ESP32. Ensure it is connected to the network. 🌐

## Future Enhancements

- Add support for **multiple GPS senders** communicating with the same receiver. 🛰️
- Implement **notifications** when the GPS device enters or exits the geofence. 📲
- Optimize for **low power consumption** on ESP32 for longer deployment times. ⚡

## License

This project is licensed under the **MIT License**. 📜

## Acknowledgments

- **Leaflet.js**: For map visualization. 🗺️
- **OpenStreetMap**: For providing free map tiles. 🌍
- **LoRa**: For enabling long-range communication. 📡
- **ESP32**: For handling both communication and web serving. 🔧
- **TinyGPSPlus**: For parsing GPS data. 📍
