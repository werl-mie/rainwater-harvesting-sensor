//Import libraries
#include <RTCZero.h>
#include "RTClib.h"

//Define pins
const int rainBucket = 12;
const int chipSelect = 4; //pin for built-in SD card on M0

RTCZero rtc_samd; //built-in Real-Time Clock for time tracking

//Define global variables
volatile int bucketCounter = 0; //will be used to count the number of bucket tips
unsigned long epochFirstBucket; //Track time of first rain event
unsigned long epochNow; //Track current time

void setup() {
  Serial.begin(115200);
  rtc_samd.begin(); //Initialize RTC

  //Pin mode assignment
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(rainBucket, INPUT_PULLUP); 

  digitalWrite(LED_BUILTIN, LOW); //turn off built in LED

}

void loop() {
    if(digitalRead(rainBucket) == LOW){
      digitalWrite(LED_BUILTIN, HIGH);
      epochFirstBucket = rtc_samd.getEpoch();
      Serial.println("Time first rain:");
      Serial.println(epochFirstBucket);
      bucketCounter++; 
      Serial.println("Bucket count:");
      Serial.println(bucketCounter);

    delay(750); //Debouncing delay
    digitalWrite(LED_BUILTIN, LOW);
    }

}
