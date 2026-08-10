#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <Wire.h>
#include <RTCZero.h>
#include <RTClib.h>

#include "GlobalVars.h"

class RTCManager{

  public: //Can be accessed by other files

  //Initialize RTCs
  bool begin();

  //Configure timer
  bool setCountdown();

  //Get current timestamp
  uint32_t getEpoch();

  //Get current timestamp
  bool resetTimer();

  //Debuug: print control
  void printControl2();

  void debugRTCIntPin();

  void clearMCUInterruptFlag();

  //Debug: read back Timer_B_FRCTL (0x12) and Timer_B_VALUE (0x13)
  void debugTimerBRegs();

  //Debug: read back CLKOUT_ctrl (0x0F) - bit0 (TBC) confirms Timer B is actually counting
  void debugClkOutControl();

  private:

  //Internal RTC
  RTCZero rtcZ;

  //External RTC: PCF8523
  RTC_PCF8523 rtcTimer;

  //Clearing PCF8523 register
  void clearPCF8523InterruptFlag();
};

#endif
