#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin();

  Serial.println("Scanning I2C bus...");

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found device at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  Serial.println("Done.");
}

void loop() {}