// Adapted from mwe_grove_tx_rx.ino
// May 19th, 2024
// Author: Eren Rudy

# include <Arduino.h>

#include "ArduinoLowPower.h"
#include <RTCZero.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Adafruit_SleepyDog.h>

#define YEAR 24
#define MONTH 1
#define DAY 1
#define HOUR 0
#define MINUTE 0
#define SECOND 0


#define SITE_ID 0
// #define RX

// MUTEX
// #define TYPE_CISTERN
#define TYPE_TLALOQUE
// #define TYPE_RAINGAUGE

#define PIN_CHIP_SELECT 4

#define POT_SENSOR_PWR 7 //WHITE
#define POT_SENSOR_AOUT A0 //BLUE

#define PIN_LVL_HIGH 11
#define PIN_LVL_LOW 12

#define PIN_RAIN_GAUGE 6

volatile uint16_t rain_gauge_count = 0;

// Logging 
RTCZero rtc_samd;
File dataFile;
DateTime now;
char sprintf_buffer[19]; //timestamp string is exactly 19 characters long
String buffer_str = "";
// Logging 


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
        char params[32] = {0,};

        p_start = strstr(recv_buf, "+TEST: RX \"5345454544");
        if (p_start)
        {
            if (p_start && (1 == sscanf(p_start, "+TEST: RX \"5345454544%s\"", data)))
            {
                switch(data[1]){
                  case 48:
                    data[10] = 0;
                    sprintf(msg,"[DATA] %s",data);
                    break;
                  case 49:
                    data[6] = 0;
                    sprintf(msg,"[DATA] %s",data);
                    break;
                  case 50:
                    data[6] = 0;
                    sprintf(msg,"[DATA] %s",data);
                    break;
                  default:
                    sprintf(msg,"[ERROR] INVALID_ID\0");
                }

                Serial.print(msg);
                Serial.print("\r\n");
            }

            p_start = strstr(recv_buf, "RSSI:");
            if (p_start && (1 == sscanf(p_start, "RSSI: %d,", &rssi)))
            {
            }
            p_start = strstr(recv_buf, "SNR:");
            if (p_start && (1 == sscanf(p_start, "SNR: %d", &snr)))
            {

            }
            p_start = strstr(recv_buf, "LEN:");
            if (p_start && (1 == sscanf(p_start, "LEN: %d", &len)))
            {
            }

            sprintf(params, "[PARAMS] rssi: %d snr: %d len: %d", rssi, snr,len);
            Serial.print(params);
            Serial.print("\r\n");

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
    // Read potentiometer
    #ifdef TYPE_CISTERN
        pinMode(POT_SENSOR_PWR, OUTPUT);
        digitalWrite(POT_SENSOR_PWR, HIGH);
        delay(100);
        uint16_t reading_pot = analogRead(POT_SENSOR_AOUT);
        digitalWrite(POT_SENSOR_PWR, LOW);
        pinMode(POT_SENSOR_PWR, INPUT);
    #endif

    uint8_t reading_lvl_high = digitalRead(PIN_LVL_HIGH);
    uint8_t reading_lvl_low = digitalRead(PIN_LVL_LOW);


    // static uint16_t count = 0;
    int ret = 0;
    char data[32];
    char cmd[128];

    memset(data, 0, sizeof(data));
    #ifdef TYPE_CISTERN
        sprintf(data, "00%04X%02X%02X\0", reading_pot, reading_lvl_low, reading_lvl_high);
    #endif
    #ifdef TYPE_TLALOQUE
        sprintf(data, "01%02X%02x\0", reading_lvl_low, reading_lvl_high);
    #endif   
    #ifdef TYPE_RAINGAUGE
        sprintf(data, "02%04X\0", rain_gauge_count);
        rain_gauge_count = 0;
    #endif

    sprintf(cmd, "AT+TEST=TXLRPKT,\"5345454544%s\"\r\n", data);

    ret = at_send_check_response("TX DONE", 2000, cmd);
    if (ret == 1)
    { 
        Serial.print(data);
        Serial.print(" sent successfully!\r\n");
        log_to_sd(get_timestamp_str() + "," + String(data) + "\r\n");
    }
    else
    {
        Serial.print("Send failed!\r\n");
        log_to_sd(get_timestamp_str() + " Send failed!\r\n");

    }
    return ret;
}
#endif

