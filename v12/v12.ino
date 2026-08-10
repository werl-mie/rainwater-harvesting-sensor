// Required Libraries
#include "ArduinoLowPower.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_SleepyDog.h>
#include <Notecard.h>

#include "GlobalVars.h"
#include "Types.h"
#include "RTCManager.h"
#include "Logger.h"
#include "NotecardManager.h"
#include "InterruptManager.h"


// Global Managers
RTCManager rtcManager;
Logger logger;
NotecardManager notecardManager;
InterruptManager interruptManager;


// GLOBAL VARIABLES
volatile int bucketCount = 0;
unsigned long globalRecordID = 0;
unsigned long lastBucketTime = 0;
volatile bool flag_event_change = false;
volatile bool rtcTriggered = false;
volatile uint32_t rtcISRCount = 0;
volatile uint32_t lastRTCIsrMillis = 0;
volatile uint32_t rtcISRGapMs = 0;

void setup() {
  // Serial
  Serial.begin(115200);

  if (DEBUG_MODE){
    delay(100);
    Serial.println();
    Serial.println(F("Starting..."));
  }

  Watchdog.enable(16000);
  pinMode(LED_BUILTIN, OUTPUT);

  SPI.begin();

  //Analog configuration
  analogReadResolution(12);
  analogReference(AR_INTERNAL2V23);

  //Initialize Managers

  //1. SD Manager
  if (!logger.begin()){
    if (DEBUG_MODE){
      Serial.println(F("ERROR: SD init failed"));
    }
  }
  //2. RTC Manager
  pinMode(PIN_RTC_INT, INPUT_PULLUP);
  if (!rtcManager.begin()){
    if (DEBUG_MODE){
      Serial.println(F("ERROR: RTC init failed"));
    }
  }

  //5. Notecard Manager
  if(!notecardManager.begin()){
    if (DEBUG_MODE){
      Serial.println(F("ERROR: Notecard init failed"));
    }
  }

  //Assign pinmodes
  pinMode(PIN_LEAF, INPUT_PULLUP);
  pinMode(PIN_FLOAT_TOP, INPUT_PULLUP);
  pinMode(PIN_FLOAT_BOTTOM, INPUT_PULLUP);
  pinMode(PIN_BUCKET, INPUT_PULLUP);
  pinMode(PIN_CHLO, INPUT);
  pinMode(PIN_MINIBOOST_ENABLE, OUTPUT);

  //Mini boost off @ start
  digitalWrite(PIN_MINIBOOST_ENABLE, LOW);
  //Locks register active through standby sleep
  PORT->Group[g_APinDescription[PIN_MINIBOOST_ENABLE].ulPort].PINCFG[g_APinDescription[PIN_MINIBOOST_ENABLE].ulPin].bit.INEN = 1;



  // Assign Digital Wakeup Interrupts
  LowPower.attachInterruptWakeup(PIN_LEAF, isr_generic, CHANGE);
  LowPower.attachInterruptWakeup(PIN_FLOAT_BOTTOM, isr_generic, CHANGE);
  LowPower.attachInterruptWakeup(PIN_FLOAT_TOP, isr_generic, CHANGE);
  LowPower.attachInterruptWakeup(PIN_CHLO, isr_generic, CHANGE);

  // RTC Wakeup Interrupt
  LowPower.attachInterruptWakeup(PIN_RTC_INT, isr_rtc, FALLING);

  //Bucket Mediated Interrupt
  LowPower.attachInterruptWakeup(PIN_BUCKET, isr_bucket, FALLING);

  if (DEBUG_MODE){
    Serial.println(F("Clearing flags after interrupt attach!"));
  }

  // NOTE: this no longer re-arms the countdown (that already happened once
  // inside rtcManager.begin()) - it only clears flags so a fresh edge is
  // recognized correctly from here on.
  rtcManager.resetTimer();

  if (DEBUG_MODE){
    Serial.println(F("Initialization completed!"));
  }

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(20);
  digitalWrite(LED_BUILTIN, LOW); 

  Watchdog.reset();

  bool processRTC = false;
  bool processDIG = false;


  //Process digital interrupts
  if(flag_event_change){
    noInterrupts();
    processDIG = true;
    flag_event_change = false;
    interrupts();
  }

  //RTC timer event
  if(rtcTriggered){
    noInterrupts();
    processRTC = true;
    rtcTriggered = false;
    interrupts();

    if (DEBUG_MODE){
      Serial.println(F("RTC software flag CLEARED"));

      Serial.print(F("rtcTriggered immediately after clear = "));
      Serial.println(rtcTriggered);

      Serial.print(F("rtcISRCount = "));
      Serial.println(rtcISRCount);

      Serial.print(F("RTC ISR Gap: "));
      Serial.println(rtcISRGapMs);
    }

  }

  if (processDIG){
    interruptManager.processEventAndLog(false);
  }

  if (processRTC){
    interruptManager.processEventAndLog(true);
  }


  if (DEBUG_MODE){
    Serial.flush();
  }


  Watchdog.disable();


  delay(200);

  if (DEBUG_MODE){
    Serial.print(F("Before sleep check: rtcTriggered="));
    Serial.print(rtcTriggered);
    Serial.print(F(", flag_event_change="));
    Serial.println(flag_event_change);
  }

  if (!rtcTriggered && !flag_event_change){
    if (DEBUG_MODE){
      Serial.println(F("Ctrl state before sleep"));
      rtcManager.printControl2();
      rtcManager.debugTimerBRegs();
      rtcManager.debugClkOutControl();

      Serial.print(F("RTC INT pin state:"));
      Serial.println(digitalRead(PIN_RTC_INT));

      Serial.print(F("Flag event state:"));
      Serial.println(flag_event_change);

      Serial.print(F("rtcISR Count = "));
      Serial.println(rtcISRCount);

      Serial.print(F("EIC INTFLAG = 0x"));
      Serial.println(EIC->INTFLAG.reg, HEX);

      Serial.println(F("Going to sleep"));

    }


    LowPower.sleep(); //Change to deep sleep for field deployment

    if (DEBUG_MODE){
      Serial.println(F("Woke up"));
    }
  }

  Watchdog.enable(WATCHDOG_TIMEOUT_MS);
  Watchdog.reset();

}


// ---------- INTERRUPT WAKEUP FUNCTIONS ----------

void isr_generic(){
  flag_event_change = true;
}

void isr_rtc(){
  uint32_t now = millis();

  rtcISRGapMs = now - lastRTCIsrMillis;
  lastRTCIsrMillis = now;
  rtcISRCount++;

  if (digitalRead(PIN_RTC_INT) == LOW){
    rtcTriggered = true;
  }

}

void isr_bucket(){ 
    bucketCount++;
    flag_event_change = true;

}
