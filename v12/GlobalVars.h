#ifndef GLOBALVARS_H
#define GLOBALVARS_H

// Libraries
#include <RTCZero.h>
#include <RTClib.h>

// GLOBAL VARIABLES 
extern volatile int bucketCount;
extern unsigned long globalRecordID;
extern unsigned long lastBucketTime;
extern volatile bool flag_event_change;
extern volatile bool rtcTriggered;


// DEBUG MODE
constexpr bool DEBUG_MODE = true; // constexpr bool is safer 

// ------------ Notecard ------------
constexpr char PRODUCT_UID[] = "ca.utoronto.mail.a.diazlozanopatino:rwhxochi"; //change to rwhxochi for reployment
constexpr char FIRMWARE_VERSION[] = "1.0.0";

// ------------ Pin assignments ------------
constexpr uint8_t PIN_SD_CS = 4; // SD Card
constexpr uint8_t PIN_RTC_INT = 6; // RTC

// Digital Sensors
constexpr uint8_t PIN_BUCKET = 10;
constexpr uint8_t PIN_LEAF = 9;
constexpr uint8_t PIN_FLOAT_BOTTOM = 12;
constexpr uint8_t PIN_FLOAT_TOP = 11;

// Analog Sensors
constexpr uint8_t PIN_PRESSURE = A2;
constexpr uint8_t PIN_CHLO = A1;
constexpr uint8_t PIN_TURBIDITY = A3;

// Sensor Power Enable
constexpr uint8_t PIN_MINIBOOST_ENABLE = 5;

// ------------ SD Card ------------
constexpr char SENSOR_LOG_FILE[] = "tdata.csv"; //Change to desired file name

// CSV File Headers
constexpr char FILE_HEADER[] = "recordID,epoch,bucket,leaf,ftop,fbot,chlo,turbidity_adc,turbidity_v,pressure_adc,pressure_v,batteryV"; 

constexpr char RAINFALL_HEADER[] = "recordID,startEpoch,endEpoch,tips";

// ------------ Sensor configuration ------------

constexpr uint8_t ADC_BITS = 12;
constexpr float ADC_REFERENCE = 2.23f;
constexpr uint16_t SENSOR_SETTLE_DELAY_MS = 50;

// Pressure conversion
constexpr float PRESSURE_GAIN = 1.0f;
constexpr float PRESSURE_OFFSET = 0.0f;

// Turbidity conversion
constexpr float TURBIDITY_GAIN = 1.0f;
constexpr float TURBIDITY_OFFSET = 0.0f;

// Rainbucket tipping reset time
#define RAINFALL_EVENT_GAP 600
constexpr uint32_t BUCKET_RESET_MS = 600000UL; // Reset tipping bucket counter after inactivity

//Sensor warm up time
constexpr uint16_t SENSOR_WARMUP_MS = 1500; 

// Defined buffer size for char variables
constexpr size_t CSV_LINE_BUFFER_SIZE = 128; 


// ------------ RTC & Watchdog configuration ------------
#if DEBUG_MODE
constexpr uint16_t RTC_COUNTDOWN_VALUE = 10; // Test RTC wake interval: 10s
constexpr PCF8523TimerClockFreq RTC_COUNTDOWN_FREQ = PCF8523_FrequencySecond;
#else
constexpr uint16_t RTC_COUNTDOWN_VALUE = 5; // RTC wake interval: change in field to 5 mins or longer
constexpr PCF8523TimerClockFreq RTC_COUNTDOWN_FREQ = PCF8523_FrequencyMinute;
#endif

// Watchdog configuration 
constexpr uint32_t WATCHDOG_TIMEOUT_MS = 16000;


// ------------ Notecard configuration ------------
const uint16_t NOTECARD_OUTBOUND_HOURS = 12;
const uint16_t NOTECARD_INBOUND_HOURS = 2;

const uint16_t NOTECARD_DEBUG_OUTBOUND_MIN = 2; // Shorter upload time for test/debugging
const uint16_t NOTECARD_DEBUG_INBOUND_MIN = 2;

// Max number of records attempted to sync in one batch
constexpr uint8_t NOTECARD_BATCH_SIZE = 10; 

//Synch with state file on SD
constexpr char SYNC_STATE_FILE[] = "sync.dat";

// Notecard note files
constexpr char NOTECARD_SENSOR_FILE[] = "sensort.qo";//"sensors.qo";


#endif



