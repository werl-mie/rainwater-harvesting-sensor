# include <Arduino.h>
# include <U8x8lib.h>

#include "ArduinoLowPower.h"

// #define RX
#define DEVICE_ID 4

#ifdef RX
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/*reset=*/U8X8_PIN_NONE);
// U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/*clock=*/ SCL, /*data=*/ SDA, /*reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
#endif

static char recv_buf[512];
// static char recv_buf2[512];
static bool is_exist = false;

static int send_ret = 0;

static int at_send_check_response(char *p_ack, int timeout_ms, char*p_cmd, ...)
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
    // Serial.printf(p_cmd, args);
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

#ifdef RX
static int recv_prase(void)
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
        char data[32] = {
            0,
        };
        int rssi = 0;
        int snr = 0;
        int len = 0;

        char device_id_str[3] = {0,};
        uint8_t device_id = 0;
        char count_str[8] = {0,};
        uint32_t count = 0;
        char data_hex_str[10] = {0,};
        uint32_t data_hex = 0;

        char msg[32] = {0,};

        p_start = strstr(recv_buf, "+TEST: RX \"");
        if (p_start)
        {
            if (p_start && (1 == sscanf(p_start, "+TEST: RX \"%s\"", data)))
            {

                strncpy(data_hex_str, data, 8);
                // data_hex = atoi(data_hex_str);
                sprintf(msg, "rx,%s\r\n",data_hex_str);

                Serial.print(msg);


                // Serial.println();
                // Serial.println(data_hex);


                // strncpy(device_id_str, data, 3);
                // device_id = atoi(device_id_str);
                // Serial.println(device_id_str);

                // strncpy(count_str, data+3,5);
                // Serial.println(count_str);
                // sscanf(count_str,"%05X",&count);
                
                // Serial.print("device_id: ");
                // Serial.print(device_id);
                // Serial.print("\r\n");
                // Serial.print("count: ");
                // Serial.print(count);
                // Serial.print("\r\n");
            }

            p_start = strstr(recv_buf, "5345454544");
            if (p_start && (1 == sscanf(p_start, "5345454544%s", data)))
            {
                data[4] = 0;
                u8x8.setCursor(0, 4);
                u8x8.print("               ");
                u8x8.setCursor(2, 4);
                u8x8.print("RX: 0x");
                u8x8.print(data);
                Serial.print(data);
                Serial.print("\r\n");
            }

            p_start = strstr(recv_buf, "RSSI:");
            if (p_start && (1 == sscanf(p_start, "RSSI:%d,", &rssi)))
            {
                u8x8.setCursor(0, 6);
                u8x8.print("                ");
                u8x8.setCursor(2, 6);
                u8x8.print("rssi:");
                u8x8.print(rssi);
            }
            p_start = strstr(recv_buf, "SNR:");
            if (p_start && (1 == sscanf(p_start, "SNR:%d", &snr)))
            {
                u8x8.setCursor(0, 7);
                u8x8.print("                ");
                u8x8.setCursor(2, 7);
                u8x8.print("snr :");
                u8x8.print(snr);
            }
            p_start = strstr(recv_buf, "LEN:");
            if (p_start && (1 == sscanf(p_start, "LEN:%d", &len)))
            {
                u8x8.setCursor(0, 5);
                u8x8.print("                ");
                u8x8.setCursor(2, 5);
                u8x8.print("len :");
                u8x8.print(len);
            }

            return 1;
        }
    }
    return 0;
}

static int node_recv(uint32_t timeout_ms)
{
    at_send_check_response("+TEST: RXLRPKT", 1000, "AT+TEST=RXLRPKT\r\n");

    recv_prase();

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

#else
static int node_send(void)
{
    static uint16_t count = 0;
    int ret = 0;
    char data[32];
    char cmd[128];
    char device_id[32];
    char msg[16];
    char msg_print[16];

    memset(data, 0, sizeof(data));
    sprintf(data, "%05X", count);
    memset(device_id, 0, sizeof(device_id));
    sprintf(device_id, "%03X", DEVICE_ID);
    // sprintf(cmd, "AT+TEST=TXLRSTR,\"5345454544%s\"\r\n", data);
    sprintf(msg, "%s%s\r\n",device_id,data);
    sprintf(cmd, "AT+TEST=TXLRPKT,\"%s%s\"\r\n",device_id,data);
    // sprintf(cmd, "AT+TEST=TXLRSTR,\"AaBb\"\r\n");

    ret = at_send_check_response("TX DONE", 2000, cmd);

    if (ret == 1)
    {
        count++;
        // Serial.print("Sent successfully!\r\n");
        sprintf(msg_print,"tx,%s",msg);
        Serial.print(msg_print);
    }
    else
    {
        Serial.print("Send failed!\r\n");
    }
    return ret;
}
#endif

void setup(void)
{

#ifdef RX
    u8x8.begin();
    u8x8.setFlipMode(1);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
#endif

    Serial.begin(115200);
    // while (!Serial);

    Serial1.begin(9600);
    Serial.print("ping pong communication!\r\n");

#ifdef RX
    u8x8.setCursor(0, 0);
#endif

    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);

# ifdef RX
        u8x8.setCursor(5, 0);
        u8x8.print("RX");
# endif

    }
    else
    {
        is_exist = false;
        Serial.print("No E5 module found.\r\n");
#ifdef RX
        u8x8.setCursor(0, 1);
        u8x8.print("unfound E5 !");
#endif 
    }
}

void loop(void)
{
    if (is_exist)
    {
# ifdef RX
      node_recv(2000);
# else
      // Serial.print("TEST");
      node_send();
      // Serial.println(send_ret);
      LowPower.sleep(600000); // 10 minutes
      // LowPower.sleep(3000);
      // delay(5000);
# endif
    }
}