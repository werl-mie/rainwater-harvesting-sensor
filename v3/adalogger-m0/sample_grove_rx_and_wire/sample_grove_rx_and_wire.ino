#include <Arduino.h>
#include "GroveLora.h"
#include "ArduinoLowPower.h"
#include <RTCZero.h>

#include <Wire.h>
#include <cppQueue.h>

// #define RX // if not, device is TX
#define SITE_ID 4
#define NODE_ID 3

volatile uint8_t count_test_sends = 0;

char sprint_buf[160];

cppQueue q(sizeof(SensorDatum_t), 5, FIFO, true);

static bool is_exist = true;

RTCZero rtc;

GroveLora dev;
SensorDatum_t datum_temp = {
    0, //ts
    SITE_ID,
    NODE_ID,
    6, // Sensor type
    9, // Sensor value
};

void setup(void)
{

    Serial.begin(115200);
    Serial1.begin(9600);
    delay(2000);

    #ifdef RX
    Serial.println("LoRa RX");
    Wire.begin(4);                // Join i2c bus with address #4
    Wire.onRequest(requestEvent); // Register master 'write' handling
    #else
    Serial.print("LoRa TX");
    pinMode(10,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(10),test_button_isr, FALLING);
    #endif

    is_exist = dev.init(&datum_temp);

    if(dev.at_send_check_response("+AT: OK", 100, "AT\r\n")){
      Serial.println("LoRa radio AWAKE");
    }

    rtc.begin();
    uint32_t epoch = rtc.getEpoch();
    // Serial.println(rtc.getEpoch);
    Serial.println(epoch);
    
}

void loop(void)
{
    if (is_exist)
    {
# ifdef RX
                
      dev.node_recv(&datum_temp);
      sprintf(sprint_buf, "Received: {\n\tts: %8x\n\tsite_id: %04x\n\tnode_id: %04x\n\tsensor_type: %02x\n\tdata: %04x\r\n}\r\n",datum_temp.ts,datum_temp.site_id,datum_temp.node_id,datum_temp.sensor_type,datum_temp.data);
      Serial.print(sprint_buf);

      uint8_t ret = q.push(&datum_temp);
      Serial.print("Status of push to queue: ");
      Serial.println(ret);

# else
      // Button triggered testing
      while (count_test_sends > 0)
      {
        // noInterrupts();
        SensorDatum_t new_data;
        new_data.ts = rtc.getEpoch();
        #ifdef TYPE_CISTERN
          new_data.node_id = 3
        #endif
        new_data.data = analogRead(A0);
        dev.node_send(&new_data);
        count_test_sends--;
        // interrupts();
      }
      // Continuous Testing
      // datum_temp.ts = rtc.getEpoch();
      // dev.node_send(&datum_temp);
      // delay(1000);

      // Sleep LoRa modem
      //   at_send_check_response("+LOWPOWER: SLEEP", 1000, "AT+LOWPOWER\r\n");
      // LowPower.sleep(600000); // 10 minutes
      //   LowPower.sleep(5000);
      
      // wake up LoRa modem
      //   Serial1.printf("A"); 
      //   delay(2);
      // validate that LoRa modem is awake
      // if(at_send_check_response("+AT: OK", 100, "AT\r\n")){
      //   Serial.println("LoRa radio AWAKE");
      // }
# endif
    }
}

void test_button_isr(void){
  count_test_sends++;
}


void requestEvent(void)
{
  Serial.println("requestEvent");
  uint8_t reg = Wire.read();    // receive byte as an integer
  SensorDatum_t temp;

  Serial.print("Received reg: ");
  Serial.print(reg);


  switch (reg){
    case 0:
      Serial.print("Received request for queue count, sending: ");
      Serial.println(q.getCount());
      Wire.write(q.getCount());
      break;
    case 1:
      {
        // Serial.print(" q.getCount() = ");
        // Serial.println(q.getCount());

        if (q.getCount() < 1){
          Serial.println("Queue empty, sending empty data of size ");
          Serial.println(sizeof(SensorDatum_t));
          uint8_t arr_nodata[sizeof(SensorDatum_t)] = {0xfe,};
          Wire.write(arr_nodata,sizeof(SensorDatum_t));
          break;
        }

        uint8_t num_bytes = Wire.read();

        Serial.print("num_bytes = ");
        Serial.println(num_bytes);

        if (num_bytes == sizeof(SensorDatum_t)) {
          q.pop(&temp);
          // uint8_t dtype = temp.data_type;
          // uint16_t val = temp.datum_temp;
          // uint8_t val_msb = val >> 8;
          // uint8_t val_lsb = val;
          uint8_t arr_send[sizeof(SensorDatum_t)] = {0xab,};
          Wire.write(arr_send,sizeof(SensorDatum_t));
        }
        break;
      }
      default:
        Serial.println("Invalid command");
        break;
  }
}