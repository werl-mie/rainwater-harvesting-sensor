
#include <ArduinoLowPower.h>
#include <MKRNB.h>
#include <MQTT.h>


// Define parameters for parsing data stream
const uint8_t LEN_RXBUF = 32;

const uint8_t LEN_ADDR_GW = 4;
const uint8_t OFFSET_ADDR_GW = 7;

const uint8_t LEN_ADDR_NODE = 4;
const uint8_t OFFSET_ADDR_NODE = 14;

const uint8_t LEN_VAL_POT = 4;
const uint8_t OFFSET_VAL_POT = OFFSET_ADDR_NODE + LEN_ADDR_NODE;

const uint8_t LEN_VAL_LVL1 = 2;
const uint8_t OFFSET_VAL_LVL1 = OFFSET_VAL_POT + LEN_VAL_POT;

const uint8_t LEN_VAL_LVL2 = 2;
const uint8_t OFFSET_VAL_LVL2 = OFFSET_VAL_LVL1 + LEN_VAL_LVL1;

char rxBuf[LEN_RXBUF];
char* ptr; // dummy pointer for strtol

char addr_gw_str[LEN_ADDR_GW+1] = "";
long addr_gw = 0;

char addr_node_str[LEN_ADDR_GW+1] = "";
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

    // Serial.print(str_out);
    // Serial.print(" ");

    return true;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600);

  delay(500);

  

  //Set a timeout for the UART receive
  // Serial1.setTimeout(UART_RECEIVE_TIMEOUT_MS);

  // while(!Serial.available()){}

  if (Serial.available()){
    Serial.print("Hello World\r\n");
  }

  //TODO: Setup the interrupts
}

void loop() {

  if (Serial1.available()) {
    // Simple pass-through validation
    // int byte = Serial1.read();
    // Serial.write(byte);
  }
  
  if (Serial1.available()) {
    // Simple pass-through validation
    // int byte = Serial1.read();
    // Serial.write(byte);
    

    Serial1.readBytesUntil(10,rxBuf, LEN_RXBUF);

    // Serial.write(rxBuf);
    // Serial.write(10);

    if(strstr(rxBuf,"[DATA]")){
      extract_str(addr_gw_str, 4, OFFSET_ADDR_GW, rxBuf, LEN_RXBUF);
      addr_gw = strtol(addr_gw_str,&ptr,16);

      extract_str(addr_node_str, 4, OFFSET_ADDR_NODE, rxBuf, LEN_RXBUF);
      addr_node = strtol(addr_node_str,&ptr,16);

      extract_str(val_pot_str, LEN_VAL_POT, OFFSET_VAL_POT, rxBuf, LEN_RXBUF);
      val_pot = strtol(val_pot_str,&ptr,16);

      extract_str(val_lvl1_str, LEN_VAL_LVL1, OFFSET_VAL_LVL1, rxBuf, LEN_RXBUF);
      val_lvl1 = strtol(val_lvl1_str,&ptr,16);
      extract_str(val_lvl2_str, LEN_VAL_LVL2, OFFSET_VAL_LVL2, rxBuf, LEN_RXBUF);
      val_lvl2 = strtol(val_lvl2_str,&ptr,16);

      // Serial.print(val_lvl1_str);
      Serial.print(" addr_gw: ");
      Serial.print(addr_gw);
      Serial.print(" addr_node: ");
      Serial.print(addr_node);
      Serial.print(" val_pot: ");
      Serial.print(val_pot);
      Serial.print(" val_lvl1: ");
      Serial.print(val_lvl1);
      Serial.print(" val_lvl2: ");
      Serial.print(val_lvl2);
      Serial.write(10);
    }
  }
}
