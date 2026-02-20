#include "GroveLora.h"

GroveLora::GroveLora(){}

void GroveLora::sendCmd(const char* cmd) {
    Serial1.print(cmd);
}

bool GroveLora::checkRX(SensorDatum_t* p_datum) {
    if (Serial1.available() > 0) {
        String raw = Serial1.readStringUntil('\n');
        
        // This MUST print if the LED flashes
        Serial.print("RADIO_RAW: "); 
        Serial.println(raw);

        if (raw.indexOf("+TEST: RX \"") != -1) {
            int start = raw.indexOf("\"") + 1;
            int end = raw.lastIndexOf("\"");
            String hex = raw.substring(start, end);
            
            unsigned int ts, sid, nid, st, d;
            if (sscanf(hex.c_str(), "%8x%4x%2x%2x%4x", &ts, &sid, &nid, &st, &d) >= 1) {
                p_datum->ts = ts; p_datum->data = d;
                return true;
            }
        }
    }
    return false;
}