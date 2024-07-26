# Dual-radio Communication with ESP32 and Espressif IDF

This repository contains two projects developed using Espressif IDF for ESP32 microcontrollers. The projects demonstrate dual-radio communication capabilities, utilizing both Bluetooth (GATT) and Wi-Fi (UDP) protocols.

## Project Descriptions

### ESP1: GATT Client and UDP Server

- **GATT Client:** Configures ESP32 as a Bluetooth GATT client. Connects to ESP2 (GATT Server) and sends a random number.
  
- **UDP Server:** Configures ESP1 to receive the random number sent by ESP2 via UDP. It echoes the received number back.

### ESP2: GATT Server and UDP Client

- **GATT Server:** Configures ESP32 as a Bluetooth GATT server. Advertises services, allowing ESP1 (GATT client) to connect and send a random number.
  
- **UDP Client:** Enables ESP2 to send the random number received from ESP1 back via UDP.

## Communication Flow
![FlowChart of the Dual-radio-communication-with-ESP32-and-Espressif-IDE](https://github.com/ayushsahu0/Dual-radio-communication-with-ESP32-and-Espressif-IDE/blob/main/esp2/Tutorial/ESP2.png)


### Bluetooth (GATT):

1. ESP1 (GATT Client) connects to ESP2 (GATT Server) and sends a random number.
2. ESP2 (GATT Server) receives the random number from ESP1.

### Wi-Fi (UDP):

1. ESP2 (UDP Client) sends the received random number back to ESP1 (UDP Server) via UDP.
2. ESP1 (UDP Server) receives the random number echoed from ESP2.

This completes the echo loop where ESP1 sends a random number via Bluetooth to ESP2, which then sends it back via UDP, demonstrating dual-radio communication capabilities.

## Development Environment

These projects use Espressif IoT Development Framework (ESP-IDF) and are intended for ESP32 microcontrollers.

### Setup Instructions

1. **Install ESP-IDF:**
   - Follow Espressif's [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html) to set up the ESP-IDF development environment on your system.

2. **Clone the Repository:**
   ```bash
   git clone https://github.com/ayushsahu0/Dual-radio-communication-with-ESP32-and-Espressif-IDE.git
   cd your-repository
