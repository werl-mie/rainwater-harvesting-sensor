/**************************************************************************/
/*
  Countdown Timer using a PCF8523 RTC connected via I2C and Wire lib
  with the INT/SQW pin wired to an interrupt-capable input.

  According to the data sheet, the PCF8523 can run countdown timers
  from 244 microseconds to 10.625 days:
  https://www.nxp.com/docs/en/data-sheet/PCF8523.pdf#page=34

  This sketch sets a countdown timer, and executes code when it reaches 0,
  then blinks the built-in LED like BlinkWithoutDelay, but without millis()!

  NOTE:
  You must connect the PCF8523's interrupt pin to your Arduino or other
  microcontroller on an input pin that can handle interrupts, and that has a
  pullup resistor. The pin will be briefly pulled low each time the countdown
  reaches 0. This example will not work without the interrupt pin connected!

  On Adafruit breakout boards, the interrupt pin is labeled 'INT' or 'SQW'.
*/
/**************************************************************************/

#include "RTClib.h"

RTC_DS3231 rtc;

// Input pin with interrupt capability
// const int timerInterruptPin = 2;  // Most Arduinos
const int timerInterruptPin = 5;  // Adafruit Feather M0/M4/nRF52840

// Variables modified during an interrupt must be declared volatile
volatile bool countdownInterruptTriggered = false;
volatile int numCountdownInterrupts = 0;

void setup () {
  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  rtc.disable32K();

  pinMode(LED_BUILTIN, OUTPUT);

  // Set the pin attached to PCF8523 INT to be an input with pullup to HIGH.
  // The PCF8523 interrupt pin will briefly pull it LOW at the end of a given
  // countdown period, then it will be released to be pulled HIGH again.
  pinMode(timerInterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(timerInterruptPin), countdownOver, FALLING);

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.disableAlarm(2);

  Serial.println(F("\nStarting PCF8523 Countdown Timer example."));
  Serial.print(F("Configured to expect PCF8523 INT/SQW pin connected to input pin: "));
  Serial.println(timerInterruptPin);
  Serial.println(F("This example will not work without the interrupt pin connected!\n\n"));

  rtc.setAlarm1(rtc.now() + TimeSpan(2),DS3231_A1_Second);


  Serial.println(F("First, use the PCF8523's 'Countdown Timer' with an interrupt."));
  Serial.println(F("Set the countdown for 10 seconds and we'll let it run for 2 rounds."));
  Serial.println(F("Starting Countdown Timer now..."));

  

  // This message proves we're not blocked while counting down!
  Serial.println(F("  While we're waiting, a word of caution:"));
  Serial.println(F("  When starting a new countdown timer, the first time period is not of fixed"));
  Serial.println(F("  duration. The amount of inaccuracy for the first time period is up to one full"));
  Serial.println(F("  clock frequency. Example: just the first second of the first round of a new"));
  Serial.println(F("  countdown based on PCF8523_FrequencySecond may be off by as much as 1 second!"));
  Serial.println(F("  For critical timing, consider starting actions on the first interrupt."));
}

// Triggered by the PCF8523 Countdown Timer interrupt at the end of a countdown
// period. Meanwhile, the PCF8523 immediately starts the countdown again.
void countdownOver () {
  // Set a flag to run code in the loop():
  countdownInterruptTriggered = true;
  numCountdownInterrupts ++;
}


void loop () {
  
  if (countdownInterruptTriggered){
    if (numCountdownInterrupts % 2 == 1){
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    
    rtc.clearAlarm(1);
    rtc.setAlarm1(rtc.now() + TimeSpan(1),DS3231_A1_Second);

    countdownInterruptTriggered = false;
  }

  // digitalWrite(LED_BUILTIN, !digitalRead(timerInterruptPin));
}
