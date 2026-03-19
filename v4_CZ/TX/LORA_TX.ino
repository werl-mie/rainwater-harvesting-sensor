#include "ArduinoLowPower.h"
#include <RTCZero.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_SleepyDog.h>
#include "GroveLora.h"

#define DEBUG_MODE true

// Pin assignments
#define PIN_SD_CS          4
#define PIN_SNS_BUCKET     12      
#define PIN_SNS_LEAFSWITCH 5  
#define PIN_SNS_FLOATBOT   6    
#define PIN_SNS_FLOATTOP   9    

GroveLora dev;
RTCZero rtc;

// Global State
volatile bool flag_event_change = false;
volatile int bucketCount = 0;
unsigned long lastBucketTime = 0;
int lastLeaf, lastTop, lastBot;

void setup() {
  if (DEBUG_MODE) {
    Serial.begin(115200);
    uint32_t startWait = millis();
    while (!Serial && (millis() - startWait < 3000));
    Serial.println("--- TX READY (SD + LORA) ---");
  }

  Serial1.begin(9600);
  Watchdog.enable(16000);

  // Initialize SD Card
  if (!SD.begin(PIN_SD_CS)) {
    if (DEBUG_MODE) Serial.println("SD Fail");
    while (1);
  }

  if (!SD.exists("datalog.csv")) {
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println("DateTime,Sensor,Epoch,Value");
      dataFile.close();
    }
  }

  // Initialize LoRa Module
  dev.sendCmd("AT+MODE=TEST\r\n"); 
  delay(200);
  dev.sendCmd("AT+TEST=RFCFG,915,SF7,125,12,15,14,ON,OFF,OFF\r\n"); 
  delay(200);

  // Initialize RTC
  rtc.begin();
  rtc.setTime(19, 58, 00);
  rtc.setDate(2, 2, 26);

  // Hardware Setup
  pinMode(PIN_SNS_BUCKET, INPUT_PULLUP);
  pinMode(PIN_SNS_LEAFSWITCH, INPUT_PULLUP);
  pinMode(PIN_SNS_FLOATTOP, INPUT_PULLUP);
  pinMode(PIN_SNS_FLOATBOT, INPUT_PULLUP);

  lastLeaf = digitalRead(PIN_SNS_LEAFSWITCH);
  lastTop  = digitalRead(PIN_SNS_FLOATTOP);
  lastBot  = digitalRead(PIN_SNS_FLOATBOT);

  // Wakeup Interrupts
  LowPower.attachInterruptWakeup(PIN_SNS_BUCKET, isr_bucket, FALLING);
  LowPower.attachInterruptWakeup(PIN_SNS_LEAFSWITCH, isr_generic, CHANGE);
  LowPower.attachInterruptWakeup(PIN_SNS_FLOATTOP, isr_generic, CHANGE);
  LowPower.attachInterruptWakeup(PIN_SNS_FLOATBOT, isr_generic, CHANGE);
}

void loop() {
  // 10-Minute Timeout Reset for Bucket
  if (bucketCount > 0 && (millis() - lastBucketTime > 600000)) {
    bucketCount = 0;
    if (DEBUG_MODE) Serial.println(F("BUCKET RESET"));
  }

  // Process Sensor Events
  if (flag_event_change) {
    noInterrupts();
    flag_event_change = false;
    interrupts();

    identifyAndLog();
  }

  Watchdog.reset();

  // Power Management
  if (!DEBUG_MODE) {
    LowPower.sleep();
  } else {
    delay(200);
  }
}

// --- Interrupt Service Routines ---

void isr_bucket() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  
  // Software Debounce
  if (interrupt_time - last_interrupt_time > 200) {
    bucketCount++;
    lastBucketTime = interrupt_time;
    flag_event_change = true;
  }
  last_interrupt_time = interrupt_time;
}

void isr_generic() {
  flag_event_change = true;
}

// --- Logic Functions ---

void identifyAndLog() {
  // 1. Bucket Check
  static int lastLoggedBucket = 0;
  if (bucketCount != lastLoggedBucket) {
    int currentCount = bucketCount; 
    writeToCSV("BUCKET," + String(currentCount));
    transmit(1, currentCount);
    lastLoggedBucket = currentCount;
  }

  // 2. Leaf Check
  int currentLeaf = digitalRead(PIN_SNS_LEAFSWITCH);
  if (currentLeaf != lastLeaf) {
    lastLeaf = currentLeaf;
    writeToCSV("LEAF," + String(currentLeaf == LOW ? "PRESSED" : "LIFTED"));
    transmit(2, currentLeaf);
  }

  // 3. Float Top Check
  int currentTop = digitalRead(PIN_SNS_FLOATTOP);
  if (currentTop != lastTop) {
    lastTop = currentTop;
    writeToCSV("FLOAT_TOP," + String(currentTop == LOW ? "FAR" : "CLOSE"));
    transmit(3, currentTop);
  }

  // 4. Float Bot Check
  int currentBot = digitalRead(PIN_SNS_FLOATBOT);
  if (currentBot != lastBot) {
    lastBot = currentBot;
    writeToCSV("FLOAT_BOT," + String(currentBot == LOW ? "CLOSE" : "FAR"));
    transmit(4, currentBot);
  }
}

void writeToCSV(String msg) {
  uint32_t epoch = rtc.getEpoch();
  char ts_human[25];
  sprintf(ts_human, "%04d-%02d-%02d %02d:%02d:%02d",
          rtc.getYear() + 2000, rtc.getMonth(), rtc.getDay(),
          rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());

  int commaIndex = msg.indexOf(',');
  String sensorName = msg.substring(0, commaIndex);
  String sensorVal = msg.substring(commaIndex + 1);

  String logLine = String(ts_human) + "," + sensorName + "," + String(epoch) + "," + sensorVal;

  if (DEBUG_MODE) Serial.println("SD LOG: " + logLine);

  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println(logLine);
    dataFile.close();
    
    // Status Blink
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void transmit(uint8_t typeID, uint16_t value) {
  char cmd[128];
  // Payload Format: Header(14 chars) + TypeID(2) + Value(4)
  sprintf(cmd, "AT+TEST=TXLRPKT,\"00000000111122%02x%04x\"\r\n", typeID, value);
  dev.sendCmd(cmd);

  if (DEBUG_MODE) {
    Serial.print(F("LORA: Type=")); Serial.print(typeID);
    Serial.print(F(" Val=")); Serial.println(value);
  }
  
  delay(200); // Radio cooldown before potential sleep
}
