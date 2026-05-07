#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "GroveLora.h"

#define PIN_SD_CS 4 
#define PIN_RTC_INT 11     
// #define PIN_ANALOG_TURB A1 
#define PIN_ANALOG_PRES A0
#define BH1750_ADDRESS 0x23

RTC_PCF8523 rtc;
GroveLora dev;
bool sd_active = false;
String inputBuffer = ""; 

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); 
  
  Serial1.begin(9600); 
  Serial1.setTimeout(50);
  Wire.begin();
  
  pinMode(PIN_RTC_INT, INPUT_PULLUP); // Pin 5

  if (rtc.begin()) {
    Serial.println("RTC Initialized.");
    // Sync to compile time automatically
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Clean start for the timer
    rtc.deconfigureAllTimers();
    rtc.enableCountdownTimer(PCF8523_FrequencySecond, 15);
  }

  if (SD.begin(PIN_SD_CS)) sd_active = true;

  dev.sendCmd("AT+MODE=TEST\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RFCFG,915,SF7,125,12,15,14,ON,OFF,OFF\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RXLRPKT\r\n");
  
  Serial.println("Source,SensorName,Time,Value");
  readTimerSensors(); // Initial verification
}

void loop() {
  // 1. LORA PROCESSING
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) parseLoraLine(inputBuffer);
      inputBuffer = ""; 
    } else { inputBuffer += c; }
  }

  // 2. THE 15S TRIGGER
  if (digitalRead(PIN_RTC_INT) == LOW) {
    // STEP A: Stop the timer to release Pin 5
    rtc.disableCountdownTimer();
    
    // STEP B: Read the sensors
    readTimerSensors();
    
    // STEP C: Clear all flags and restart the 15s countdown
    rtc.deconfigureAllTimers();
    rtc.enableCountdownTimer(PCF8523_FrequencySecond, 15);
    
    Serial.println("--- Restarting 15s Countdown ---");
  }

  // Safety: Reset RX mode every 30s
  static uint32_t lastRXReset = 0;
  if (millis() - lastRXReset > 30000) {
    dev.sendCmd("AT+TEST=RXLRPKT\r\n");
    lastRXReset = millis();
  }
}

void parseLoraLine(String line) {
  if (line.indexOf("RX \"") != -1) {
    int startQuote = line.indexOf("\"") + 1;
    int endQuote = line.indexOf("\"", startQuote);
    String rawHex = line.substring(startQuote, endQuote);
    if (rawHex.length() >= 20) {
      int mType = (int)strtol(rawHex.substring(14, 16).c_str(), NULL, 16);
      int mVal  = (int)strtol(rawHex.substring(16, 20).c_str(), NULL, 16);
      logData("LORA", "REMOTE_" + String(mType), String(mVal));
    }
  }
}

void readTimerSensors() {
  // Analog Reads
  logData("LOCAL", "Turbidity", String(analogRead(PIN_ANALOG_TURB) * (3.3 / 1023.0)));
  logData("LOCAL", "Pressure", String((analogRead(PIN_ANALOG_PRES) / 1023.0 * 5.0) / 0.7));

  // I2C Lux Read
  Wire.beginTransmission(BH1750_ADDRESS);
  Wire.write(0x10); 
  if (Wire.endTransmission() == 0) {
    delay(30);
    Wire.requestFrom(BH1750_ADDRESS, 2);
    if (Wire.available() == 2) {
      uint16_t rawLux = (Wire.read() << 8) | Wire.read();
      logData("LOCAL", "Lux", String(rawLux / 1.2));
    } else { logData("LOCAL", "Lux", "TIMEOUT"); }
  } else { logData("LOCAL", "Lux", "DISCONNECTED"); }
}

void logData(String src, String sensor, String val) {
  DateTime now = rtc.now();
  char timestamp[25];
  sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02d", 
          now.year(), now.month(), now.day(), 
          now.hour(), now.minute(), now.second());

  String entry = src + "," + sensor + "," + String(timestamp) + "," + val;
  Serial.println(entry);
  if (sd_active) {
    File f = SD.open("datalog.csv", FILE_WRITE);
    if (f) { f.println(entry); f.close(); }
  }
}
