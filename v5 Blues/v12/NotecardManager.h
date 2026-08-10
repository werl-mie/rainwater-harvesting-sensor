#ifndef NOTECARD_MANAGER_H
#define NOTECARD_MANAGER_H

#include <Notecard.h>
#include "GlobalVars.h"
#include "Types.h"

class NotecardManager{

  public:

    //Initialize notecard communication 
    bool begin();

    //Sync to notehub
    bool sync();

    //Add sensor data
    bool addSensorData(const SensorDataBatch &data);

    float getBatteryVoltageFromNotecard();

    bool isBusy();

  private:
    //Notecard interface init
    Notecard notecard;

    //JSON configuration 
    bool configureHub();
    bool configureSensorTemplate();

    //Transmit data 
    //bool sendDataBatch(); 

};

#endif