#include "phSensor.h"

void PhSensor::generateData() {

    // Creamos un generador de números aleatorios con la librería estándar de C++11:
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 14);

    // ph entre 0 y 14:
    phData = dis(gen);

    // Imprimir el ph en la consola:
    std::cout << "Ph: " << phData << std::endl;
}

void PhSensor::sendData() {

    // Abrir el FIFO en modo escritura:
    openFifo();

    // Escribe el ph en el FIFO:
    writeFifo();

    // Cerrar el FIFO:
    closeFifo();
}

[[noreturn]] void PhSensor::run() {

    // Crear el FIFO si no existe:
    createFifo();

    // Abrir el FIFO en modo escritura:
    openFifo();

    // Abrir el archivo de configuración (sobreescribir):
    std::ofstream file(this->fileName, std::ios::trunc);

    // Generar y enviar datos de ph cada cierto intervalo:
    while(true) {

        // Generar datos de ph:
        generateData();

        // Enviar datos de ph:
        sendData();

        // Escribir el ph en el archivo de configuración:
        file << this->phData << std::endl;
        file.flush();

        // Esperar en el tiempo de manejo:
        sleep(this->handlerTime);
    }

    // Cerrar el archivo de configuración:
    file.close();
}