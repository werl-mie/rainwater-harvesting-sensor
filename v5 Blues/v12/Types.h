#ifndef TYPES_H
#define TYPES_H

// ------------ Sensor IDs -----------
enum class SensorID : uint8_t
{
    Bucket = 1,
    Leaf = 2,
    FloatTop = 3,
    FloatBottom = 4,
    Chlo = 5,
    Turbidity_adc = 6,
    Turbidity_v = 7,
    Pressure_adc = 8,
    Pressure_v = 9,
    //BatteryV = 10

};

// ------------ Datalogging structures -----------

struct SensorData {

  uint32_t recordID = 0;
  uint32_t epoch = 0;
  SensorID sensorID;
  float value = 0.0f; // to allow for float values of TS + PS sensors & battery voltage

};

struct SensorDataBatch {
  uint32_t recordID = 0;
  uint32_t epoch = 0;
  uint32_t bucket = 0;
  bool leaf = false;
  bool ftop = false;
  bool fbot = false;
  bool chlo = false;
  uint16_t turbidity_adc = 0;
  float turbidity_v = 0.0f;
  uint16_t pressure_adc = 0;
  float pressure_v = 0.0f;
  //float batteryV = 0.0f;
};

#endif
