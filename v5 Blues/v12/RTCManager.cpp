#include "RTCManager.h"
#include "GlobalVars.h"

// RTC Related functions (as defined in class in .h file)

// --------------- RTC Initialization ------------

bool RTCManager::begin(){

  Wire.begin();

  //RTCZero
  rtcZ.begin();
  rtcZ.setTime(12, 0, 0);
  rtcZ.setDate(10, 7, 26); //Replace by date. Later, should sync with notecard datetime?

  //PCF8523
  if (!rtcTimer.begin()){
    if (DEBUG_MODE){
      Serial.println(F("ERROR: PCF8523 init failed"));
    }
    return false;
  }

  rtcTimer.deconfigureAllTimers();

  // Arm the countdown ONCE at boot. Timer B auto-reloads and repeats on its
  // own in hardware after this - do NOT call setCountdown() again on every
  // wake, that reconfigure was what was causing spurious retriggers.
  if (!setCountdown()){
    if (DEBUG_MODE){
      Serial.println(F("RTC Timer failed"));
    }
  }

  if (DEBUG_MODE){
    Serial.println(F("RTC Manager init"));
    debugTimerBRegs();
    debugClkOutControl();
  }

  return true;

}

// --------------- RTC Countdown timer ------------

bool RTCManager::setCountdown(){

  rtcTimer.deconfigureAllTimers();
  delay(10);

  rtcTimer.writeSqwPinMode(PCF8523_OFF);
  delay(100);

  //Re-enable timer
  //Kept hardcoded at 35s intentionally for testing - swap to
  //RTC_COUNTDOWN_FREQ / RTC_COUNTDOWN_VALUE (GlobalVars.h) once the wake
  //bug is confirmed fixed, since those currently resolve to 10s in DEBUG_MODE.
  rtcTimer.enableCountdownTimer(PCF8523_FrequencySecond, 35);
  delay(10);


  if (DEBUG_MODE){
    Serial.println(F("RTC Timer set"));
    printControl2();
  }

  return true;
}


// --------------- RTC get time ------------

uint32_t RTCManager::getEpoch(){
  return rtcZ.getEpoch();
}

// --------------- Clear RTC interrrupt flag ------------
void RTCManager::clearPCF8523InterruptFlag(){
  //Force timeout to prevent clash with notecard i2c
  uint32_t timeout = millis();
  while ((SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSERR) && (millis() - timeout < 500)){
    delay(1);
  }

  Wire.beginTransmission(0x68);
  Wire.write(0x01); // Control_2
  uint8_t err1 = Wire.endTransmission(false);
  if (err1 != 0 && DEBUG_MODE){
    Serial.print(F("I2C error selecting Control_2: "));
    Serial.println(err1);
  }

  Wire.requestFrom(0x68, (uint8_t)1);
  if (Wire.available()){
    uint8_t ctrl2 = Wire.read();
    ctrl2 &= ~(1 << 5); // Clear CTBF (Countdown Timer B flag) - Timer B is what's actually in use
    Wire.beginTransmission(0x68);
    Wire.write(0x01);
    Wire.write(ctrl2);
    uint8_t err2 = Wire.endTransmission();
    if (err2 != 0 && DEBUG_MODE){
      Serial.print(F("I2C error writing Control_2: "));
      Serial.println(err2);
    }
  } else if (DEBUG_MODE){
    Serial.println(F("No data available reading Control_2"));
  }

  delay(5);
}

// --------------- RTC reset timer ------------

bool RTCManager::resetTimer(){

  // Timer B auto-reloads on its own in hardware - do NOT call setCountdown()
  // here. Just acknowledge/clear flags so a fresh edge is recognized next time.
  clearPCF8523InterruptFlag();
  clearMCUInterruptFlag();

  if (DEBUG_MODE){
    Serial.println(F("MCU RTC Flag cleared"));
    printControl2();
  }

  return true;

}

// --------------- RTC print ctrl 2 ------------

void RTCManager::printControl2(){

    Wire.beginTransmission(0x68);
    Wire.write(0x01);

    if (Wire.endTransmission(false) != 0){
        Serial.println(F("ERROR: Could not access PCF8523 Control_2"));
        return;
    }

    Wire.requestFrom(0x68, (uint8_t)1);

    if (Wire.available()){

        uint8_t ctrl2 = Wire.read();

        Serial.print(F("PCF8523 Control_2 = 0b"));

        for (int8_t i = 7; i >= 0; i--){
            Serial.print((ctrl2 >> i) & 0x01);
        }

        Serial.print(F(" (0x"));
        Serial.print(ctrl2, HEX);
        Serial.println(F(")"));
    }
}

// --------------- RTC INT Pin debug ------------

void RTCManager::debugRTCIntPin(){
    Serial.print(F("RTC INT = "));
    Serial.println(digitalRead(PIN_RTC_INT));
}

// --------------- RTC clear MCU INT Flag ------------

void RTCManager::clearMCUInterruptFlag(){
      EIC->INTFLAG.reg = EIC_INTFLAG_EXTINT4;   // PA20 / D6 -> EXTINT line 4, not raw pin 20
}

// --------------- RTC Timer B register debug ------------

void RTCManager::debugTimerBRegs(){
  Wire.beginTransmission(0x68);
  Wire.write(0x12); // Timer_B_FRCTL
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, (uint8_t)2); // reads 0x12 and 0x13 (FRCTL then VALUE)

  if (Wire.available() >= 2){
    uint8_t frctl = Wire.read();
    uint8_t value = Wire.read();
    Serial.print(F("Timer_B_FRCTL = 0x"));
    Serial.print(frctl, HEX);
    Serial.print(F(" (clkFreq bits = "));
    Serial.print(frctl & 0x07);
    Serial.println(F(")"));
    Serial.print(F("Timer_B_VALUE = "));
    Serial.println(value);
  }
}

// --------------- RTC CLKOUT_ctrl debug ------------
// Bit 0 (TBC) confirms Timer B is actually enabled/counting, independent of
// whether Control_2's CTBIE (interrupt enable) bit looks fine.

void RTCManager::debugClkOutControl(){
  Wire.beginTransmission(0x68);
  Wire.write(0x0F); // CLKOUT_ctrl
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, (uint8_t)1);

  if (Wire.available()){
    uint8_t reg = Wire.read();
    Serial.print(F("CLKOUT_ctrl = 0b"));
    for (int8_t i = 7; i >= 0; i--){
      Serial.print((reg >> i) & 0x01);
    }
    Serial.print(F(" (TBC="));
    Serial.print(reg & 0x01);
    Serial.println(F(")"));
  }
}
