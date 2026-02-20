#include <Arduino.h>
#include "GroveLora.h"
#include <RTCZero.h>

GroveLora dev;
RTCZero rtc;
SensorDatum_t myData = {0, 4, 3, 6, 0};

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  while(!Serial && millis() < 5000); 
  
  rtc.begin();
  Serial.println("TX STARTING...");
  
  dev.sendCmd("AT+MODE=TEST\r\n"); delay(200);
  dev.sendCmd("AT+TEST=RFCFG,915,SF7,125,12,15,14,ON,OFF,OFF\r\n"); delay(200);
  Serial.println("TX CONFIGURED (915MHz)");
}

void loop() {
  myData.ts = rtc.getEpoch();
  myData.data = analogRead(A0);
  
  char cmd[128];
  sprintf(cmd, "AT+TEST=TXLRPKT,\"%08x%04x%02x%02x%04x\"\r\n", 
          myData.ts, myData.site_id, myData.node_id, myData.sensor_type, myData.data);
  
  Serial.print("Sending A0: "); Serial.println(myData.data);
  dev.sendCmd(cmd);
  delay(5000); 
}