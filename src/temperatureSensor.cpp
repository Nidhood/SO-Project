#include "temperatureSensor.h"

void TemperatureSensor::generateData() {

    // Creamos un generador de números aleatorios con la librería estándar de C++11:
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-10, 30);

    // Temperatura entre -10 °C y 30 °C:
    temperatureData = dis(gen);

    // Imprimir la temperatura en la consola:
    std::cout << "Temperatura: " << temperatureData  << " °C"<< std::endl;
}

void TemperatureSensor::sendData() {
    // Abrir el FIFO en modo escritura:
    openFifo();

    // Escribe la temperatura en el FIFO:
    writeFifo();

    // Cerrar el FIFO:
    closeFifo();
}

[[noreturn]] void TemperatureSensor::run() {

    // Crear el FIFO si no existe:
    createFifo();

    // Abrir el FIFO en modo escritura:
    openFifo();

    // Abrir el archivo de configuración (sobreescribir):
    std::ofstream file(this->fileName, std::ios::trunc);

    // Generar y enviar datos de temperatura cada cierto intervalo
    while(true) {

        // Generar datos de temperatura:
        generateData();

        // Enviar datos de temperatura:
        sendData();

        // Escribir la temperatura en el archivo de configuración:
        file << this->temperatureData << std::endl;
        file.flush();

        // Esperar en el tiempo de manejo:
        sleep(this->handlerTime);
    }

    // Cerrar el archivo de configuración:
    file.close();
}