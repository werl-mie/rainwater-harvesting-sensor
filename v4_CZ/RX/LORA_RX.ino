#include <Arduino.h>
#include "GroveLora.h"

GroveLora dev;
SensorDatum_t incomingData;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  while(!Serial && millis() < 5000); 

  Serial.println("RX STARTING...");
  dev.sendCmd("AT+MODE=TEST\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RFCFG,915,SF7,125,12,15,14,ON,OFF,OFF\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RXLRPKT\r\n");
  Serial.println("RX LISTENING ON 915MHz...");
}

void loop() {
  if (dev.checkRX(&incomingData)) {
    Serial.println("---------------------------");
    Serial.print("!!! SUCCESS! Value: ");
    Serial.println(incomingData.data);
    Serial.println("---------------------------");
  }
}