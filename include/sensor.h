#ifndef PROJECT_SM_SENSOR_H
#define PROJECT_SM_SENSOR_H

#include <iostream>
#include <random>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

class Sensor {
    std::string sensorType;
    std::string pipeName;
    int fifo_fd{};
    int data{};
    mode_t mode{};

protected:
    int handlerTime{};
    std::string fileName;
public:
    Sensor() = default;
    Sensor(std::string sensorType, const std::string& fileName, const std::string& pipeName, int handlerTime) : sensorType(std::move(sensorType)), fileName(std::move("../files/" + fileName)) , pipeName(std::move("/tmp/" + pipeName)), handlerTime(handlerTime), mode(0666) {};
    void createFifo();
    void openFifo();
    void writeFifo();
    void closeFifo() const;
    virtual void generateData();
    virtual void sendData();
    virtual void run();
    void stop() const;
};

#endif