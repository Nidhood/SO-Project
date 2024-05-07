#include "sensor.h"

// Método para crear el FIFO:
void Sensor::createFifo() {
    try{
        if(mkfifo(this->pipeName.c_str(), this->mode) == -1){
            if(errno == EEXIST){
                std::cout << "El FIFO ya existe" << std::endl;
            } else {
                throw std::runtime_error("Error al crear el FIFO");
            }
        }
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        exit(1);
    }

}

// Método para abrir el FIFO:
void Sensor::openFifo() {
    try {
        this->fifo_fd = open(this->pipeName.c_str(), O_WRONLY);
        if (this->fifo_fd == -1) {
            throw std::runtime_error("Error al abrir el FIFO");
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

// Método para escribir en el FIFO:
void Sensor::writeFifo() {
    try{
        if (write(this->fifo_fd, &this->data, sizeof(this->data)) == -1) {
            throw std::runtime_error("Error al escribir en el FIFO");
        }
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

// Método para cerrar el FIFO:
void Sensor::closeFifo() const {
    close(this->fifo_fd);
}

// Método para generar datos:
void Sensor::generateData() {
    std::cout << "Generating data..." << std::endl;
}

// Método para enviar datos:
void Sensor::sendData() {
    std::cout << "Sending data from Sensor..." << std::endl;
}

// Método para ejecutar el sensor:
void Sensor::run() {
    std::cout << "Running Sensor..." << std::endl;
}

// Método para detener el sensor:
void Sensor::stop() const {
    this->closeFifo();
}