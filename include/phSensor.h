
#ifndef PROJECT_SM_PH_SENSOR_H
#define PROJECT_SM_PH_SENSOR_H

#include "sensor.h"

class PhSensor : public Sensor {
    int phData{};
public:
    PhSensor(std::string sensorType, const std::string& fileName, const std::string& pipeName, int handlerTime) : Sensor(std::move(sensorType), fileName, pipeName, handlerTime) {};
    void generateData() override;
    void sendData() override;
    [[noreturn]] void run() override;
};

#endif
