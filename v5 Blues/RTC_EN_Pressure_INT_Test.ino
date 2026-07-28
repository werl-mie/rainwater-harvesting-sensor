#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_SleepyDog.h>

#define RTC_INT_PIN     6   
#define BOOST_EN_PIN    5   
#define ANALOG_PIN      A2  

RTC_PCF8523 rtc;

void wakeUpISR() {
  // Keep empty to speed up the hardware wake vector
}

void setup() {
  // 1. Establish the Miniboost state safely BEFORE touching the RTC
  pinMode(BOOST_EN_PIN, OUTPUT);
  digitalWrite(BOOST_EN_PIN, LOW); 
  
  // 2. FORCE THE SAMD21 INTERNAL PULL-UP STRENGTH VIA PORT DIRECTIVES
  pinMode(RTC_INT_PIN, INPUT_PULLUP);
  // Low-level register override: Ensures the pull-up samples continuously in standby
  PORT->Group[g_APinDescription[RTC_INT_PIN].ulPort].PINCFG[g_APinDescription[RTC_INT_PIN].ulPin].bit.INEN = 1;
  PORT->Group[g_APinDescription[RTC_INT_PIN].ulPort].PINCFG[g_APinDescription[RTC_INT_PIN].ulPin].bit.PULLEN = 1;

  Serial.begin(115200);
  
  // Emergency recovery window (prevents the USB bricking cycle)
  for (int i = 0; i < 4; i++) { delay(1000); }

  if (!rtc.begin()) { while (1); }
  if (!rtc.initialized() || rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.writeSqwPinMode(PCF8523_OFF);
}

void loop() {
  // ==========================================
  // STEP A: ISOLATED POWER TRANSITION & SENSE
  // ==========================================
  // Temporarily disable the pin interrupt so the Miniboost spike is ignored
  detachInterrupt(digitalPinToInterrupt(RTC_INT_PIN));
  
  digitalWrite(BOOST_EN_PIN, HIGH); 
  delay(500); // Give the 5V booster plenty of time to rise and smooth out
  
  int sensorValue = analogRead(ANALOG_PIN);
  
  if (Serial) {
    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);
    delay(20); 
  }

  digitalWrite(BOOST_EN_PIN, LOW); // Collapse the 5V rail
  delay(150); // CRITICAL: Sleep must not occur until the power rail decay settles

  // ==========================================
  // STEP B: CLEAN ARMS FOR THE NEXT WAKEUP
  // ==========================================
  rtc.disableCountdownTimer(); 
  rtc.enableCountdownTimer(PCF8523_FrequencySecond, 10); 

  // Force-clear any lingering hardware interrupt flag in the EIC controller
  EIC->INTFLAG.reg = (1 << g_APinDescription[RTC_INT_PIN].ulPin); 
  
  // Re-attach the trigger now that the electrical environment is quiet
  attachInterrupt(digitalPinToInterrupt(RTC_INT_PIN), wakeUpISR, FALLING);

  // USB Isolation Routine
  if (Serial) { Serial.end(); }
  USBDevice.detach(); 
  delay(100);         

  // ==========================================
  // STEP C: STANDBY SLEEP
  // ==========================================
  Watchdog.sleep(); 

  // ==========================================
  // STEP D: IMMEDIATE POST-WAKE RECOVERY
  // ==========================================
  USBDevice.attach();
  Serial.begin(115200);
  
  // Instantly strip the interrupt so the next power cycle doesn't cause a trip
  detachInterrupt(digitalPinToInterrupt(RTC_INT_PIN));
  rtc.disableCountdownTimer(); 
  rtc.writeSqwPinMode(PCF8523_OFF); 
  
  // Wait up to 2 seconds for a serial window if troubleshooting
  unsigned long usbWait = millis();
  while (!Serial && (millis() - usbWait < 2000));
}
