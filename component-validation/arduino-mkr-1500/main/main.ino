/*
  AWS IoT NB

  This sketch securely connects to an AWS IoT using MQTT over NB IoT/LTE Cat M1.
  It uses a private key stored in the ATECC508A and a public
  certificate for SSL/TLS authetication.

  It publishes a message every 5 seconds to arduino/outgoing
  topic and subscribes to messages on the arduino/incoming
  topic.

  The circuit:
  - MKR NB 1500 board
  - Antenna
  - SIM card with a data plan
  - LiPo battery

  This example code is in the public domain.
*/

#define DEBUG //Only if connected to computer

#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <MKRNB.h>
#include <Adafruit_SleepyDog.h>

#include "arduino_secrets.h"
#include "params_parsing.h"

/////// Enter your sensitive data in arduino_secrets.h
const char pinnumber[] = SECRET_PINNUMBER;
const char broker[] = SECRET_BROKER;
const char* certificate = SECRET_CERTIFICATE;

NB nbAccess;
GPRS gprs;

NBClient nbClient;                  // Used for the TCP socket connection
BearSSLClient sslClient(nbClient);  // Used for SSL/TLS connection, integrates with ECC508
MqttClient mqttClient(sslClient);

unsigned long lastMillis = 0;

// Define variables parsing variables

char rxBuf[LEN_RXBUF];
char* ptr; // dummy pointer for strtol

char addr_parent_str[LEN_ADDR_PARENT+1] = "";
long addr_parent = 0;

char addr_node_str[LEN_ADDR_NODE+1] = "";
long addr_node = 0;

char val_pot_str[LEN_VAL_POT+1] = "";
long val_pot = 0;

char val_lvl1_str[LEN_VAL_LVL1+1] = "";
long val_lvl1 = 0;

char val_lvl2_str[LEN_VAL_LVL2+1] = "";
long val_lvl2 = 0;

bool extract_str(char* str_out, uint8_t len, uint8_t offset, const char* buff, uint8_t buff_len){

    for (int i = 0; i < len; i++){
      if (i+offset < buff_len){
        str_out[i] = buff[i+offset];
      } else {
        return false;
      }
    }

    return true;
}

void setup() {

  Watchdog.enable(16000);

#ifdef DEBUG
  Serial.begin(115200);
#endif
  Serial1.begin(9600);

#ifdef DEBUG
  while (!Serial);
#endif

  if (!ECCX08.begin()) {
#ifdef DEBUG
    Serial.println("No ECCX08 present!");
#endif
    while (1);
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Optional, set the client id used for MQTT,
  // each device that is connected to the broker
  // must have a unique client id. The MQTTClient will generate
  // a client id for you based on the millis() value if not set
  //
  mqttClient.setId("mkr-nb-1500-0");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);
}

void loop() {
  if (nbAccess.status() != NB_READY || gprs.status() != GPRS_READY) {
    Watchdog.disable();
    connectNB();
    Watchdog.enable(16000);
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    Watchdog.disable();
    connectMQTT();
    Watchdog.enable(16000);
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  if (Serial1.available()) {
    // Simple pass-through validation
    // int byte = Serial1.read();
    // Serial.write(byte);

    Serial1.readBytesUntil(10,rxBuf, LEN_RXBUF);

    Serial.write(rxBuf);
    Serial.write(10);

    if(strstr(rxBuf,"[DATA]")){
      extract_str(addr_parent_str, 4, OFFSET_ADDR_PARENT, rxBuf, LEN_RXBUF);
      addr_parent = strtol(addr_parent_str,&ptr,16);

      extract_str(addr_node_str, 4, OFFSET_ADDR_NODE, rxBuf, LEN_RXBUF);
      addr_node = strtol(addr_node_str,&ptr,16);

      extract_str(val_pot_str, LEN_VAL_POT, OFFSET_VAL_POT, rxBuf, LEN_RXBUF);
      val_pot = strtol(val_pot_str,&ptr,16);

      extract_str(val_lvl1_str, LEN_VAL_LVL1, OFFSET_VAL_LVL1, rxBuf, LEN_RXBUF);
      val_lvl1 = strtol(val_lvl1_str,&ptr,16);
      extract_str(val_lvl2_str, LEN_VAL_LVL2, OFFSET_VAL_LVL2, rxBuf, LEN_RXBUF);
      val_lvl2 = strtol(val_lvl2_str,&ptr,16);

      publishMessage();
    }
  }

  Watchdog.reset();
}

unsigned long getTime() {
  // get the current time from the NB module
  return nbAccess.getTime();
}

void connectNB() {
#ifdef DEBUG
  Serial.println("Attempting to connect to the cellular network");
#endif

  while ((nbAccess.begin(pinnumber) != NB_READY) || (gprs.attachGPRS() != GPRS_READY)) {
    // failed, retry
#ifdef DEBUG
    Serial.print(".");
#endif
    delay(1000);
  }
#ifdef DEBUG
  Serial.println("You're connected to the cellular network");
  Serial.println();
#endif
}

void connectMQTT() {
#ifdef DEBUG
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");
#endif

  // Watchdog.enable(16000);


  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
#ifdef DEBUG
    Serial.print(".");
#endif
    // Watchdog.reset();
    delay(5000);
    // Watchdog.reset();
  }

  // Watchdog.disable();

#ifdef DEBUG
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();
#endif

  // subscribe to a topic
  mqttClient.subscribe("arduino/incoming");
}

void publishMessage() {
  unsigned long ts = getTime();
#ifdef DEBUG
  Serial.print("Publishing message at time ");
  Serial.println(ts);
#endif

  // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage("arduino/outgoing");
  // mqttClient.print("hello ");
  mqttClient.print("{\"ts\": ");
  mqttClient.print(ts);
  mqttClient.print(",");

  mqttClient.print("\"id_parent\": ");
  mqttClient.print(addr_parent);
  mqttClient.print(",");

  mqttClient.print("\"id\": ");
  mqttClient.print(addr_node);
  mqttClient.print(",");

  mqttClient.print("\"val_pot\": ");
  mqttClient.print(val_pot);
  mqttClient.print(",");

  mqttClient.print("\"val_lvl1\": ");
  mqttClient.print(val_lvl1);
  mqttClient.print(",");

  mqttClient.print("\"val_lvl2\": ");
  mqttClient.print(val_lvl2);

  mqttClient.print("}");

  mqttClient.endMessage();
}

void onMessageReceived(int messageSize) {
  // we received a message, print out the topic and contents
#ifdef DEBUG
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
#endif

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
#ifdef DEBUG
    Serial.print((char)mqttClient.read());
#endif
  }
#ifdef DEBUG
  Serial.println();

  Serial.println();
#endif
}
