#include <RTCZero.h>
#include <SPI.h>
#include <SD.h>

#define YEAR 24
#define MONTH 7
#define DAY 23
#define HOUR 19
#define MINUTE 34
#define SECOND 0

#define PIN_SNS_BUCKET 6
#define PIN_CHIP_SELECT 4
#define PIN_CHIP_DETECT 7

File dataFile;
RTCZero rtc;

volatile uint8_t bucket_event = 0;
volatile uint8_t bucket_counter = 0;
uint8_t bucket_counter_buffer = 0;

char sprintf_buffer[50];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Hello from feather_simple.ino");

  pinMode(PIN_SNS_BUCKET, INPUT_PULLUP);
  pinMode(PIN_CHIP_DETECT, INPUT_PULLUP);

  if (digitalRead(PIN_CHIP_DETECT))
  {
    if (!SD.begin(PIN_CHIP_SELECT)) {
      Serial.println("[ERROR] SDBEGIN");
      while (1);
    } else {
      Serial.println("[STATUS] Initialized SD");
    }
  } else {
    Serial.println("[ERROR] SDMISSING");
  }


  log_to_sd("time,bucket_count\n");

  
  attachInterrupt(digitalPinToInterrupt(PIN_SNS_BUCKET), isr_bucket, RISING);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (bucket_counter) {
    noInterrupts();
    bucket_counter_buffer = bucket_counter;
    bucket_counter = 0;
    interrupts();
  
    sprintf(sprintf_buffer, "[EVENT] %d\n", bucket_counter_buffer);
    Serial.print(sprintf_buffer);

    sprintf(sprintf_buffer, "%d,%d\n",rtc.getEpoch(), bucket_counter_buffer);
    log_to_sd(sprintf_buffer);
  } 

  Serial.println("[STATUS] Arduino alive");

  delay(2000);

}

void isr_bucket(){
  bucket_counter ++;
}


void log_to_sd(char str[]){

  if (digitalRead(PIN_CHIP_DETECT)){
    dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(str);

      digitalWrite(LED_BUILTIN,HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN,LOW);

      dataFile.close();
    } else {
      Serial.println("[ERROR] SDFAIL");
    }
  } else {
    Serial.println("[ERROR] SDMISSING");
  }

}
