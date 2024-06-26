#include "phSensor.h"

void PhSensor::generateData() {

    // Creamos un generador de números aleatorios con la librería estándar de C++11:
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 14.0);
    std::uniform_real_distribution<> prob(0.0, 1.0); // Distribución para probabilidad

    // pH entre 0 y 14 (con un solo decimal):
    this->phData = dis(gen);

    // Determinar si se produce un error (20% de probabilidad)
    if (prob(gen) <= 0.2) {
        this->phData = -dis(gen); // pH negativo
    }

    // Si se muestran valores negativos, se marca como error:
    if (this->phData < 0) {
        std::cerr << "[Error]\t\tpH negativo:\t\t" << std::fixed << std::setprecision(1) << this->phData << std::endl;
    }

    // Comprobar si el pH está fuera del rango permitido y alertar:
    else if (this->phData < 6.0 || this->phData > 8.0) {
        std::cerr << "[Alerta]\tpH fuera de rango:\t" << std::fixed << std::setprecision(1) << this->phData << std::endl;
    }

    // Si se muestran valores positivos, se marca como correcto:
    else {
        std::cout << "[Correcto]\tpH dentro del rango:\t" << std::fixed << std::setprecision(1) << this->phData << std::endl;
    }
}


// Método para escribir en el FIFO:
void PhSensor::writeFifo() {

    // Declaramos un arreglo de bytes de 4 bytes:
    uint8_t data_frame[5];

    // El primer byte es el tipo de sensor, 1 = temperatura, 2 = ph:
    data_frame[0] = this->sensorType;

    // El segundo byte es el signo de la temperatura, 0 = positivo, 1 = negativo:
    data_frame[1] = this->phData < 0 ? 1 : 0;

    // Los dos bytes que le siguen son los enteros valores máximos del ph y el último byte es el decimal (0.0 a 14.9):
    double absPhData = std::abs(this->phData); // Valor absoluto del pH
    int integerPart = static_cast<int>(absPhData); // Parte entera del pH
    int decimalPart = static_cast<int>((absPhData - integerPart) * 10); // Parte decimal del pH

    data_frame[2] = integerPart / 10;
    data_frame[3] = integerPart % 10;
    data_frame[4] = decimalPart;

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

    // Comprobar si el archivo se abrió correctamente:
    if (!file) {
        std::cerr << "Error al abrir el archivo: '" << this->fileName << "'." << std::endl;
        exit(1);
    }

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
    // file.close();
}