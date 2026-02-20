#include "ArduinoLowPower.h"
#include <SPI.h>
#include <SD.h>
#include <Adafruit_SleepyDog.h>
#include "GroveLora.h"

#define PIN_SNS_BUCKET 12      
#define PIN_SNS_LEAFSWITCH 6  
#define PIN_SNS_FLOATTOP 11    
#define PIN_SNS_FLOATBOT 9    

GroveLora dev;
volatile bool flag_event_change = false;
volatile int bucketCount = 0;
volatile unsigned long lastBucketTime = 0; // Track last tip time
int lastLeaf, lastTop, lastBot;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // --- 10S SAFETY FLASHING WINDOW ---
  for (int i = 0; i < 40; i++) {
    digitalWrite(LED_BUILTIN, HIGH); delay(125);
    digitalWrite(LED_BUILTIN, LOW); delay(125);
  }

  Serial.begin(115200);
  Serial1.begin(9600); 
  
  dev.sendCmd("AT+MODE=TEST\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RFCFG,915,SF7,125,12,15,14,ON,OFF,OFF\r\n"); delay(200);

  pinMode(PIN_SNS_BUCKET, INPUT_PULLUP);
  pinMode(PIN_SNS_LEAFSWITCH, INPUT_PULLUP);
  pinMode(PIN_SNS_FLOATTOP, INPUT_PULLUP);
  pinMode(PIN_SNS_FLOATBOT, INPUT_PULLUP);

  lastLeaf = digitalRead(PIN_SNS_LEAFSWITCH);
  lastTop  = digitalRead(PIN_SNS_FLOATTOP);
  lastBot  = digitalRead(PIN_SNS_FLOATBOT);

  LowPower.attachInterruptWakeup(PIN_SNS_BUCKET, isr_bucket, FALLING);
  LowPower.attachInterruptWakeup(PIN_SNS_LEAFSWITCH, isr_generic, CHANGE);
  LowPower.attachInterruptWakeup(PIN_SNS_FLOATTOP, isr_generic, CHANGE);
  LowPower.attachInterruptWakeup(PIN_SNS_FLOATBOT, isr_generic, CHANGE);
  
  Serial.println("--- TX READY (EVENT-ONLY MODE) ---");
}

void loop() {
  // --- 10 MINUTE BUCKET RESET LOGIC ---
  // 600,000 ms = 10 minutes
  if (bucketCount > 0 && (millis() - lastBucketTime > 600000)) {
    bucketCount = 0;
    Serial.println(F(">>> 10 MIN INACTIVITY: BUCKET COUNT RESET TO 0"));
  }

  if (flag_event_change) {
    Serial.begin(115200);
    unsigned long startWait = millis();
    while(!Serial && (millis() - startWait < 500)); 

    digitalWrite(LED_BUILTIN, HIGH);
    processSensors(); 
    
    Serial.println(F(">>> DATA SENT. SLEEPING..."));
    Serial.flush();
    
    delay(2000); 
    digitalWrite(LED_BUILTIN, LOW);
    flag_event_change = false;
  }
  
  Watchdog.reset();
  LowPower.sleep(); 
}

void isr_bucket() {
  static unsigned long last_tip_debounce = 0;
  if (millis() - last_tip_debounce > 200) { 
    bucketCount++; 
    lastBucketTime = millis(); // Update the "last active" timestamp
    flag_event_change = true; 
  }
  last_tip_debounce = millis();
}

void isr_generic() { flag_event_change = true; }

void processSensors() {
  static int lastSentBucket = 0;
  if (bucketCount != lastSentBucket) { transmit(1, bucketCount); lastSentBucket = bucketCount; }

  int cLeaf = digitalRead(PIN_SNS_LEAFSWITCH);
  if (cLeaf != lastLeaf) { transmit(2, cLeaf); lastLeaf = cLeaf; }

  int cTop = digitalRead(PIN_SNS_FLOATTOP);
  if (cTop != lastTop) { transmit(3, cTop); lastTop = cTop; }

  int cBot = digitalRead(PIN_SNS_FLOATBOT);
  if (cBot != lastBot) { transmit(4, cBot); lastBot = cBot; }
}

void transmit(uint8_t typeID, uint16_t value) {
  Serial.print(F("EVENT: Type=")); Serial.print(typeID);
  Serial.print(F(" Val=")); Serial.println(value);

  char cmd[128];
  sprintf(cmd, "AT+TEST=TXLRPKT,\"00000000000403%02X%04X\"\r\n", typeID, value);
  dev.sendCmd(cmd);
  delay(1000); 
}
