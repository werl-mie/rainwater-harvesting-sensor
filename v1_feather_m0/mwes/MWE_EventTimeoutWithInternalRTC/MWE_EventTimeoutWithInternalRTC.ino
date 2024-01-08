/*
  Adapted from SimpleRTC from RTCZero Library
*/

#include <RTCZero.h>
#include "ArduinoLowPower.h"

#define PIN_FLOAT1 6

/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
const byte seconds = 50;
const byte minutes = 59;
const byte hours = 16;

/* Change these values to set the current initial date */
const byte day = 15;
const byte month = 6;
const byte year = 15;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_FLOAT1, INPUT_PULLUP);

  Serial.begin(9600);

  // Internal RTC config
  rtc.begin(); // initialize RTC
  rtc.setDate(day, month, year);
  rtc.setTime(hours, minutes, seconds);
  rtc.attachInterrupt(isr_alarm);

  LowPower.attachInterruptWakeup(PIN_FLOAT1, isr_float, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT1), isr_float, CHANGE);
  
}

void loop()
{
  // Works with both methods (I'm assuming they are basically the same under the hood)
  // rtc.standbyMode();
  LowPower.sleep();
}

void isr_alarm(){
  digitalWrite(LED_BUILTIN, LOW);
}

void isr_float(){
  digitalWrite(LED_BUILTIN, HIGH);
  setIncrementalAlarm(3);
}

// Will set with only match for MMSS
// For now only supports up to 60 second alarms

void setIncrementalAlarm(byte increment_s)
{
  // Not sure this is necessary...
  rtc.disableAlarm();

  byte seconds_new = rtc.getSeconds() + increment_s - 1;
  byte minutes_new = rtc.getMinutes();

  if (seconds_new > 59){
    seconds_new = seconds_new - 60;
    minutes_new = minutes_new + 1;
  }

  if (minutes_new > 59){
    minutes_new = 0;
  }

  // Serial.print("Current time: ");
  // print2digits(rtc.getHours());
  // Serial.print(":");
  // print2digits(rtc.getMinutes());
  // Serial.print(":");
  // print2digits(rtc.getSeconds());
  // Serial.print(" Setting alarm for time (MM:SS): ");
  // print2digits(minutes_new);
  // Serial.print(":");
  // print2digits(seconds_new);
  // Serial.println();

  rtc.setAlarmTime(rtc.getHours(), minutes_new, seconds_new);
  rtc.enableAlarm(rtc.MATCH_MMSS);
}


void print2digits(int number) {
  if (number < 10) {
    Serial.print("0"); // print a 0 before if the number is < than 10
  }
  Serial.print(number);
}