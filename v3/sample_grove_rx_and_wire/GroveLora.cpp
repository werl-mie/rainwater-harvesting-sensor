#include <Arduino.h>
#include "GroveLora.h"

#define DEVICE_ID 4

static char recv_buf[512];

GroveLora::GroveLora(){}

int GroveLora::init(void* struct_packet)
{
  

  int is_exist;
  if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
  {
      Serial.print("E5 module found...");
      is_exist = true;
      at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
      at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
      delay(200);
      Serial.println(" and configured.");
  }
  else
  {
      is_exist = false;
      Serial.print("No E5 module found.\r\n");
  }
  return is_exist;
} 

int GroveLora::at_send_check_response(char *p_ack, int timeout_ms, char*p_cmd, ...)
{
  // Serial.println("---");
  // Serial.print(p_cmd);
  // Serial.println("---");

  int ch = 0;
  int index = 0;
  int startMillis = 0;
  va_list args;
  memset(recv_buf, 0, sizeof(recv_buf));
  va_start(args, p_cmd);
  Serial1.printf(p_cmd, args);
  Serial.printf(p_cmd, args);
  va_end(args);
  delay(200);
  startMillis = millis();

  if (p_ack == NULL)
  {
      return 0;
  }

  do
  {
      while (Serial1.available() > 0)
      {
          ch = Serial1.read();
          recv_buf[index++] = ch;
          // Serial.print((char)ch);
          // delay(2);
      }

      if (strstr(recv_buf, p_ack) != NULL)
      {
          return 1;
      }

  } while (millis() - startMillis < timeout_ms);

  

  return 0;
}

int GroveLora::recv_prase(SensorDatum_t* p_datum)
{

    while (Serial1.available() == 0){}

    char *p_start = NULL;
    char *p_start2 = NULL;

    char ch;
    int index = 0;
    memset(recv_buf, 0, sizeof(recv_buf));
    do
    {
        ch = Serial1.read();
        recv_buf[index++] = ch;
        // Serial.print((char)ch);
        delay(2);
        p_start = strstr(recv_buf, "+TEST: RX \"");
    } while (p_start == NULL);

    int i_packet = index;

    do
    {
        ch = Serial1.read();
        recv_buf[index++] = ch;
        // Serial.print((char)ch);
        delay(2);
        p_start2 = strstr(recv_buf + i_packet, "\"");
    } while (p_start2 == NULL);


    if (index)
    {
        // char *p_start = NULL;
        char str_packet[32] = {0,};
        int rssi = 0;
        int snr = 0;
        int len = 0;

        Serial.println("recv_buf:");
        Serial.println(recv_buf);

        p_start = strstr(recv_buf, "+TEST: RX \"");
        if (p_start)
        {
            if (p_start && (1 == sscanf(p_start, "+TEST: RX \"%s\"", str_packet)))
            {
                Serial.println("Trying to decode message");
                const char* test = "vv";
                // int number;
                // int num2;
                // char name[20];

                // Serial.print("num: ");
                // Serial.println(number);
                // Serial.print("name: ");
                // Serial.println(name);
                // Serial.print("num2: ");
                // Serial.println(num2);


                // Don't ask me why, but it had to be done like this, or sscanf would fail silently
                int ts = 0;
                int site_id = 0;
                int node_id = 0;
                int sensor_type = 0;
                int data = 0;

                sscanf(str_packet,"%8x%4x%2x%2x%4x", &ts, &site_id, &node_id, &sensor_type, &data);

                p_datum->ts = (uint32_t)ts;
                p_datum->site_id = (uint16_t)site_id;
                p_datum->node_id = (uint8_t)node_id;
                p_datum->sensor_type = (uint8_t)sensor_type;
                p_datum->data = (uint16_t)data;
              
                // sscanf(test,"%8x%4x%2x", &decode_ts, &decode_site_id, &decode_node_id);//, &p_datum->sensor_type, &p_datum->data);
                // Serial.print("ts: ");
                // Serial.println(decode_ts);
                // Serial.print("site_id: ");
                // Serial.println(decode_site_id);
                // Serial.print("node_id: ");
                // Serial.println(decode_node_id);
            }

            p_start = strstr(recv_buf, "5345454544");
            if (p_start && (1 == sscanf(p_start, "5345454544%s", str_packet)))
            {
                str_packet[4] = 0;
                Serial.print(str_packet);
                Serial.print("\r\n");
            }

            return 1;
        }
    }
    return 0;
}

int GroveLora::node_recv(SensorDatum_t* p_datum)
{
    at_send_check_response("+TEST: RXLRPKT", 1000, "AT+TEST=RXLRPKT\r\n");

    recv_prase(p_datum);

    // int startMillis = millis();
    // do
    // {
    //     if (recv_prase())
    //     {
    //         return 1;
    //     }
    // } while (millis() - startMillis < timeout_ms);
    return 0;
}

int GroveLora::node_send(SensorDatum_t* p_datum)
{
    int ret = 0;
    char cmd[128];

    sprintf(cmd, "AT+TEST=TXLRPKT,\"%08x%04x%02x%02x%04x\"\r\n",p_datum->ts,p_datum->site_id,p_datum->node_id,p_datum->sensor_type,p_datum->data);

    ret = at_send_check_response("TX DONE", 3000, cmd);

    if (ret == 1)
    {
        Serial.print("Sent successfully!\r\n");
        // sprintf(msg_print,"tx,%s",msg);
        // Serial.print(msg_print);
    }
    else
    {
        Serial.print("Send failed!\r\n");
    }
    return ret;
}



