#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

#include "GlobalVars.h"
#include "Types.h"
#include "RTCManager.h"
#include "Logger.h"
#include "NotecardManager.h"
#include "RTCManager.h"

extern RTCManager rtcManager;
extern NotecardManager notecardManager;
extern Logger logger;

class InterruptManager{
  public:
    void processEventAndLog(bool rtcState);
     
    void powerOn();

    void powerOff();
};


#endif