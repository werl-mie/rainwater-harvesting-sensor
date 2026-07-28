#include <Wire.h>
#include "RTClib.h"

RTC_PCF8523 rtc;

void setup() {
  Serial.begin(115200);

  // Wait for Serial Monitor
  while (!Serial) {
    delay(10);
  }

  Serial.println("RTC PCF8523 Test");

  // Start I2C + RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  Serial.println("RTC found!");

  // OPTIONAL:
  // Set RTC to compile time the first upload
  // Uncomment once, upload, then comment again

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (!rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC lost power, setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  delay(1000);
}