/*
  DFRobot SEN0239 IR Proximity Sensor - Feather M0 Hardware Interrupt
*/

const int SENSOR_PIN = A0;      // Analog pin A0 configured as digital interrupt pin
const int LED_PIN = LED_BUILTIN; // Built-in Red LED (Pin 13)

// Volatile flag used to pass interrupt triggers to main loop safely
volatile bool obstacleDetected = false;

// Interrupt Service Routine (ISR)
void ISR_sensorTriggered() {
  obstacleDetected = true;
}

void setup() {
  Serial.begin(9600);

  // Set up pins
  pinMode(SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Native SAMD21 Hardware Interrupt setup
  // attachInterrupt requires converting the pin using digitalPinToInterrupt()
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), ISR_sensorTriggered, RISING);

  Serial.println("--- Feather M0 Hardware Interrupt Test Started ---");
}

void loop() {
  if (obstacleDetected) {
    // Reset flag
    obstacleDetected = false;

    // Output trigger notification
    digitalWrite(LED_PIN, HIGH);
    Serial.println("INTERRUPT: Obstacle detected on pin A0!");

    delay(200); 
    digitalWrite(LED_PIN, LOW);
  }

  // Rest of your main program (e.g., logging to SD card) can safely run here
}