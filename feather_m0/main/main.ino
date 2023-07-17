
#include "ArduinoLowPower.h"
#include <Adafruit_AHTX0.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

RTC_PCF8523 rtc;
Adafruit_AHTX0 aht;



// Pin assignments
#define PIN_CHIP_SELECT 10
#define PIN_FLOAT1 6
#define PIN_RTC_INT 5

#define PIN_POT A0

// Variables
volatile int debug_isr_entries_1 = 0;
volatile int flag_event_alarm = 0;
volatile int event_counter_float1 = 0;
volatile int dv_float1 = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // if (! aht.begin()) {
  //   Serial.println("Could not find AHT? Check wiring");
  //   while (1);
  // }

  // Serial.println("AHT10 or AHT20 found");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  rtc.deconfigureAllTimers();

  rtc.enableCountdownTimer(PCF8523_FrequencySecond, 60);  // 30 seconds




  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_FLOAT1, INPUT_PULLUP);
  pinMode(PIN_RTC_INT, INPUT_PULLUP);

  // Attach a wakeup interrupt on pin 8, calling repetitionsIncrease when the device is woken up
  LowPower.attachInterruptWakeup(PIN_FLOAT1, isr_float, CHANGE);
  LowPower.attachInterruptWakeup(PIN_RTC_INT, isr_rtc_alarm, FALLING);

  attachInterrupt(digitalPinToInterrupt(PIN_FLOAT1), isr_float, CHANGE);


  digitalWrite(LED_BUILTIN,LOW);
}

void loop() {
  
  while (event_counter_float1 || flag_event_alarm){
    if (event_counter_float1) {
      noInterrupts();
      log_to_sd("FloatLevelChange", dv_float1);
      event_counter_float1 --;
      interrupts();
    }

    if (flag_event_alarm){
      flag_event_alarm --;
    }
    
  }

  // ---- Periodic readings ---- //
  // sensorValue = analogRead(analogInPin);
  // outputValue = map(sensorValue, 0, 1023, 0, 270);

  // Humidity sensor

  // Timestamp


  

  LowPower.sleep();
}

void log_to_sd(String event_type, int data){
    String timestamp_str = get_timestamp_str();
    String dataString = timestamp_str + "," + event_type + "," + String(data);


    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
}

void isr_rtc_alarm(){
  flag_event_alarm += 1;
}

void isr_float() {
  event_counter_float1 += 1;
  dv_float1 = digitalRead(PIN_FLOAT1);
  
}

String get_timestamp_str(){
  DateTime now = rtc.now();

  char buffer[40];
  sprintf(buffer, "%d-%d-%d %d:%d:%d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  String buffer_str = buffer;
  return buffer_str;
}