void setup(void)
{

  Watchdog.enable(16000);

    Serial.begin(115200);
    // while (!Serial);

    Serial1.begin(9600);
    Serial.print("ping pong communication!\r\n");


    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);

    }
    else
    {
        is_exist = false;
        Serial.print("No E5 module found.\r\n");
    }

    if (!SD.begin(PIN_CHIP_SELECT)) {
      // Serial.println("Card failed, or not present");
      while (1);
    }

    #ifdef TYPE_CISTERN
      log_to_sd("CISTERN, SITE_ID: " + String(SITE_ID) + "\n\r");
    #endif
    #ifdef TYPE_TLALOQUE
      log_to_sd("TLALOQUE, SITE_ID: " + String(SITE_ID) + "\n\r");
    #endif
    #ifdef TYPE_RAINGAUGE
      log_to_sd("RAINGAUGE, SITE_ID: " + String(SITE_ID) + "\n\r");
    #endif

    rtc_samd.begin();
    rtc_samd.setDate(DAY, MONTH, YEAR);
    rtc_samd.setTime(HOUR, MINUTE, SECOND);

    pinMode(PIN_LVL_HIGH, INPUT_PULLUP);
    pinMode(PIN_LVL_LOW, INPUT_PULLUP);
    LowPower.attachInterruptWakeup(PIN_LVL_HIGH, isr_dummy, CHANGE);
    LowPower.attachInterruptWakeup(PIN_LVL_LOW, isr_dummy, CHANGE);

    pinMode(PIN_RAIN_GAUGE, INPUT_PULLUP);
    LowPower.attachInterruptWakeup(PIN_RAIN_GAUGE, isr_bucket, RISING);
    
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
      at_send_check_response("+LOWPOWER: SLEEP", 1000, "AT+LOWPOWER\r\n");


      Watchdog.disable();
      #ifdef TYPE_CISTERN
          LowPower.sleep(900000); // 15 minutes
          // delay(5000);
      #endif
      #ifdef TYPE_TLALOQUE
          LowPower.sleep(915000); // 15 minutes + 15 seconds
      #endif   
      #ifdef TYPE_RAINGAUGE
          LowPower.sleep(885000); // 15 minutes - 15 seconds
      #endif
      Watchdog.enable();

      
      // LowPower.sleep(5000);
      // delay(5000);
      
      // wake up LoRa modem
      Serial1.printf("A"); 
      delay(2);
        // validate that LoRa modem is awake
        // if(at_send_check_response("+AT: OK", 100, "AT\r\n")){
        //   Serial.println("LoRa radio AWAKE");
        // }
# endif
    }
    Watchdog.reset();
}

void isr_bucket(){
  rain_gauge_count ++;
}

void isr_dummy(){}

String get_timestamp_str(){
  sprintf(sprintf_buffer, "%d-%.2d-%.2d %.2d:%.2d:%.2d", rtc_samd.getYear(), rtc_samd.getMonth(), rtc_samd.getDay(), rtc_samd.getHours(), rtc_samd.getMinutes(), rtc_samd.getSeconds());
  buffer_str = sprintf_buffer;
  return buffer_str;
}

void log_to_sd(String str){

    dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(str);

      // digitalWrite(LED_BUILTIN,HIGH);
      // delay(10);
      // digitalWrite(LED_BUILTIN,LOW);

      dataFile.close();

      // print to the serial port too:
      Serial.print(str);
    }

    // else {
    //   SD_failure = 1;
    // }
}

