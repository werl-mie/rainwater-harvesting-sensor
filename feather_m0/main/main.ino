#include "ArduinoLowPower.h"
#include <Adafruit_AHTX0.h>
#include <RTCZero.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

RTCZero rtc_samd;
RTC_PCF8523 rtc_pcf;
Adafruit_AHTX0 aht;


// Pin assignments
#define PIN_CHIP_SELECT 10
#define PIN_SNS_FLOAT0 6
#define PIN_RTC_INT 5
#define PIN_SNS_BUCKET 9

#define PIN_POT A0

// Constants
#define COUNTER_TIMEOUT 3

const byte seconds = 50;
const byte minutes = 59;
const byte hours = 16;

const byte day = 15;
const byte month = 6;
const byte year = 15;

// Variables
volatile int event_flag_measurement_timer = 0;
volatile int event_flag_bucket_monitoring_timeout = 0;
volatile int event_flag_first_bucket_count = 0;
volatile int event_flag_float1 = 0;

volatile int counter_bucket = 0;
volatile int val_float1 = 0;

String ts_first_bucket_count = "";

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

  if (! rtc_pcf.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  rtc_pcf.adjust(DateTime(2023, 7, 19, 12, 18, 0));
  rtc_pcf.deconfigureAllTimers();
  rtc_pcf.enableCountdownTimer(PCF8523_FrequencySecond, 60); 

  rtc_samd.begin();
  rtc_samd.setDate(day, month, year);
  rtc_samd.setTime(hours, minutes, seconds);
  rtc_samd.attachInterrupt(isr_bucket_monitoring_timeout);



  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_SNS_FLOAT0, INPUT_PULLUP);
  pinMode(PIN_SNS_BUCKET, INPUT_PULLUP);
  pinMode(PIN_RTC_INT, INPUT_PULLUP);

  LowPower.attachInterruptWakeup(PIN_SNS_FLOAT0, isr_float1, CHANGE);
  LowPower.attachInterruptWakeup(PIN_SNS_BUCKET, isr_bucket, RISING);
  LowPower.attachInterruptWakeup(PIN_RTC_INT, isr_rtc_alarm, FALLING);


  digitalWrite(LED_BUILTIN,LOW);
}

void loop() {
  
  while (event_flag_float1 || event_flag_measurement_timer || event_flag_bucket_monitoring_timeout || event_flag_first_bucket_count){
    if (event_flag_float1) {
      noInterrupts();
      log_to_sd("FloatLevelChange", String(val_float1));
      event_flag_float1 = 0;
      interrupts();
    }

    if (event_flag_measurement_timer){
      // Do measurements

      event_flag_measurement_timer = 0;
    }

    if (event_flag_first_bucket_count){
      ts_first_bucket_count = get_timestamp_str();
      event_flag_first_bucket_count = 0;
    }

    if (event_flag_bucket_monitoring_timeout){
      // Log to SD
      log_to_sd("BucketFirstCountTime", ts_first_bucket_count);
      log_to_sd("BucketCounts", String(counter_bucket));

      counter_bucket = 0;
      event_flag_bucket_monitoring_timeout = 0;
    }
    
  }

  // ---- Periodic readings ---- //
  // sensorValue = analogRead(analogInPin);
  // outputValue = map(sensorValue, 0, 1023, 0, 270);

  // Humidity sensor

  // Timestamp
  

  LowPower.sleep();
}

void log_to_sd(String event_type, String data){
    String timestamp_str = get_timestamp_str();
    String dataString = timestamp_str + "," + event_type + "," + data;


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

void isr_bucket_monitoring_timeout() {
  rtc_samd.disableAlarm();
  event_flag_bucket_monitoring_timeout = 1;
}

void isr_rtc_alarm(){
  event_flag_measurement_timer ++;
}

void isr_float1() {
  event_flag_float1 ++;
  val_float1 = digitalRead(PIN_SNS_FLOAT0);
}

void isr_bucket(){

  if (counter_bucket == 0){
    Serial.println("First bucket");
    event_flag_first_bucket_count = 1;
  }

  counter_bucket ++;
  
  setIncrementalAlarm(5);
}

void setIncrementalAlarm(byte increment_s)
{
  // Not sure this is necessary...
  rtc_samd.disableAlarm();

  byte seconds_new = rtc_samd.getSeconds() + increment_s - 1;
  byte minutes_new = rtc_samd.getMinutes();

  if (seconds_new > 59){
    seconds_new = seconds_new - 60;
    minutes_new = minutes_new + 1;
  }

  if (minutes_new > 59){
    minutes_new = 0;
  }

  rtc_samd.setAlarmTime(rtc_samd.getHours(), minutes_new, seconds_new);
  rtc_samd.enableAlarm(rtc_samd.MATCH_MMSS);
}

String get_timestamp_str(){
  DateTime now = rtc_pcf.now();

  char buffer[40];
  sprintf(buffer, "%d-%.2d-%.2d %d:%.2d:%.2d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  String buffer_str = buffer;
  return buffer_str;
}

