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

void TemperatureSensor::writeFifo() {

    // Declaramos un arreglo de bytes de 4 bytes:
    uint8_t data_frame[5];

    // El primer byte es el tipo de sensor, 1 = temperatura, 2 = ph:
    data_frame[0] = this->sensorType;

    // El segundo byte es el signo de la temperatura, 0 = positivo, 1 = negativo:
    data_frame[1] = this->temperatureData < 0 ? 1 : 0;

    // Los tres últimos bytes son los valores máximos de la temperatura (-273 °C a 999 °C):
    int integerPart = std::abs(this->temperatureData); // Valor absoluto de la temperatura
    data_frame[2] = integerPart / 100;
    data_frame[3] = (integerPart % 100) / 10;
    data_frame[4] = integerPart % 10;

    // Escribimos la trama de datos en el FIFO:
    try{
        if (write(this->fifo_fd, data_frame, sizeof(data_frame)) == -1) {
            throw std::runtime_error("Error al escribir en el FIFO");
        }
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        exit(1);
    }
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
    // file.close();
}