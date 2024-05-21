#ifndef PROJECT_SM_SENSOR_H
#define PROJECT_SM_SENSOR_H

#include <iostream>
#include <random>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <iomanip>

class Sensor {
    // 0 = temperatura, 1 = ph
    std::string pipeName;
    int data{};
    mode_t mode{};

protected:
    int handlerTime{};
    std::string fileName;
    uint8_t sensorType{};
    int fifo_fd{};
public:
    Sensor() = default;
    Sensor(uint8_t sensorType, const std::string& fileName, const std::string& pipeName, int handlerTime) : sensorType(sensorType), fileName(std::move("files/" + fileName)) , pipeName(std::move("/tmp/" + pipeName)), handlerTime(handlerTime), mode(0666) {};
    void createFifo();
    void openFifo();
    virtual void writeFifo();
    void closeFifo() const;
    virtual void generateData();
    virtual void sendData();
    virtual void run();
    void stop() const;
};

#endif