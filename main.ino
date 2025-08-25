#include <WiFi.h>
#include <PubSubClient.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// ---------------- CONFIG ----------------
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* mqtt_server = "MQTT-IP";
const char* MyHostName = "Domain_name"; #optional


// MQTT topics
const char* mqtt_topics[9] = {
  "esp3228r/button1",
  "esp3228r/button2",
  "esp3228r/button3",
  "esp3228r/button4",
  "esp3228r/button5",
  "esp3228r/button6",
  "esp3228r/button7",
  "esp3228r/button8",
  "esp3228r/button9"
};

// TFT and Touch pins
#define XPT2046_IRQ   36
#define XPT2046_MOSI  32
#define XPT2046_MISO  39
#define XPT2046_CLK   25
#define XPT2046_CS    33
#define TFT_BL        21

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define SCREEN_ROTATION 1

#define TOUCH_X_MIN 200
#define TOUCH_X_MAX 3700
#define TOUCH_Y_MIN 240
#define TOUCH_Y_MAX 3800

// ---------------- OBJECTS ----------------
WiFiClient espClient;
PubSubClient client(espClient);
TFT_eSPI tft = TFT_eSPI();
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

// ---------------- BUTTON STRUCT ----------------
struct Button {
  int x, y, w, h;
  uint16_t baseColor;
  uint16_t highlightColor;
  const char* label;
  int radius;
};

Button buttons[9] = {
  // Ari
  {20,  40, 90, 50, TFT_GREEN,  tft.color565(100,255,100), "BUT1",  8},
  {115, 40, 90, 50, TFT_GREEN,  tft.color565(100,255,100), "BUT2",  8},
  {210, 40, 90, 50, TFT_GREEN,  tft.color565(100,255,100), "BUT3", 8},
  // Ellie
  {20,  100, 90, 50, TFT_YELLOW, tft.color565(255,255,100), "BUT4", 8},
  {115, 100, 90, 50, TFT_YELLOW, tft.color565(255,255,100), "BUT5", 8},
  {210, 100, 90, 50, TFT_YELLOW, tft.color565(255,255,100), "BUT6", 8},
  // Dad
  {20,  160, 90, 50, TFT_RED,    tft.color565(255,100,100), "BUT7", 8},
  {115, 160, 90, 50, TFT_RED,    tft.color565(255,100,100), "BUT8", 8},
  {210, 160, 90, 50, TFT_RED,    tft.color565(255,100,100), "BUT9", 8}
};


// ---------------- FUNCTION DECLARATIONS ----------------
void drawButton(Button btn, bool pressed=false);
void redrawScreen();
void prepareDeepSleep();
void handleTouch(TS_Point p);
void setup_wifi();
void reconnect();

// ---------------- DEEP SLEEP ----------------
void prepareDeepSleep() {
  Serial.println("Preparing for deep sleep...");
  
  // Turn off display
  tft.fillScreen(TFT_BLACK);
  digitalWrite(TFT_BL, LOW);
  
  // Disconnect from MQTT
  if (client.connected()) {
    client.disconnect();
  }
  
  // Disconnect WiFi
  WiFi.disconnect(true);
  delay(100);
  
  // Configure wakeup source
  esp_sleep_enable_ext0_wakeup((gpio_num_t)XPT2046_IRQ, LOW);
  
  Serial.println("Entering deep sleep...");
  esp_deep_sleep_start();
  // Code never reaches here
}

// ---------------- DRAW BUTTON ----------------
void drawButton(Button btn, bool pressed) {
  uint16_t color = pressed ? btn.highlightColor : btn.baseColor;

  // Shadow
  tft.fillRoundRect(btn.x + 2, btn.y + 2, btn.w, btn.h, btn.radius, TFT_DARKGREY);
  // Main button
  tft.fillRoundRect(btn.x, btn.y, btn.w, btn.h, btn.radius, color);
  // Border
  tft.drawRoundRect(btn.x, btn.y, btn.w, btn.h, btn.radius, TFT_WHITE);
  // Label
  tft.setTextColor(TFT_BLACK, color);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString(btn.label, btn.x + btn.w / 2, btn.y + btn.h / 2);
}

// ---------------- REDRAW SCREEN ----------------
void redrawScreen() {
  tft.fillRect(0, 0, SCREEN_WIDTH, 30, TFT_NAVY);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.setTextDatum(MC_DATUM);
  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Dads Buttons", SCREEN_WIDTH/2, 18);
  for(int i=0;i<9;i++) drawButton(buttons[i]);
}

// ---------------- TOUCH HANDLER ----------------
void handleTouch(TS_Point p) {
  int16_t x = map(p.x, TOUCH_X_MIN, TOUCH_X_MAX, 0, SCREEN_WIDTH);
  int16_t y = map(p.y, TOUCH_Y_MIN, TOUCH_Y_MAX, 0, SCREEN_HEIGHT);

  for (int i = 0; i < 9; i++) {
    if (x > buttons[i].x && x < (buttons[i].x + buttons[i].w) &&
        y > buttons[i].y && y < (buttons[i].y + buttons[i].h)) {

      drawButton(buttons[i], true);
      if (client.connected()) {
        client.publish(mqtt_topics[i], "PRESS");
      }
      delay(150);
      drawButton(buttons[i], false);
      delay(100);
    }
  }
}

// ---------------- WIFI ----------------
void setup_wifi() {
  Serial.printf("Connecting to %s", ssid);
  WiFi.setHostname(MyHostName);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
    }
  Serial.printf("\nWiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
}

// ---------------- MQTT ----------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_TFT")) {
      Serial.println("connected");
    } else {
      Serial.printf("failed, rc=%d, retrying...\n", client.state());
      delay(2000);
    }
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Initialize display
  tft.init();
  tft.setRotation(SCREEN_ROTATION);
  tft.fillScreen(TFT_BLACK);

  // Initialize touch
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(touchscreenSPI);
  ts.setRotation(SCREEN_ROTATION);

  // Connect to WiFi
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Draw UI
  redrawScreen();
}

// ---------------- LOOP ----------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check for touch
  if (ts.tirqTouched() && ts.touched()) {
    TS_Point p = ts.getPoint();
    handleTouch(p);
  }

  // If no activity for 10 seconds, go to sleep
  static unsigned long lastActivity = millis();
  if (ts.touched()) {
    lastActivity = millis();
  }
  else if (millis() - lastActivity > 18000) {
    prepareDeepSleep();
  }
}
