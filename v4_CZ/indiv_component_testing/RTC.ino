#include <Wire.h>
#include "RTClib.h"

RTC_PCF8523 rtc;

void setup() {
  Serial.begin(115200);
  
  // Wait for Serial Monitor to open
  while (!Serial); 
  delay(1000);

  Serial.println("--- RTC SYNC TOOL ---");

  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC! Check PCB/Wiring.");
    while (1);
  }

  // --- THE SYNC COMMAND ---
  // This line takes the date and time from your computer at the second of compilation
  Serial.println("Synchronizing RTC with computer time...");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // Start the internal oscillator (just in case it was stopped)
  rtc.start(); 

  Serial.println("Sync Complete!");
  Serial.println("Current Time (YY/MM/DD HH:MM:SS):");
}

void loop() {
  DateTime now = rtc.now();

  // Print the time every second to verify it is counting
  char buf[] = "YYYY/MM/DD  hh:mm:ss";
  Serial.println(now.toString(buf));

  delay(1000);
}
