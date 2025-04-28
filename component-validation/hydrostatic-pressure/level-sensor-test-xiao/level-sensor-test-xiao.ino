#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>

#define PIN_LEVEL_SENSE A0 
#define NUM_MEASUREMENTS 5
#define PERIOD_MEAS_MS 100
#define PERIOD_MOVING_AVERAGE_1_MS 5000
#define PERIOD_MOVING_AVERAGE_2_MS 30000

const uint16_t NUM_SAMPLES_1 = PERIOD_MOVING_AVERAGE_1_MS/PERIOD_MEAS_MS;
const uint16_t NUM_SAMPLES_2 =  PERIOD_MOVING_AVERAGE_2_MS/PERIOD_MEAS_MS;

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

uint16_t mov_avg_vals_1[NUM_SAMPLES_1] = {0};
uint16_t mov_avg_vals_2[NUM_SAMPLES_2] = {0};

uint32_t count = 0;
uint32_t sum_1 = 0;
uint32_t sum_2 = 0;

void setup(void) {
  Serial.begin(115200);
  delay(3000);

  u8x8.begin();
  u8x8.setFlipMode(1);   // set number from 1 to 3, the screen word will rotary 180

  pinMode(PIN_LEVEL_SENSE, INPUT);
  analogReadResolution(12);

  Serial.println(NUM_SAMPLES_1);
  Serial.println(NUM_SAMPLES_2);
}

void loop(void) {
  

  uint16_t val = analogRead(PIN_LEVEL_SENSE);

  int index_1 = count % NUM_SAMPLES_1;
  int index_2 = count % NUM_SAMPLES_2;
  Serial.print(count);
  Serial.print(" ");
  Serial.print(index_1);
  Serial.print(" ");
  Serial.println(index_2);

  // sum
  sum_1 -= mov_avg_vals_1[index_1];
  sum_1 += val;
  mov_avg_vals_1[index_1] = val;

  sum_2 -= mov_avg_vals_2[index_2];
  sum_2 += val;
  mov_avg_vals_2[index_2] = val;


  uint16_t val_avg_1 = sum_1/NUM_SAMPLES_1;
  uint16_t val_avg_2 = sum_2/NUM_SAMPLES_2;


  if (!(count % 10)){
    u8x8.clear();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0);
    u8x8.print("Level last: ");
    u8x8.print(val);
    u8x8.setCursor(0, 1);
    u8x8.print("5s avg: ");
    u8x8.print(val_avg_1);
    u8x8.setCursor(0, 2);
    u8x8.print("30s avg: ");
    u8x8.print(val_avg_2);
  }

  Serial.println(val);

  delay(PERIOD_MEAS_MS);
  count ++;

}