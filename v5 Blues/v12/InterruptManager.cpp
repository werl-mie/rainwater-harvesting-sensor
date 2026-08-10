#include "InterruptManager.h"


void InterruptManager::processEventAndLog(bool rtcState){
 
 // Capture current time
  uint32_t currentEpoch = rtcManager.getEpoch();

  // Check for bucket timeout
  static uint32_t lastBucketEpoch = 0;
  if (bucketCount > 0 && (currentEpoch - lastBucketEpoch > (BUCKET_RESET_MS/1000))){
    bucketCount = 0;
  }

  // RTC-Driven sensors
  if(rtcState){
    
    if (DEBUG_MODE){
      Serial.println(F("RTC Awake"));

      Serial.print(F("RTC INT at entry ="));
      Serial.println(digitalRead(PIN_RTC_INT));
    }
    //Power 5V rail on 
    powerOn();

    SensorDataBatch data; 
    data.recordID = ++globalRecordID;
    data.epoch = currentEpoch;

    //Analog sensors
    analogRead(PIN_PRESSURE); //throwaway reading
    delay(50);

    data.pressure_adc = analogRead(PIN_PRESSURE);  
    data.pressure_v = (data.pressure_adc*2.23f) / 4095.0;
    data.turbidity_adc = analogRead(PIN_TURBIDITY);
    data.turbidity_v = (data.turbidity_adc*2.23f) / 4095.0;

    //Snapshot of digital sensors
    data.bucket = bucketCount; 
    data.leaf = digitalRead(PIN_LEAF);
    data.ftop = digitalRead(PIN_FLOAT_TOP);
    data.fbot = digitalRead(PIN_FLOAT_BOTTOM);
    data.chlo = digitalRead(PIN_CHLO);
    //data.batteryV = readBatteryVoltage(); //note sure readBatV is a func I have

    //Power 5V rail off
    powerOff(); 
    delay(50);

    // Log to SD
    logger.writeToSD(data);

    //Queue to notecard
    notecardManager.addSensorData(data);  

    rtcManager.resetTimer();

    //rtcTriggered = false;
  
  }else {
    //temporary software debounce, capacitors should handle

    static uint32_t lastDigitalProcessEpoch = 0;
    if (currentEpoch - lastDigitalProcessEpoch < 2){
      if (DEBUG_MODE){
        Serial.println("Digitial int rejected. Too fast!");
      }
      return;
    }

    lastDigitalProcessEpoch = currentEpoch;

    if (DEBUG_MODE){
      Serial.println(F("DigitalINT"));
    }

    delay(20);

    SensorDataBatch data = {}; //Analog remains 0

    data.recordID = ++globalRecordID;
    data.epoch = currentEpoch;

    data.bucket = bucketCount; 
    data.leaf = digitalRead(PIN_LEAF);
    data.ftop = digitalRead(PIN_FLOAT_TOP);
    data.fbot = digitalRead(PIN_FLOAT_BOTTOM);
    data.chlo = digitalRead(PIN_CHLO);  

    //Log to SD
    logger.writeToSD(data);

    // Queue to notecard
    notecardManager.addSensorData(data);

    //Clear interrupt flag
    //flag_event_change = false;

  }
}

// Power handling functions

void InterruptManager::powerOn(){
   pinMode(PIN_MINIBOOST_ENABLE, OUTPUT); 
    
    digitalWrite(PIN_MINIBOOST_ENABLE, HIGH);
    PORT->Group[g_APinDescription[PIN_MINIBOOST_ENABLE].ulPort].PINCFG[g_APinDescription[PIN_MINIBOOST_ENABLE].ulPin].bit.INEN = 1;

    //explicit delay to overcome standby clock compression
    for (int i = 0; i < 30; i++) {
        delay(50); // delayMicrosends = 30 * 50ms = Exactly 1,500ms (1.5 seconds) of warm-up
    }
}

void InterruptManager::powerOff(){
    pinMode(PIN_MINIBOOST_ENABLE, OUTPUT);
    digitalWrite(PIN_MINIBOOST_ENABLE, LOW);
    PORT->Group[g_APinDescription[PIN_MINIBOOST_ENABLE].ulPort].PINCFG[g_APinDescription[PIN_MINIBOOST_ENABLE].ulPin].bit.INEN = 1;
    for (int i = 0; i < 4; i++) {
        delay(50); 
    }
}