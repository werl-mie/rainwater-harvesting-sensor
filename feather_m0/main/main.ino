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
#define PIN_RTC_INT 5
#define PIN_CHIP_SELECT 10

#define PIN_SNS_BUCKET 6
#define PIN_SNS_FLOAT0 9
#define PIN_SNS_FLOAT1 11
#define PIN_SNS_FLOAT2 12

#define PIN_POT A0

// Parameters
#define COUNTER_TIMEOUT_BUCKET 5
#define MEAUSREMENT_PERIOD_S 10

const byte seconds = 50;
const byte minutes = 59;
const byte hours = 16;

const byte day = 15;
const byte month = 6;
const byte year = 15;

// Variables
volatile int flag_measurement_timer = 0;
volatile int flag_counter_timeout_bucket = 0;
volatile int flag_first_count_bucket = 0;
volatile int flag_float0_change, flag_float1_change, flag_float2_change = 0;

volatile int counter_bucket = 0;
volatile int val_float0, val_float1, val_float2 = 0;
sensors_event_t val_rh, val_temp;

String ts_first_bucket_count = "";

File dataFile;
DateTime now;
char sprintf_buffer[19]; //timestamp string is exactly 19 characters long
String buffer_str = "";

byte seconds_new = 0;
byte minutes_new = 0;

// Debug
volatile int SD_failure = 0;
volatile int aht_failure = 0;

void setup() {
  // Serial.begin(115200);
  // while (!Serial);

  if (!aht.begin()) {
    // Serial.println("Could not find AHT? Check wiring");
    while (1);
  }

  if (!SD.begin(chipSelect)) {
    // Serial.println("Card failed, or not present");
    while (1);
  }

  if (! rtc_pcf.begin()) {
    // Serial.println("Couldn't find RTC");
    // Serial.flush();
    while (1);
  }

  rtc_pcf.adjust(DateTime(2023, 7, 20, 16, 47, 0));
  rtc_pcf.deconfigureAllTimers();
  rtc_pcf.enableCountdownTimer(PCF8523_FrequencySecond, MEAUSREMENT_PERIOD_S); 

  rtc_samd.begin();
  rtc_samd.setDate(day, month, year);
  rtc_samd.setTime(hours, minutes, seconds);
  rtc_samd.attachInterrupt(isr_bucket_monitoring_timeout);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(PIN_RTC_INT, INPUT_PULLUP);
  pinMode(PIN_SNS_BUCKET, INPUT_PULLUP);
  pinMode(PIN_SNS_FLOAT0, INPUT_PULLUP);
  pinMode(PIN_SNS_FLOAT1, INPUT_PULLUP);
  pinMode(PIN_SNS_FLOAT2, INPUT_PULLUP);

  LowPower.attachInterruptWakeup(PIN_RTC_INT, isr_rtc_alarm, FALLING);  
  LowPower.attachInterruptWakeup(PIN_SNS_BUCKET, isr_bucket, RISING);
  LowPower.attachInterruptWakeup(PIN_SNS_FLOAT0, isr_float0, CHANGE);
  LowPower.attachInterruptWakeup(PIN_SNS_FLOAT1, isr_float1, CHANGE);
  LowPower.attachInterruptWakeup(PIN_SNS_FLOAT2, isr_float2, CHANGE);
  
  digitalWrite(LED_BUILTIN,LOW);
}

void loop() {
  
  while (flag_float0_change || flag_float1_change || flag_float2_change || flag_measurement_timer || flag_counter_timeout_bucket || flag_first_count_bucket){
    if (flag_float0_change) {
      noInterrupts();
      log_to_sd(get_timestamp_str() + ",Float0LevelChange," + String(val_float0) + "\n");
      flag_float0_change = 0;
      interrupts();
    }

    if (flag_float1_change) {
      noInterrupts();
      log_to_sd(get_timestamp_str() + ",Float1LevelChange," + String(val_float1) + "\n");
      flag_float1_change = 0;
      interrupts();
    }

    if (flag_float2_change) {
      noInterrupts();
      log_to_sd(get_timestamp_str() + ",Float2LevelChange," + String(val_float2) + "\n");
      flag_float2_change = 0;
      interrupts();
    }

    if (flag_measurement_timer){
      noInterrupts();

      if (!aht.getEvent(&val_rh, &val_temp)){
        aht_failure = 1;
      }

      log_to_sd(get_timestamp_str() + ",TempC," + String(val_temp.temperature) + ",RH," + String(val_rh.relative_humidity) + ",TankLevelRaw," + String(analogRead(PIN_POT)) + ",Float0Level," + String(digitalRead(PIN_SNS_FLOAT0)) + ",Float1Level," + String(digitalRead(PIN_SNS_FLOAT1)) + ",Float2Level," + String(digitalRead(PIN_SNS_FLOAT2)) + "\n");

      // flag_measurement_timer = 0;
      interrupts();

      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(1);

      if (aht_failure){
        while(1){
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
          delay(250);
        }
      }
    }

    if (flag_first_count_bucket){
      noInterrupts();
      ts_first_bucket_count = get_timestamp_str();
      flag_first_count_bucket = 0;
      interrupts();
    }

    if (flag_counter_timeout_bucket){
      noInterrupts();
      log_to_sd(get_timestamp_str() + ",BucketFirstCountTime," + ts_first_bucket_count + ",BucketCounts," + String(counter_bucket) + "\n");
      counter_bucket = 0;
      flag_counter_timeout_bucket = 0;
      interrupts();
    }

    if (SD_failure) {
       while(1){
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(1000);
      }
    }
    
  }

  // ---- Periodic readings ---- //
  // sensorValue = analogRead(analogInPin);
  // outputValue = map(sensorValue, 0, 1023, 0, 270);

  // Humidity sensor

  // Timestamp
  

  LowPower.sleep();
}

void isr_float0() {
  flag_float0_change = 1;
  val_float0 = digitalRead(PIN_SNS_FLOAT0);
}

void isr_float1() {
  flag_float1_change = 1;
  val_float1 = digitalRead(PIN_SNS_FLOAT1);
}

void isr_float2() {
  flag_float2_change = 1;
  val_float2 = digitalRead(PIN_SNS_FLOAT2);
}


void isr_bucket_monitoring_timeout() {
  rtc_samd.disableAlarm();
  flag_counter_timeout_bucket = 1;
}

void isr_rtc_alarm(){
  flag_measurement_timer = 1;
}


void isr_bucket(){

  if (counter_bucket == 0){
    // Serial.println("First bucket");
    flag_first_count_bucket = 1;
  }

  counter_bucket ++;
  
  setIncrementalAlarm(COUNTER_TIMEOUT_BUCKET);
}

void setIncrementalAlarm(byte increment_s){
  // Not sure this is necessary...
  rtc_samd.disableAlarm();

  seconds_new = rtc_samd.getSeconds() + increment_s - 1;
  minutes_new = rtc_samd.getMinutes();

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
  now = rtc_pcf.now();
  sprintf(sprintf_buffer, "%d-%.2d-%.2d %.2d:%.2d:%.2d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  buffer_str = sprintf_buffer;
  return buffer_str;
  // return "TS";
}

void log_to_sd(String str){

    dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(str);
      dataFile.close();
      // print to the serial port too:
      Serial.print(str);
    }
    // if the file isn't open, pop up an error:
    else {
      // Serial.println("error opening datalog.txt");
      SD_failure = 1;
    }
}

