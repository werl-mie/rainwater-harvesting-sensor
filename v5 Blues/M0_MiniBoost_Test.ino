const int BOOST_EN = 5;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }

  pinMode(BOOST_EN, OUTPUT);

  Serial.println("Mini Boost EN test starting");
}

void loop() {
  Serial.println("BOOST LOW");
  digitalWrite(BOOST_EN, LOW);
  delay(6000);

  Serial.println("BOOST HIGH");
  digitalWrite(BOOST_EN, HIGH);
  delay(5500);
}