// For Adafruit M0 
// Dummy gateway that transmits replcate [DATA] strings over serial to validate parsing on the Arduino MKR NB 1500

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600);

  Serial.print("Hello World\r\n");

  //TODO: Setup the interrupts
}

void loop() {
  Serial1.println("[DATA] 67A3 6 E84400B00102");
  delay(1000);

}