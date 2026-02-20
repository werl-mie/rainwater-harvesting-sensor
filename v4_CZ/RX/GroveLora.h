#ifndef GROVELORA_H
#define GROVELORA_H
#include <Arduino.h>

typedef struct struct_datum {
  uint32_t ts;
  uint16_t site_id;
  uint8_t node_id;
  uint8_t sensor_type;
  uint16_t data;
} SensorDatum_t;

class GroveLora {
  public:
    GroveLora();
    void sendCmd(const char* cmd);
    bool checkRX(SensorDatum_t* p_datum);
};
#endif