#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>

#include <Dps310.h>

Dps310 Dps310PressureSensor = Dps310();

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  // OLEDs without Reset of the Display


void setup(void) {
  Serial.begin(115200);
  delay(3000);

  u8x8.begin();
  u8x8.setFlipMode(1);  // set number from 1 to 3, the screen word will rotary 180

  Dps310PressureSensor.begin(Wire);
  Serial.println("Init complete!");
}

void loop(void) {

  float pressure;
  float temperature;
  uint8_t oversampling = 7;
  int16_t ret;

  ret = Dps310PressureSensor.measurePressureOnce(pressure, oversampling);
  ret |= Dps310PressureSensor.measureTempOnce(temperature, oversampling);

  if (ret != 0) {
    Serial.print("FAIL! ret = ");
    Serial.println(ret);
  } else {
    Serial.print(pressure);
    Serial.print(", ");
    Serial.println(temperature);

    u8x8.clear();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0);
    u8x8.print("Pressure:");
    u8x8.setCursor(0, 1);
    u8x8.print(pressure);
  }

  delay(1000);
}


