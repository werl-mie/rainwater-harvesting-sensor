#include "NotecardManager.h"

// ------------ Initialize Notecard Manager ----------------


bool NotecardManager::begin(){
  notecard.begin();

  if (DEBUG_MODE){
    notecard.setDebugOutputStream(Serial);
  }

  // Configure Notehub
  if (!configureHub()){
    return false;
  }

  // Temporarily reset template, comment out after
  //J *clearReq = notecard.newRequest("note.template");
  //if (clearReq != nullptr){
    //JAddStringToObject(clearReq, "file", NOTECARD_SENSOR_FILE);
    //JAddItemToObject(clearReq, "body", JCreateObject());
    //notecard.sendRequest(clearReq);
    //delay(1500);
    
    //if (DEBUG_MODE){
      //Serial.println(F("Cache cleared"));
   // }
  //}

  // Configure sensor template
  if (!configureSensorTemplate()){
    return false;
  }

  if (DEBUG_MODE){
    Serial.println(F("Notecard manager init"));
  }

  return true;
}

// ------------ Configure notehub ----------------

bool NotecardManager::configureHub(){

  J *req = notecard.newRequest("hub.set");

  if (req == nullptr){
    return false;
  }

  // Establish project id 
  JAddStringToObject(req, "product", PRODUCT_UID);

  // Establish upload frequency
  JAddStringToObject(req, "mode", "periodic");

  JAddNumberToObject(req, "outbound", NOTECARD_DEBUG_OUTBOUND_MIN); //Change to desired freq in field, shorter for testing
  JAddNumberToObject(req, "inbound", NOTECARD_DEBUG_INBOUND_MIN); //Change to desired freq in field, shorter for testing

  return notecard.sendRequest(req);

}

// ------------ Configure sensor template ----------------

bool NotecardManager::configureSensorTemplate(){

  J *tmp = notecard.newRequest("note.template");

  if (tmp == nullptr){
    return false;
  }

  JAddStringToObject(tmp, "file", NOTECARD_SENSOR_FILE);

  J *body = JCreateObject();
  if (body == nullptr){
    JDelete(tmp); //Clear memory if body cannot be created
    return false;
  }
  //INDIVIDUAL NOTES: Populate fields matching keys from template

  //JAddNumberToObject(body, "recordID", 14); // 14 = 4-byte uint
  //JAddNumberToObject(body, "epoch", 14);
  //JAddNumberToObject(body, "sensorID", 11); // 11 = 1-byte uint
  //JAddNumberToObject(body, "value", 14.2); //14.2 = Float with 2 decimal points

  //CONSOLIDATED BATCH: 
  JAddNumberToObject(body, "recordID", 14); 
  JAddNumberToObject(body, "epoch", 14); 
  JAddNumberToObject(body, "bucket", 12);

  JAddBoolToObject(body, "leaf", false);
  JAddBoolToObject(body, "ftop", false);
  JAddBoolToObject(body, "fbot", false);
  JAddBoolToObject(body, "chlo", false);

  JAddNumberToObject(body, "turbidity_adc", 12);
  //JAddNumberToObject(body, "turbidity_v", 0);
  JAddNumberToObject(body, "pressure_adc", 12);
  //JAddNumberToObject(body, "pressure_v", 0);
  //JAddNumberToObject(body, "batteryV", 12.2);

  JAddItemToObject(tmp, "body", body);

  return notecard.sendRequest(tmp);;

}

// ------------ SensorData ------------

bool NotecardManager::addSensorData(const SensorDataBatch &data){

  J *req = notecard.newRequest("note.add");
  if (req == nullptr){
    return false;
  }

  JAddStringToObject(req, "file", NOTECARD_SENSOR_FILE);

  //JAddBoolToObject(req, "sync", true);

  J *body = JCreateObject();
  if (body == nullptr){
    JDelete(req);
    return false;
  }

  //INDIVIDUAL NOTES: Populate fields matching keys from template
  //JAddNumberToObject(body, "recordID", data.recordID); 
  //JAddNumberToObject(body, "epoch", data.epoch); 
  //JAddNumberToObject(body, "sensorID", static_cast<uint8_t>(data.sensorID)); //cast enum class for JSON serialization
  //JAddNumberToObject(body, "value", data.value);

  //CONSOLIDATED BATCH: 
  JAddNumberToObject(body, "recordID", data.recordID); 
  JAddNumberToObject(body, "epoch", data.epoch); 
  JAddNumberToObject(body, "bucket", data.bucket);

  JAddBoolToObject(body, "leaf", data.leaf);
  JAddBoolToObject(body, "ftop", data.ftop);
  JAddBoolToObject(body, "fbot", data.fbot);
  JAddBoolToObject(body, "chlo", data.chlo);

  JAddNumberToObject(body, "turbidity_adc", data.turbidity_adc);
  //JAddNumberToObject(body, "turbidity_v", data.turbidity_v);
  JAddNumberToObject(body, "pressure_adc", data.pressure_adc);
  //JAddNumberToObject(body, "pressure_v", data.pressure_v);
  //JAddNumberToObject(body, "batteryV", data.batteryV);


  //Attach body and send to Notehub
  JAddItemToObject(req, "body", body);

  return notecard.sendRequest(req);

}


// ------------ Read battery voltage ------------

float NotecardManager::getBatteryVoltageFromNotecard(){
  
  J *req = notecard.newRequest("card.voltage");

  J *rsp = notecard.requestAndResponse(req);

  if (rsp == nullptr){
    return 0.0; //Communication error
  }

  float voltage = JGetNumber(rsp, "value");

  notecard.deleteResponse(rsp);

  return voltage;
}

// ------------ Determine if notecard is busy ------------

bool NotecardManager::isBusy(){
  J *req = notecard.newRequest("card.location");
  J *rsp = notecard.requestAndResponse(req);

  if (rsp == nullptr){
    return false; //Error communicating
  }

  //Return stop key if notecard is actively looking for towers or processing
  bool busy = JGetBool(rsp, "stop");
  notecard.deleteResponse(rsp);

  return busy;
}
