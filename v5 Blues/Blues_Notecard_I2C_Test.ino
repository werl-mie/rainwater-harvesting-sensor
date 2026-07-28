#include <Wire.h>
#include <Notecard.h>

#define PRODUCT_UID "ca.utoronto.mail.a.diazlozanopatino:v6_boardtocell" //Change to product UID. No caps. 


Notecard notecard;


void setup() {

  Serial.begin(115200);

  while (!Serial);

  Wire.begin();

  notecard.begin();

  Serial.println("Initializing Notecard...");

  // Configure Notehub
  J *req = notecard.newRequest("hub.set");
  if (req != NULL) {
    JAddStringToObject(req, "product", PRODUCT_UID);
    JAddStringToObject(req, "mode", "continuous");
    JAddBoolToObject(req, "sync", true);

    if (!notecard.sendRequest(req)) {
      Serial.println("hub.set failed");
    } else {
      Serial.println("hub.set OK");
    }
  }

  delay(2000);

  Serial.println("Sending test note...");

  // Send a test message
  req = notecard.newRequest("note.add");

  if (req != NULL) {

    JAddStringToObject(req, "file", "test.qo");
    JAddBoolToObject(req, "sync", true);

    J *body = JAddObjectToObject(req, "body");

    if (body != NULL) {
      JAddStringToObject(body, "message", "Hello Notehub!");
      JAddNumberToObject(body, "value", 125); // change value here if needed
    }

    if (!notecard.sendRequest(req)) {
      Serial.println("note.add failed");
    } else {
      Serial.println("Test note queued.");
    }
  }

  // Force an immediate sync
  req = notecard.newRequest("hub.sync");
  if (req != NULL) {
    notecard.sendRequest(req);
    Serial.println("Sync requested.");
  }

  Serial.println("Finished.");
}

void loop() {
}