#include <Wire.h>
#include "RTClib.h"

RTC_PCF8523 rtc;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial monitor to open
  delay(1000);

  Serial.println("\n--- RTC HARDWARE TEST ---");

  Wire.begin();

  // 1. Check physical connection
  Serial.print("Scanning I2C bus for 0x68... ");
  Wire.beginTransmission(0x68);
  if (Wire.endTransmission() == 0) {
    Serial.println("FOUND!");
  } else {
    Serial.println("NOT FOUND. Check your PCB traces/soldering.");
    while (1); // Stop here if not found
  }

  // 2. Initialize Library
  if (!rtc.begin()) {
    Serial.println("Library could not find PCF8523!");
    while (1);
  }

  // 3. Check if the clock is "Stopped"
  if (!rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC was stopped/power lost. Setting to compile time...");
    // This kickstarts the internal oscillator
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC is already initialized and running.");
  }

  Serial.println("Starting clock output (1 second interval):");
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" - ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(1000);
}
