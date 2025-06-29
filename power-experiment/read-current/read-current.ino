#include "LTC2941.h"

// #ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
//   #define Serial SerialUSB
// #else
//   #define Serial Serial
// #endif

#define COTTON_CANDY_ID "0x00"

float coulomb = 0, mAh = 0;

void setup(void)
{
    Wire.begin();
    
    Serial.begin(115200);
    // while(!Serial.available());
    
    // Serial.println("LTC2941 Raw Data: id, current_cumulative_C, current_cumulative_mAh");
    
    ltc2941.initialize();
    ltc2941.setPrescaler(PRESCALAR_M_1);
}

void loop(void)
{
    coulomb = ltc2941.getCoulombsExpend();
    mAh = ltc2941.getmAhExpend();
    
    Serial.print("cc");
    Serial.print(",");
    Serial.print(coulomb);
    Serial.print(",");
    Serial.print(mAh);
    
    Serial.println();
    
    delay(1000);
}
