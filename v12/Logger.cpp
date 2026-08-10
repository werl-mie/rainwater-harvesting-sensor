#include "Logger.h"

// ------------ Initializes SD logger ----------------
bool Logger::begin(){
    
  if (!SD.begin(PIN_SD_CS)){
    
    if (DEBUG_MODE){
      Serial.println(F("ERROR: SD failed")); 
    }
    return false;
  }

  if(!createFile(SENSOR_LOG_FILE, FILE_HEADER)){
    return false;
  }

  if (DEBUG_MODE){
    Serial.println(F("Logger init"));
  }

  return true;
}

// ------------ Create CSV File ----------------
bool Logger::createFile(const char *filename, const char *header){
  if (SD.exists(filename)){
    return true;
  }

  File file = SD.open(filename, FILE_WRITE);

  if (!file){
    if (DEBUG_MODE){
      Serial.print(F("ERROR: Cannot create file"));
      Serial.println(filename);
    }
    return false;
  }

  file.println(header);
  file.flush();
  file.close();

  if (DEBUG_MODE){
    Serial.println(F("File created"));
  }
  return true;
}

// ------------ Adding new entry lines to CSV --------------------

bool Logger::appendLine(const char *filename, const char *line){
  File file = SD.open(filename, FILE_WRITE); 

  if (!file){
    if (DEBUG_MODE){
      Serial.print(F("ERROR: Cannot open ")); 
      Serial.println(filename);
    }
    return false;
  }

  size_t bytesWritten = file.println(line);

  file.flush();
  file.close();

  return (bytesWritten > 0);

  
}

// ------------ Write to SD --------------------
bool Logger::writeToSD(const SensorDataBatch &data){
  
  char line[CSV_LINE_BUFFER_SIZE];

  int written = snprintf(line, sizeof(line),
  "%lu,%lu,%u,%d,%d,%d,%d,%u,%.2f,%u,%.2f", 
  (unsigned long)data.recordID,
  (unsigned long)data.epoch,
  data.bucket, 
  data.leaf ? 1 : 0, 
  data.ftop ? 1 : 0, 
  data.fbot ? 1 : 0, 
  data.chlo ? 1 : 0, 
  data.pressure_adc,
  data.pressure_v,
  data.turbidity_adc,
  data.turbidity_v
  );

  // Check for succesful write
  if (written < 0 || written >= (int)sizeof(line)){
    if (DEBUG_MODE){
      Serial.println(F("ERROR: CSV line buffer overflow!"));
    }
    return false;
  }

  //Append formatted row to SD
  if (DEBUG_MODE){
    Serial.println(F("SD line: "));
    Serial.println(line);
  }
  
  return appendLine(SENSOR_LOG_FILE, line);
}



// ----------- Convert SensorID to string -----------------
const char* Logger::sensorToString(SensorID sensor){
  switch(sensor){

    case SensorID::Bucket:
      return "Bkt";

    case SensorID::Leaf:
      return "Lf";

    case SensorID::FloatTop:
      return "FlT";

    case SensorID::FloatBottom:
      return "FlB";

    case SensorID::Chlo:
      return "Chl";

    case SensorID::Turbidity_adc:
      return "TS_adc";

    case SensorID::Turbidity_v:
      return "TS_v";      

    case SensorID::Pressure_adc:
      return "Prs_adc"; 

    case SensorID::Pressure_v:
      return "Prs_v";  

  //  case SensorID::BatteryV:
  //    return "vBat";

    default: 
      return "Unknown";

  }
}

