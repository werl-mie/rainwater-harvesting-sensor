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

#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <MKRNB.h>

#include "arduino_secrets.h"

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

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1)
      ;
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  nbAccess.setTimeout(30*1000);

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
  
  if (nbAccess.status() != NB_READY){//|| gprs.status() != GPRS_READY) {
    Serial.print("Attempting to connect to the cellular network");
    while ((nbAccess.begin(pinnumber) != NB_READY)){
      // failed, retry
      Serial.print(".");
    }
  }

  if (nbAccess.status() == NB_READY){
    Serial.println("Connected to NB, connecting to MQTT");

    if (!mqttClient.connected()) {
      // MQTT client is disconnected, connect
      // connectMQTT();
      while (!mqttClient.connect(broker, 8883)) {
          // failed, retry
          Serial.print(".");
          delay(5000);
      }
    }

    // poll for new MQTT messages and send keep alives
    mqttClient.poll();

    // publish a message roughly every 5 seconds.
    // if (millis() - lastMillis > 5000) {
    //   lastMillis = millis();

    publishMessage();
    // }

    delay(2000);

    // Serial.println("Shutting down modem...");

    if (nbAccess.shutdown()){
      Serial.println("Modem successfully shutdown");
    } else {
      Serial.println("Failed to shutdown modem");
    }

  } else {
    Serial.println("Not connected");
  }

  Serial.println(".....");
  Serial.println();
}

unsigned long getTime() {
  // get the current time from the NB module
  return nbAccess.getTime();
}

void connectNB() {
  Serial.println("Attempting to connect to the cellular network");

  nbAccess.begin();

  // while ((nbAccess.begin(pinnumber) != NB_READY)){//} || (gprs.attachGPRS() != GPRS_READY)) {
  //   // failed, retry
  //   Serial.print(".");
  //   delay(1000);
  // }

   if (nbAccess.status() != NB_READY) {
          Serial.println("Failed to connect to cellular");
    } else {
      Serial.println("Connected to cellular");
    }

  // Serial.println("You're connected to the cellular network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe("arduino/incoming");
}

void publishMessage() {
  unsigned long t = getTime();
  Serial.print("Publishing message at time ");
  Serial.println(t);

  // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage("arduino/outgoing");
  // mqttClient.print("hello ");
  // mqttClient.print("{\"millis\": ");
  // mqttClient.print(millis());
  // mqttClient.print("}");

  mqttClient.print("01020304F0FF0F");

  mqttClient.endMessage();
}

void onMessageReceived(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();

  Serial.println();
}
