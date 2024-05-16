#ifndef MONITOR_H
#define MONITOR_H

#include <vector>
#include <thread>
#include <semaphore.h>
#include <string>

class Monitor {
private:
    std::vector<int> temperatureBuffer;
    std::vector<double> phBuffer;
    sem_t temperatureSemaphore;
    sem_t phSemaphore;
    sem_t temperatureBufferSemaphore;
    sem_t phBufferSemaphore;
    std::string temperatureFileName;
    std::string phFileName;
    std::string fifoName;
    int bufferSize;
    std::thread recolectorThread;
    std::thread phThread;
    std::thread temperatureThread;

public:
    Monitor(const std::string& temperatureFile, const std::string& phFile, const std::string& fifo, int bufferSize);
    ~Monitor();

    void run();
    void join();

private:
    void recolectorFunction();
    void phFunction();
    void temperatureFunction();
};

#endif