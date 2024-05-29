#ifndef PROJECT_SM_TEMPERATURE_SENSOR_H
#define PROJECT_SM_TEMPERATURE_SENSOR_H

#include "sensor.h"

class TemperatureSensor : public Sensor {
    int temperatureData{};
    std::ifstream infile;
public:
    TemperatureSensor(uint8_t sensorType, const std::string& fileName, const std::string& pipeName, int handlerTime);
    void generateData() override;
    void writeFifo() override;
    void sendData() override;
    [[noreturn]] void run() override;
};

#endif
