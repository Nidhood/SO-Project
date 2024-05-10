#ifndef PROJECT_SM_TEMPERATURE_SENSOR_H
#define PROJECT_SM_TEMPERATURE_SENSOR_H

#include "sensor.h"

class TemperatureSensor : public Sensor {
    int temperatureData{};
public:
    TemperatureSensor(uint8_t sensorType, const std::string& fileName, const std::string& pipeName, int handlerTime) : Sensor(sensorType, fileName, pipeName, handlerTime) {};
    void generateData() override;
    void writeFifo() override;
    void sendData() override;
    [[noreturn]] void run() override;
};

#endif
