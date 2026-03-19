#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "GroveLora.h"

#define PIN_SD_CS 4 
#define PIN_RTC_INT 5
#define PIN_SWITCH 12  
#define PIN_ANALOG_TURB A1 //NOT USED
#define PIN_ANALOG_PRES A0
#define BH1750_ADDRESS 0x23

RTC_PCF8523 rtc;
GroveLora dev;
bool sd_active = false;
int lastSwitchState;
String inputBuffer = ""; 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  for (int i = 0; i < 40; i++) {
    digitalWrite(LED_BUILTIN, HIGH); delay(125);
    digitalWrite(LED_BUILTIN, LOW); delay(125);
  }

  Serial.begin(115200);
  Serial1.begin(9600); 
  Wire.begin();
  
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(PIN_RTC_INT, INPUT_PULLUP);
  lastSwitchState = digitalRead(PIN_SWITCH);

  if (rtc.begin()) {
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //ONLY SET ONCE 
    rtc.deconfigureAllTimers();
    rtc.enableCountdownTimer(PCF8523_FrequencySecond, 15);
  }
  if (SD.begin(PIN_SD_CS)) sd_active = true;

  dev.sendCmd("AT+MODE=TEST\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RFCFG,915,SF7,125,12,15,14,ON,OFF,OFF\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RXLRPKT\r\n");
  
  Serial.println("Source,SensorName,UnixTime,Value");
}

void loop() {
  int currentSwitch = digitalRead(PIN_SWITCH);
  if (currentSwitch != lastSwitchState) {
    logData("LOCAL", "Float", currentSwitch == LOW ? "FAR" : "CLOSE");
    lastSwitchState = currentSwitch;
  }

  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) parseLoraLine(inputBuffer);
      inputBuffer = ""; 
    } else {
      inputBuffer += c;
    }
  }

  if (digitalRead(PIN_RTC_INT) == LOW) {
    readTimerSensors();
  }

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

      String label = "UNKNOWN";
      String statusStr = String(mVal); 

      if (mType == 1) {
        label = "REMOTE_BUCKET";
      } 
      else if (mType == 2) {
        label = "REMOTE_LEAF";
        statusStr = (mVal == LOW) ? "PRESSED" : "LIFTED";
      } 
      else if (mType == 3) {
        label = "REMOTE_FLOAT_TOP";
        statusStr = (mVal == LOW) ? "FAR" : "CLOSE";
      } 
      else if (mType == 4) {
        label = "REMOTE_FLOAT_BOT";
        statusStr = (mVal == LOW) ? "FAR" : "CLOSE";
      }

      logData("LORA", label, statusStr);
    }
    dev.sendCmd("AT+TEST=RXLRPKT\r\n");
  }
}

void readTimerSensors() {
  // TURBID
  int sensorValue = analogRead(PIN_ANALOG_TURB);
  float voltage_aftparallel = sensorValue * (3.3 / 1023.0);
  float R_eq = 1/(1/4.7+ 1/3.6); 
  float voltage = voltage_aftparallel * (4.7/R_eq); 

  //convert to NTU...
  logData("LOCAL", "Turbidity", String(voltage)); //in voltage

  // --- PRESSURE SENSOR (Analog) ---
  int HeightRaw = analogRead(PIN_ANALOG_PRES); 
  float HeightVal = (float(HeightRaw) / 1023.0 * 5.0) ; 
  float PressureVal = 1.0/0.7 * HeightVal; //psi conversion 
  logData("LOCAL", "Pressure", String(PressureVal));//in psi 

  // --- LUX SENSOR (I2C) ---
  Wire.beginTransmission(BH1750_ADDRESS);
  Wire.write(0x10); 
  Wire.endTransmission(false); 

  delay(20); // Mandatory sensor processing delay 
  
  Wire.requestFrom(BH1750_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint16_t raw = (Wire.read() << 8) | Wire.read();
    float lux = raw / 1.2; // Standard BH1750 scaling factor
    logData("LOCAL", "Lux", String(lux));//in lx
  } else {
    logData("LOCAL", "Lux", "ERR_TIMEOUT");
  }

}
void logData(String src, String sensor, String val) {
  DateTime now = rtc.now();
  String humanTime = now.timestamp(); 
  String entry = src + "," + sensor + "," + humanTime + "," + val;
  
  Serial.println(entry);
  
  if (sd_active) {
    File f = SD.open("datalog.csv", FILE_WRITE);
    if (f) { 
      f.println(entry); 
      f.close(); 
    }
  }
}
