#include "monitor.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <iomanip>

Monitor::Monitor(const std::string& temperatureFile, const std::string& phFile, const std::string& fifo, int bufferSize)
    : temperatureFileName(temperatureFile), phFileName(phFile), fifoName(fifo), bufferSize(bufferSize) {
    // Inicializar los semáforos
    sem_init(&temperatureSemaphore, 0, 1);
    sem_init(&phSemaphore, 0, 1);
    sem_init(&temperatureBufferSemaphore, 0, bufferSize);
    sem_init(&phBufferSemaphore, 0, bufferSize);
}

Monitor::~Monitor() {
    // Destruir los semáforos
    sem_destroy(&temperatureSemaphore);
    sem_destroy(&phSemaphore);
    sem_destroy(&temperatureBufferSemaphore);
    sem_destroy(&phBufferSemaphore);
}

void Monitor::run() {
    // Crear los hilos
    recolectorThread = std::thread(&Monitor::recolectorFunction, this);
    phThread = std::thread(&Monitor::phFunction, this);
    temperatureThread = std::thread(&Monitor::temperatureFunction, this);
}

void Monitor::join() {
    // Esperar a que los hilos terminen
    recolectorThread.join();
    phThread.join();
    temperatureThread.join();
}

void Monitor::recolectorFunction() {
    // Abrir el FIFO en modo lectura
    int fd = open(fifoName.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error al abrir el FIFO" << std::endl;
        return;
    }

    while (true) {
        // Leer los datos del FIFO
        char data[5];
        ssize_t bytesRead = read(fd, data, sizeof(data));
        if (bytesRead == -1) {
            std::cerr << "Error al leer del FIFO" << std::endl;
            break;
        }
        if (bytesRead == 0) {
            // No hay más datos, el FIFO se cerró
            break;
        }

        // Determinar el tipo de sensor
        char sensorType = data[0];
        if (sensorType == '1') {
            // Temperatura
            int temperature = (data[2] - '0') * 100 + (data[3] - '0') * 10 + (data[4] - '0');
            if (data[1] == '1') {
                temperature *= -1;
            }
            if (temperature < 0) {
                std::cerr << "Temperatura errónea descartada: " << temperature << std::endl;
                continue;
            }
            sem_wait(&temperatureBufferSemaphore);
            sem_wait(&temperatureSemaphore);
            temperatureBuffer.push_back(temperature);
            sem_post(&temperatureSemaphore);
        } else if (sensorType == '2') {
            // pH
            double ph = (data[2] - '0') * 10 + (data[3] - '0') + (data[4] - '0') / 10.0;
            if (data[1] == '1') {
                ph *= -1;
            }
            if (ph < 0) {
                std::cerr << "pH erróneo descartado: " << ph << std::endl;
                continue;
            }
            sem_wait(&phBufferSemaphore);
            sem_wait(&phSemaphore);
            phBuffer.push_back(ph);
            sem_post(&phSemaphore);
        }
    }

    // Cerrar el FIFO
    close(fd);
}

void Monitor::phFunction() {
    // Abrir el archivo de pH en modo escritura
    std::ofstream phFile(phFileName);
    if (!phFile) {
        std::cerr << "Error al abrir el archivo de pH" << std::endl;
        return;
    }

    while (true) {
        // Esperar a que haya mediciones disponibles en el búfer de pH
        sem_wait(&phSemaphore);
        if (phBuffer.empty()) {
            sem_post(&phSemaphore);
            break;
        }
        double ph = phBuffer.front();
        phBuffer.erase(phBuffer.begin());
        sem_post(&phSemaphore);
        sem_post(&phBufferSemaphore);

        // Obtener la hora actual
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);

        // Escribir la medición de pH en el archivo con la hora actual
        phFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - pH: " << ph << std::endl;

        // Generar alerta si el pH está fuera de rango
        if (ph < 6.0 || ph > 8.0) {
            std::cout << "Alerta: pH fuera de rango - " << ph << std::endl;
        }
    }

    // Cerrar el archivo de pH
    phFile.close();
}

void Monitor::temperatureFunction() {
    // Abrir el archivo de temperatura en modo escritura
    std::ofstream temperatureFile(temperatureFileName);
    if (!temperatureFile) {
        std::cerr << "Error al abrir el archivo de temperatura" << std::endl;
        return;
    }

    while (true) {
        // Esperar a que haya mediciones disponibles en el búfer de temperatura
        sem_wait(&temperatureSemaphore);
        if (temperatureBuffer.empty()) {
            sem_post(&temperatureSemaphore);
            break;
        }
        int temperature = temperatureBuffer.front();
        temperatureBuffer.erase(temperatureBuffer.begin());
        sem_post(&temperatureSemaphore);
        sem_post(&temperatureBufferSemaphore);

        // Obtener la hora actual
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);

        // Escribir la medición de temperatura en el archivo con la hora actual
        temperatureFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - Temperatura: " << temperature << " °C" << std::endl;

        // Generar alerta si la temperatura está fuera de rango
        if (temperature < 68 || temperature > 89) {
            std::cout << "Alerta: Temperatura fuera de rango - " << temperature << " °C" << std::endl;
        }
    }

    // Cerrar el archivo de temperatura
    temperatureFile.close();
}