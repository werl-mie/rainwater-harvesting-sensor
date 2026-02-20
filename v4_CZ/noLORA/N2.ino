#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "ArduinoLowPower.h"


// --- PINS ---
#define PIN_SD_CS 4     // CORRECTED: Pin 4 for Feather M0 Adalogger SD
#define PIN_RTC_INT 5
#define PIN_SWITCH 12
#define PIN_ANALOG1 A1
#define PIN_ANALOG2 A2
#define BH1750_ADDRESS 0x23


// --- GLOBALS ---
RTC_PCF8523 rtc;
volatile bool wakeUpByRTC = false;
volatile bool wakeUpBySwitch = false;
bool sd_active = false;


float lastLux = 0.0;
int lastAnalog1 = 0;
int lastAnalog2 = 0;
const int analogThreshold = 50;
const float luxThreshold = 0.5;


// --- ISRs ---
void rtc_isr() { wakeUpByRTC = true; }
void switch_isr() { wakeUpBySwitch = true; }


// --- HELPER: READ I2C REGISTER ---
uint8_t readReg(uint8_t reg, void* pBuf, size_t size) {
  if (pBuf == NULL) return 0;
  uint8_t * _pBuf = (uint8_t *)pBuf;
  Wire.beginTransmission(BH1750_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission() != 0) return 0;
  delay(180);
  Wire.requestFrom((uint8_t)BH1750_ADDRESS, (uint8_t)size);
  for (uint16_t i = 0; i < size; i++) {
    if (Wire.available()) {
      _pBuf[i] = Wire.read();
    }
  }
  return size;
}


void setup() {
  Serial.begin(57600);
  Wire.begin();
 
  uint32_t start = millis();
  while (!Serial && (millis() - start < 3000));


  Serial.println("\n--- PIN 4 FIX: SD INITIALIZING ---");
 
  pinMode(PIN_RTC_INT, INPUT_PULLUP);
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);


  if (!rtc.begin()) {
    Serial.println("RTC Not Found!");
    while (1);
  }
 
  rtc.deconfigureAllTimers();
  rtc.enableCountdownTimer(PCF8523_FrequencySecond, 15);


  // --- SD INITIALIZATION (Pin 4) ---
  Serial.print("Initializing SD card on Pin 4...");
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println(" still failed! Check card/format.");
    sd_active = false;
  } else {
    Serial.println(" success!");
    sd_active = true;


    if (!SD.exists("datalog.csv")) {
      File dataFile = SD.open("datalog.csv", FILE_WRITE);
      if (dataFile) {
        dataFile.println("Sensor,UnixTime,Value");
        dataFile.close();
      }
    }
  }


  // ATTACH WAKEUP INTERRUPTS
  LowPower.attachInterruptWakeup(PIN_SWITCH, switch_isr, CHANGE);
  LowPower.attachInterruptWakeup(PIN_RTC_INT, rtc_isr, FALLING);


  lastAnalog1 = analogRead(PIN_ANALOG1);
  lastAnalog2 = analogRead(PIN_ANALOG2);
}


void loop() {
  // LowPower.sleep(); // Keep commented for debugging Serial
 
  delay(10);


  DateTime now = rtc.now();
  uint32_t ts = now.unixtime();


  if (wakeUpBySwitch) {
    digitalWrite(LED_BUILTIN, HIGH);
    int state = digitalRead(PIN_SWITCH);
    logEvent("SWITCH," + String(ts) + "," + (state == LOW ? "FAR" : "CLOSE"));
    wakeUpBySwitch = false;
    digitalWrite(LED_BUILTIN, LOW);
  }


  if (wakeUpByRTC) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("--- TIMER TRIGGERED ---");


    int a1 = analogRead(PIN_ANALOG1);
    if (abs(a1 - lastAnalog1) > analogThreshold) {
      logEvent("Pressure," + String(ts) + "," + String(a1));
      lastAnalog1 = a1;
    }


    int a2 = analogRead(PIN_ANALOG2);
    if (abs(a2 - lastAnalog2) > analogThreshold) {
      logEvent("Turbidity," + String(ts) + "," + String(a2));
      lastAnalog2 = a2;
    }


    uint8_t buf[2];
    if (readReg(0x10, buf, 2) > 0) {
      uint16_t data = (uint16_t)buf[0] << 8 | buf[1];
      float currentLux = data / 1.2;
      if (abs(currentLux - lastLux) > luxThreshold) {
        logEvent("LUX," + String(ts) + "," + String(currentLux));
        lastLux = currentLux;
      }
    }
   
    wakeUpByRTC = false;
    digitalWrite(LED_BUILTIN, LOW);
  }
}


void logEvent(String data) {
  Serial.println(data);
  if (sd_active) {
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println(data);
      dataFile.close();
    }
  }
}


15s timer delay 
