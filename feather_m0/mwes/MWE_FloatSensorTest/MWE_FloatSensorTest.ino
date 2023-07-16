
void setup() {

  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(8, INPUT_PULLUP);
}


void loop() {

  // Serial.println(digitalRead(13));

  // digitalWrite(LED_BUILTIN,HIGH);
  // delay(1000);
  // digitalWrite(LED_BUILTIN,LOW);
  // delay(1000);
  

  if (digitalRead(8)) digitalWrite(LED_BUILTIN, HIGH);
  else digitalWrite(LED_BUILTIN, LOW);

  // delay(1);

}
