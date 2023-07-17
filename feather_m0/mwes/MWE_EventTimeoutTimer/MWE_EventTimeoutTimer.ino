/*
 * MWE_EventTimeoutTimer
 *
 * Initiates timer (turning LED ON) whenever there is an input interrupt. Timer expiry is indicated by turning LED OFF.
 *
 */

#include <arduino-timer.h>

#define PIN_SENSOR 6

auto timer = timer_create_default(); // create a timer with default settings
// auto task;

bool isr_timeout(void *){
  digitalWrite(LED_BUILTIN, LOW);
  return true;
}

void isr_event() {
  digitalWrite(LED_BUILTIN, HIGH);
  timer.cancel();
  timer.in(2000, isr_timeout);
}


void setup() {
  pinMode(PIN_SENSOR, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT
  digitalWrite(LED_BUILTIN, LOW);

  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR), isr_event, CHANGE);

  // call the toggle_led function every 1000 millis (1 second)
  // timer.every(1000, toggle_led);
}

void loop() {
  timer.tick(); // tick the timer
}
