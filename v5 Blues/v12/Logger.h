#ifndef LOGGER_H
#define LOGGER_H

#include <SPI.h>
#include <SD.h>

#include "GlobalVars.h"
#include "Types.h"

class Logger{
  public:
    bool begin();

    bool writeToSD(const SensorDataBatch &data);

    //bool writeBucket(const RainfallSummary &data);

  private:
    const char* sensorToString(SensorID sensor);

    bool createFile(const char *filename, const char *header);

    bool appendLine(const char *filename, const char *line);

};

#endif