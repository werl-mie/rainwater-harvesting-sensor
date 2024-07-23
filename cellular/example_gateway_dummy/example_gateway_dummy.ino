// For Adafruit M0 
// Dummy gateway that transmits replcate [DATA] strings over serial to validate parsing on the Arduino MKR NB 1500
char msg_valid[9] = {0,3,255,5,5,6,2,8,9}; // case A
char msg_short1[8] = {0,3,255,5,5,2,1,8}; // case B
char msg_long1[10] = {0,3,255,5,5,6,3,8,9,7}; // case C
char msg_long2[10] = {0,3,255,5,5,6,3,8,9,1}; // case D
char data_short1[11] = {0,3,255,6,5,5,4,4,5,6,7}; // case E (message invalid)
char header_short[3] = {3,3,4}; // F

char rxBuf[64];
char txBuf[64];

char terminator = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600);

  Serial.print("Hello World\r\n");

}

void loop() {

  if (Serial.available()) {
    int numBytes = Serial.readBytesUntil(terminator, rxBuf, 64) - 1;
    int txLen = 0;
    char* msg;

    for (int i = 0; i < numBytes; i ++){
      switch (rxBuf[i]){
        case 65: //A
          {
            txLen = 9;
            msg = msg_valid;
            break;
          }
        case 66: //B
        {
          txLen = 8;
          msg = msg_short1;
          break;
        }
        case 67: //C
        {
          txLen = 10;
          msg = msg_long1;
          break;
        }
        case 68: //D
        {
          txLen = 10;
          msg = msg_long2;
          break;
        }
        case 69: //E
        {
          txLen = 11;
          msg = data_short1;
          break;
        }
        case 70: //F
        {
          txLen = 3;
          msg = header_short;
          break;
        }
      }

      if (rxBuf[i] > 64 && rxBuf[i] < 71)
      {

        for (int j = 0; j < txLen; j ++){
          txBuf[j] = msg[j];
        }

        Serial.print("txLen: ");
        Serial.print(txLen);
        Serial.print(" ");

        Serial1.write(txBuf,txLen);

        Serial.print("Sent: ");
        for (int j = 0; j < txLen; j ++)
        {
          Serial.print(txBuf[j],HEX);
          Serial.print(" ");
        }
        Serial.print("\n\r");
      } else if (rxBuf[i] == 45){ //-
        delay(3500);
      } else {
        Serial.println("Invalid character");
      }

    }
  }

}

