#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <ctime>
#include <cstdint>
#include <cmath>
#include <iostream>

void read_temperature(uint8_t data_frame[5], int *temperatureData);
void read_ph(uint8_t data_frame[5], float *phData);

int main(int argc, char *argv[]) {

    // Verificar que se haya ingresado la ruta después de /tmp:
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_despues_de_tmp>" << std::endl;
        return 1;
    }

    // Definimos el path de los FIFOs:
    std::string fifo_path = "/tmp/" + std::string(argv[1]);

    // Abrir los FIFOs en modo de solo lectura y no bloqueante:
    int fd_temperature = open(fifo_path.c_str(), O_RDONLY | O_NONBLOCK);

    // Verificar si se pudieron abrir los FIFOs:
    if (fd_temperature == -1) {
        perror("Error al abrir los FIFOs");
        return 1;
    }

    // Variables para almacenar los datos de los sensores:
    uint8_t data_frame[5];
    int temperatureData;
    float phData;

    // Variables para almacenar el tipo de sensor:
    uint8_t sensor_type;

    // Variables para obtener la hora actual:
    time_t current_time;
    char time_buffer[9]; // Suficiente para almacenar "HH:MM:SS\0"

    // Iniciamos el proceso de lectura de los datos de los FIFOs:
    while(1) {

        // Leer los datos de los FIFOs:
        read(fd_temperature, data_frame, sizeof(data_frame));
        sensor_type = data_frame[0];

        // Obtener la hora actual:
        time(&current_time);
        strftime(time_buffer, 9, "%T", localtime(&current_time));

        // Imprimir los datos en la consola:
        if(sensor_type == '1'){
            read_temperature(data_frame, &temperatureData);
            printf("Temperatura: %d °F\tHora de lectura: %s\n", temperatureData, time_buffer);
        } else if (sensor_type == '2'){
            read_ph(data_frame, &phData);
            printf("ph:          %.1f\tHora de lectura: %s\n", phData, time_buffer);
        }
        // Esperar 1 segundo:
        sleep(1);
    }

    return 0;
}

void read_temperature(uint8_t data_frame[5], int *temperatureData){

    // Los tres bytes que le siguen son los enteros valores máximos de la temperatura (-273 °C a 999 °C):
    *temperatureData = data_frame[2] * 100 + data_frame[3] * 10 + data_frame[4];

    // El segundo byte es el signo de la temperatura, 0 = positivo, 1 = negativo:
    if(data_frame[1] == 1){
        *temperatureData *= -1;
    }
}

void read_ph(uint8_t data_frame[5], float *phData){

    // Los dos bytes que le siguen son los enteros valores máximos del ph y el último byte es el decimal (0.0 a 14.9):
    *phData = std::ceil((data_frame[2] * 10 + data_frame[3] + (float)data_frame[4] / 10) * 10) / 10;

    // El segundo byte es el signo del ph, 0 = positivo, 1 = negativo:
    if(data_frame[1] == 1){
        *phData *= -1;
    }
}

