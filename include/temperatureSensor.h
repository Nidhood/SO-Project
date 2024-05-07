#ifndef PROJECT_SM_TEMPERATURE_SENSOR_H
#define PROJECT_SM_TEMPERATURE_SENSOR_H

#include "sensor.h"

class TemperatureSensor : public Sensor {
    int temperatureData{};
public:
    TemperatureSensor(std::string sensorType, const std::string& fileName, const std::string& pipeName, int handlerTime) : Sensor(std::move(sensorType), fileName, pipeName, handlerTime) {};
    void generateData() override;
    void sendData() override;
    [[noreturn]]  void run() override;
};
#endif
