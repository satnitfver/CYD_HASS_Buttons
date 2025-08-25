# ESP32 CYD (Cheap Yellow Display) Touchscreen MQTT Button Panel

This project transforms an ESP32 with a CYD touchscreen into a customizable MQTT button panel. It allows you to trigger MQTT messages by touching virtual buttons on the display, making it ideal for home automation, IoT control, or interactive interfaces.

---

## ✨ Features

* **Customizable Buttons:** Define multiple virtual buttons with specific labels, colors, and positions.
* **MQTT Integration:** Publishes messages to pre-defined MQTT topics when buttons are pressed.
* **WiFi Connectivity:** Connects to your local WiFi network.
* **TFT Display Support:** Utilizes a TFT_eSPI compatible display for crisp visuals.
* **XPT2046 Touchscreen Support:** Detects touch inputs for interactive control.
* **Deep Sleep Mode:** Enters a low-power deep sleep state after a period of inactivity to conserve energy, waking up on touch.

---

## 🛠️ Hardware Requirements (working versions, from Ali Express)

* **2.8inch ESP32 Module TFT LCD Display with Acrylic Case Dual-core ESP32-2432S028R WIFI+BT 240X320 ILI9341 Resistive Touchscreen
* **TENSTAR ROBOT ESP32 LVGL WIFI&Bluetooth Development Board 2.8 Inch 240*320 Smart Display Screen TFT Module With Touch

---

## 💻 Software Requirements

* **Arduino IDE:** Or any compatible ESP32 development environment.
* **ESP32 Board Package:** Install the ESP32 boards manager in Arduino IDE.
* **Libraries:**
    * `WiFi.h` (Standard Arduino ESP32 library)
    * `PubSubClient.h` ([Install from Library Manager](https://github.com/knolleary/pubsubclient))
    * `TFT_eSPI.h` ([Install from Library Manager](https://github.com/Bodmer/TFT_eSPI)) - **Important:** You will need to configure `User_Setup_Select.h` in the `TFT_eSPI` library to match your specific display.
    * `XPT2046_Touchscreen.h` ([Install from Library Manager](https://www.google.com/search?q=https://github.com/xpt2046-library/XPT2046_Touchscreen))
    * `SPI.h` (Standard Arduino library)
    * `FreeSans9pt7b.h` (Part of the `TFT_eSPI` library examples, or can be included directly for font support)

---

## ⚙️ Configuration

Before uploading the code, you need to modify the `CONFIG` section with your network and MQTT server details.

```cpp
// ---------------- CONFIG ----------------

const char* ssid = "YOUR_WIFI_SSID";          // Your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD";  // Your WiFi network password
const char* mqtt_server = "YOUR_MQTT_BROKER_IP"; // IP address or hostname of your MQTT broker
const char* MyHostName = "esp32-button-panel"; // Optional: A hostname for your ESP32 on the network
