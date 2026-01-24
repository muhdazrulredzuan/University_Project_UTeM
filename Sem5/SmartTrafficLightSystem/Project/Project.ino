#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"

// ================= WIFI =================
const char* ssid = "HONOR 400 Pro";
const char* password = "aaaaaaaa";
const char* serverUrl = "http://10.27.105.153:1880/update";

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= TRAFFIC LIGHT =================
int ledRed = 14;
int ledYellow = 12;
int ledGreen = 13;
int irSensor = 2;
Servo barrier;

enum TrafficState { RED, GREEN, GREEN_BLINK, YELLOW };
TrafficState currentState = RED;

unsigned long stateMillis = 0;
unsigned long redTime = 5000;
unsigned long greenTime = 5000;

bool pedestrianWaiting = false;
bool pedestrianReported = false;
bool barrierUp = false;

// ================= LOAD CELL =================
HX711 scale;
int loadCellDT = 32;
int loadCellSCK = 33;
float weightThreshold = 50.0;
bool vehicleDetected = false;

// ================= STREET LIGHT =================
int ldrPin = 34;
int streetLedPin = 25;
int streetBrightness = 0;

// ================= TIMERS =================
unsigned long oledMillis = 0;
unsigned long sendMillis = 0;
unsigned long loadMillis = 0;
unsigned long ldrMillis = 0;
unsigned long blinkMillis = 0;
bool blinkState = false;
int blinkCount = 0;

// ================= SETUP =================
void setup() {
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(irSensor, INPUT_PULLUP);
  pinMode(streetLedPin, OUTPUT);

  barrier.attach(27);
  barrier.write(0);

  scale.begin(loadCellDT, loadCellSCK);
  scale.set_scale();
  scale.tare();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);

  WiFi.begin(ssid, password);
}

// ================= LOOP =================
void loop() {
  unsigned long now = millis();

  trafficLight(now);
  readLoadCell(now);
  streetLight(now);
  updateOLED(now);
  sendToNodeRed(now);
}

// ================= TRAFFIC LIGHT =================
void trafficLight(unsigned long now) {

  if (digitalRead(irSensor) == LOW) {
    pedestrianWaiting = true;
    pedestrianReported = true;
  }

  switch (currentState) {

    case RED:
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, LOW);

      redTime = pedestrianWaiting ? 8000 : 5000;

      if (pedestrianWaiting) {
        barrier.write(90);
        barrierUp = true;
      } else {
        barrier.write(0);
        barrierUp = false;
      }

      if (now - stateMillis >= redTime) {
        stateMillis = now;
        pedestrianWaiting = false;
        barrier.write(0);
        barrierUp = false;
        currentState = GREEN;
      }
      break;

    case GREEN:
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, LOW);
      digitalWrite(ledGreen, HIGH);

      greenTime = vehicleDetected ? 8000 : 5000;

      if (now - stateMillis >= greenTime) {
        stateMillis = now;
        blinkMillis = now;
        blinkCount = 0;
        blinkState = false;
        currentState = GREEN_BLINK;
      }
      break;

    case GREEN_BLINK:
      if (now - blinkMillis >= 200) {
        blinkMillis = now;
        blinkState = !blinkState;
        digitalWrite(ledGreen, blinkState);
        blinkCount++;
      }

      if (blinkCount >= 10) {
        digitalWrite(ledGreen, LOW);
        stateMillis = now;
        currentState = YELLOW;
      }
      break;

    case YELLOW:
      digitalWrite(ledRed, LOW);
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, LOW);

      if (now - stateMillis >= 3000) {
        stateMillis = now;
        currentState = RED;
      }
      break;
  }
}

// ================= LOAD CELL =================
void readLoadCell(unsigned long now) {
  if (now - loadMillis < 500) return;
  loadMillis = now;

  if (scale.is_ready()) {
    vehicleDetected = scale.get_units(1) >= weightThreshold;
  }
}

// ================= STREET LIGHT =================
void streetLight(unsigned long now) {
  if (now - ldrMillis < 200) return;
  ldrMillis = now;

  int ldr = analogRead(ldrPin);

  if (ldr > 900) streetBrightness = 0;
  else if (ldr < 200) streetBrightness = 255;
  else streetBrightness = 60;

  analogWrite(streetLedPin, streetBrightness);
}

// ================= OLED =================
void updateOLED(unsigned long now) {
  if (now - oledMillis < 200) return;
  oledMillis = now;

  long total = 0;
  long remain = 0;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);

  if (currentState == RED) {
    display.print("RED");
    total = redTime;
  } else if (currentState == GREEN) {
    display.print("GREEN");
    display.setCursor(60,0);
    display.print("GO!!!");
    total = greenTime;
  } else if (currentState == GREEN_BLINK) {
    display.print("GREEN");
    display.setCursor(60,0);
    display.print("WAIT");
    total = 2000;
  } else {
    display.print("YELLOW");
    display.setCursor(60,0);
    display.print("SLOW");
    display.setCursor(60,8);
    display.print("DOWN");
    total = 3000;
  }

  remain = total - (now - stateMillis);
  if (remain < 0) remain = 0;

  display.setTextSize(2);
  display.setCursor(50,16);
  display.print(remain / 1000);

  // Loading bar
  display.drawRect(14, 42, 100, 6, SSD1306_WHITE);
  int fill = map(remain, 0, total, 0, 100);
  display.fillRect(14, 42, fill, 6, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Street:");
  display.print(streetBrightness);

  display.display();
}

// ================= NODE-RED =================
void sendToNodeRed(unsigned long now) {
  if (now - sendMillis < 1000) return;
  sendMillis = now;

  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.setTimeout(300);

  long countdown = 0;
  if (currentState == RED) countdown = redTime - (now - stateMillis);
  else if (currentState == GREEN) countdown = greenTime - (now - stateMillis);
  else if (currentState == YELLOW) countdown = 3000 - (now - stateMillis);
  else countdown = 2000 - (now - blinkMillis);
  if (countdown < 0) countdown = 0;

  String url = String(serverUrl) +
    "?traffic=" + (currentState==RED?"RED":currentState==GREEN?"GREEN":currentState==YELLOW?"YELLOW":"GREEN_BLINK") +
    "&pedestrian=" + String(pedestrianReported) +
    "&vehicle=" + String(vehicleDetected) +
    "&street=" + String(streetBrightness) +
    "&barrier=" + String(barrierUp) +
    "&countdown=" + String(countdown/1000);

  http.begin(url);
  http.GET();
  http.end();

  pedestrianReported = false;
}
