#include "ArduinoLowPower.h"
#include <RTCZero.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_SleepyDog.h>


#define DEBUG_MODE true


const int chipSelect = 4;
RTCZero rtc;


// Pin assignments
#define PIN_SNS_BUCKET 12      
#define PIN_SNS_LEAFSWITCH 6  
#define PIN_SNS_FLOATTOP 11    
#define PIN_SNS_FLOATBOT 9    


volatile bool flag_event_change = false;
volatile int bucketCount = 0;


int lastLeaf, lastTop, lastBot;


void setup() {
  if (DEBUG_MODE) {
    Serial.begin(115200);
    uint32_t startWait = millis();
    while (!Serial && (millis() - startWait < 3000));
    Serial.println("--- START ---");
  }


  Watchdog.enable(16000);


  if (!SD.begin(chipSelect)) {
    while (1);
  }


  // --- CSV HEADER LOGIC ---
  // If the file doesn't exist, create it and add headers
  if (!SD.exists("datalog.csv")) {
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println("DateTime,Sensor,Epoch,Value");
      dataFile.close();
    }
  }


  rtc.begin();
  rtc.setTime(19, 58, 00);
  rtc.setDate(2, 2, 26);


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
}


void loop() {
  if (flag_event_change) {
    noInterrupts();
    identifyAndLog();
    flag_event_change = false;
    interrupts();
  }


  Watchdog.reset();


  if (!DEBUG_MODE) {
    LowPower.sleep();
  } else {
    delay(200);
  }
}


void isr_bucket() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    bucketCount++;
    flag_event_change = true;
  }
  last_interrupt_time = interrupt_time;
}


void isr_generic() {
  flag_event_change = true;
}


void identifyAndLog() {
  String eventMsg = "";
 
  static int lastLoggedBucket = 0;
  if (bucketCount > lastLoggedBucket) {
    eventMsg = "BUCKET," + String(bucketCount);
    lastLoggedBucket = bucketCount;
  }
 
  int currentLeaf = digitalRead(PIN_SNS_LEAFSWITCH);
  if (currentLeaf != lastLeaf) {
    eventMsg = "LEAF," + String(currentLeaf == LOW ? "CLOSED" : "OPEN");
    lastLeaf = currentLeaf;
  }


  int currentTop = digitalRead(PIN_SNS_FLOATTOP);
  if (currentTop != lastTop) {
    eventMsg = "FLOAT_TOP," + String(currentTop == LOW ? "HIGH" : "OK");
    lastTop = currentTop;
  }


  int currentBot = digitalRead(PIN_SNS_FLOATBOT);
  if (currentBot != lastBot) {
    eventMsg = "FLOAT_BOT," + String(currentBot == LOW ? "LOW" : "OK");
    lastBot = currentBot;
  }


  if (eventMsg != "") {
    writeToCSV(eventMsg);
  }
}


void writeToCSV(String msg) {
  uint32_t epoch = rtc.getEpoch();
  char ts_human[25];
  sprintf(ts_human, "%04d-%02d-%02d %02d:%02d:%02d",
          rtc.getYear()+2000, rtc.getMonth(), rtc.getDay(),
          rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());


  int commaIndex = msg.indexOf(',');
  String sensorName = msg.substring(0, commaIndex);
  String sensorVal = msg.substring(commaIndex + 1);


  // Formats row: 2026-02-02 19:58:00,SENSOR,1738526280,VALUE
  String logLine = String(ts_human) + "," + sensorName + "," + String(epoch) + "," + sensorVal;


  if (DEBUG_MODE) Serial.println(logLine);


  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println(logLine);
    dataFile.close();
   
    digitalWrite(LED_BUILTIN, HIGH);
    delay(20);
    digitalWrite(LED_BUILTIN, LOW);
  }
}


