#include "monitor.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <iomanip>

Monitor::Monitor(const std::string& temperatureFile, const std::string& phFile, const std::string& fifo, int bufferSize)
    : temperatureFileName(temperatureFile), phFileName(phFile), fifoName(fifo), bufferSize(bufferSize) {
    temperatureSemaphore = sem_open("/temperatureSemaphore", O_CREAT, 0644, 1);
    phSemaphore = sem_open("/phSemaphore", O_CREAT, 0644, 1);
    temperatureBufferSemaphore = sem_open("/temperatureBufferSemaphore", O_CREAT, 0644, bufferSize);
    phBufferSemaphore = sem_open("/phBufferSemaphore", O_CREAT, 0644, bufferSize);
}

Monitor::~Monitor() {
    sem_close(temperatureSemaphore);
    sem_unlink("/temperatureSemaphore");
    sem_close(phSemaphore);
    sem_unlink("/phSemaphore");
    sem_close(temperatureBufferSemaphore);
    sem_unlink("/temperatureBufferSemaphore");
    sem_close(phBufferSemaphore);
    sem_unlink("/phBufferSemaphore");
}

void Monitor::run() {
    recolectorThread = std::thread(&Monitor::recolectorFunction, this);
    phThread = std::thread(&Monitor::phFunction, this);
    temperatureThread = std::thread(&Monitor::temperatureFunction, this);
}

void Monitor::join() {
    recolectorThread.join();
    phThread.join();
    temperatureThread.join();
}

void Monitor::recolectorFunction() {
    int fd = open(fifoName.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error al abrir el FIFO" << std::endl;
        return;
    }
    while (true) {
        char data[5];
        ssize_t bytesRead = read(fd, data, sizeof(data));
        if (bytesRead == -1) {
            std::cerr << "Error al leer del FIFO" << std::endl;
            break;
        }
        if (bytesRead == 0) {
            break;
        }
        char sensorType = data[0];
        if (sensorType == '1') {
            int temperature = (data[2] - '0') * 100 + (data[3] - '0') * 10 + (data[4] - '0');
            if (data[1] == '1') {
                temperature *= -1;
            }
            sem_wait(temperatureBufferSemaphore);
            sem_wait(temperatureSemaphore);
            temperatureBuffer.push_back(temperature);
            sem_post(temperatureSemaphore);
            sem_post(temperatureBufferSemaphore);
        } else if (sensorType == '2') {
            double ph = (data[2] - '0') * 10 + (data[3] - '0') + (data[4] - '0') / 10.0;
            if (data[1] == '1') {
                ph *= -1;
            }
            sem_wait(phBufferSemaphore);
            sem_wait(phSemaphore);
            phBuffer.push_back(ph);
            sem_post(phSemaphore);
            sem_post(phBufferSemaphore);
        }
    }
    close(fd);
}

void Monitor::phFunction() {
    std::ofstream phFile(phFileName);
    if (!phFile) {
        std::cerr << "Error al abrir el archivo de pH" << std::endl;
        return;
    }
    while (true) {
        sem_wait(phSemaphore);
        if (phBuffer.empty()) {
            sem_post(phSemaphore);
            break;
        }
        double ph = phBuffer.front();
        phBuffer.erase(phBuffer.begin());
        sem_post(phSemaphore);
        sem_post(phBufferSemaphore);
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        phFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - pH: " << ph << std::endl;
        if (ph < 6.0 or ph > 8.0) {
            std::cout << "Alerta: pH fuera de rango - " << ph << std::endl;
        }
    }
    phFile.close();
}

void Monitor::temperatureFunction() {
    std::ofstream temperatureFile(temperatureFileName);
    if (!temperatureFile) {
        std::cerr << "Error al abrir el archivo de temperatura" << std::endl;
        return;
    }
    while (true) {
        sem_wait(temperatureSemaphore);
        if (temperatureBuffer.empty()) {
            sem_post(temperatureSemaphore);
            break;
        }
        int temperature = temperatureBuffer.front();
        temperatureBuffer.erase(temperatureBuffer.begin());
        sem_post(temperatureSemaphore);
        sem_post(temperatureBufferSemaphore);
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        temperatureFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - Temperatura: " << temperature << " °C" << std::endl;
        if (temperature < 68 or temperature > 89) {
            std::cout << "Alerta: Temperatura fuera de rango - " << temperature << " °C" << std::endl;
        }
    }
    temperatureFile.close();
}
