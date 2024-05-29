#include "temperatureSensor.h"

TemperatureSensor::TemperatureSensor(uint8_t sensorType, const std::string& fileName, const std::string& pipeName, int handlerTime)
        : Sensor(sensorType, fileName, pipeName, handlerTime) {
    infile.open(this->fileName);
    if (!infile) {
        std::cerr << "Error al abrir el archivo: '" << this->fileName << "'." << std::endl;
        exit(1);
    }
}


void TemperatureSensor::generateData() {
    // Verificar si se ha llegado al final del archivo
    if (infile.eof()) {
        // Volver al principio del archivo para leer desde el inicio
        infile.clear();
        infile.seekg(0, std::ios::beg);
    }

    // Leer el siguiente valor de temperatura del archivo
    if (!(infile >> this->temperatureData)) {
        // Si se alcanzó el final del archivo, indicarlo y salir
        if (infile.eof()) {
            std::cerr << "Se ha llegado al final del archivo: '" << this->fileName << "'." << std::endl;
            exit(1);
        } else {
            std::cerr << "Error al leer el archivo de temperatura: '" << this->fileName << "'." << std::endl;
            exit(1);
        }
    }

    // Si se muestran valores negativos, se marca como error:
    if (this->temperatureData < 0) {
        std::cerr << "[Error]\t\tTemperatura negativa:\t\t" << this->temperatureData << " °F" << std::endl;
    }
        // Comprobar si la temperatura está fuera del rango permitido y alertar (68 F° a 89 °F):
    else if (this->temperatureData < 68 || this->temperatureData > 89) {
        std::cerr << "[Alerta]\tTemperatura fuera de rango:\t" << this->temperatureData << " °F" << std::endl;
    }
        // Si se muestran valores positivos, se marca como correcto:
    else {
        std::cout << "[Correcto]\tTemperatura dentro del rango:\t" << this->temperatureData << " °F" << std::endl;
    }
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
    try {
        if (write(this->fifo_fd, data_frame, sizeof(data_frame)) == -1) {
            throw std::runtime_error("Error al escribir en el FIFO");
        }
    } catch (const std::runtime_error& e) {
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

    // Generar y enviar datos de temperatura cada cierto intervalo
    while (true) {
        // Generar datos de temperatura:
        generateData();

        // Enviar datos de temperatura:
        sendData();

        // Esperar en el tiempo de manejo:
        sleep(this->handlerTime);
    }
}
