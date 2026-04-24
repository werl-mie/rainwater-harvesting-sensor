/*
  Digital Read for Adafruit RP2040
  Reads a digital input on pin 9 and prints the result to the Serial Monitor.

NEXT STEPS: 
1) RTC - resolder pin 5 to RTC SQW pin
2) N1 AND N2 
*/

const int inputPin = 9; // Define the pin number

void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
  // Configure pin 9 as an INPUT
  // Note: Use INPUT_PULLUP if you are using a simple button to ground
  pinMode(inputPin, INPUT_PULLUP);
}

void loop() {
  // Read the input pin:
  int pinValue = digitalRead(inputPin);
  
  // Print out the value (0 for LOW, 1 for HIGH):
  Serial.print("Digital Value on Pin 9: ");
  Serial.println(pinValue);
  
  // Delay a bit between reads for stability
  delay(100);
}
