#include <iostream>
#include "stringValidations.h"
#include "temperatureSensor.h"
#include "phSensor.h"

#define TEMPERATURE_OPTION "1"
#define PH_OPTION "2"

int main( int argc, char* argv[] ) {

    // Declaramos variables para almacenar los argumentos recibidos:
    uint8_t sensorType;        // 1 = temperatura, 2 = ph
    std::string fileName;
    std::string pipeName;
    int handlerTime = 0;

    // Declaramos un objeto de la clase Sensor:
    Sensor *sensor{};

    // Validar que existen los 8 argumentos (-s tipo-sensor -t tiempo -f archivo -p pipe-nominal):
    if (argc != 9) {

        // Mostrar el uso correcto del programa:
        std::cout << "Uso: ./sensor -s tipo-sensor -t tiempo -f archivo -p pipe-nominal\n"
               "Donde:\n"
               "tipo-sensor: puede ser '0 = temperatura' o '1 = ph'.\n"
               "tiempo: tiempo en segundos entre cada lectura.\n"
               "archivo: nombre del archivo con las medidas de temperatura o ph.\n"
               "pipe-nominal: nombre del pipe nominal.\n" << std::endl;

        // Error genérico.
        perror("Error: la cantidad de argumentos es incorrecta.\n");
        exit(1);
    }

    // Si la cantidad de argumentos es correcta, entonces verificamos que los argumentos sean válidos.
    else {

        // 1. Debe existir únicamente una opción de cada tipo.
        if (check_duplicate_option("-s", argc, argv)) {
            perror("Error: solo puede existir una opción de tipo '-s'");
            exit(1);
        }
        if (check_duplicate_option("-t", argc, argv)) {
            perror("Error: solo puede existir una opción de tipo '-t'");
            exit(1);
        }
        if (check_duplicate_option("-f", argc, argv)) {
            perror("Error: solo puede existir una opción de tipo '-f'");
            exit(1);
        }
        if (check_duplicate_option("-p", argc, argv)) {
            perror("Error: solo puede existir una opción de tipo '-p'");
            exit(1);
        }

        // 2. Solo puede existir una opción de cada tipo.
        if (equal_strings(argv[1], "-s") && equal_strings(argv[3], "-s") && equal_strings(argv[5], "-s") &&
            equal_strings(argv[7], "-s")) {
            perror("Error: solo puede existir una opción de tipo '-s'");
            exit(1);
        } else if (equal_strings(argv[1], "-t") && equal_strings(argv[3], "-t") && equal_strings(argv[5], "-t") &&
                   equal_strings(argv[7], "-t")) {
            perror("Error: solo puede existir una opción de tipo '-t'");
            exit(1);
        } else if (equal_strings(argv[1], "-f") && equal_strings(argv[3], "-f") && equal_strings(argv[5], "-f") &&
                   equal_strings(argv[7], "-f")) {
            perror("Error: solo puede existir una opción de tipo '-f'");
            exit(1);
        } else if (equal_strings(argv[1], "-p") && equal_strings(argv[3], "-p") && equal_strings(argv[5], "-p") &&
                   equal_strings(argv[7], "-p")) {
            perror("Error: solo puede existir una opción de tipo '-p'");
            exit(1);
        }

        // 3. El tipo de sensor debe ser "1=temperatura" o "2=ph".
        if (equal_strings(argv[1], "-s") && ((!equal_strings(argv[2], TEMPERATURE_OPTION) && !equal_strings(argv[2], PH_OPTION)))) {
            perror("Error en '-s': el tipo de sensor no es válido");
            exit(1);
        } else if (equal_strings(argv[3], "-s") &&
                  (!equal_strings(argv[4], TEMPERATURE_OPTION) && !equal_strings(argv[4], PH_OPTION))) {
            perror("Error en '-s': el tipo de sensor no es válido");
            exit(1);
        } else if (equal_strings(argv[5], "-s") &&
                  (!equal_strings(argv[6], TEMPERATURE_OPTION) && !equal_strings(argv[6], PH_OPTION))) {
            perror("Error en '-s': el tipo de sensor no es válido");
            exit(1);
        } else if (equal_strings(argv[7], "-s") &&
                   (!equal_strings(argv[8], TEMPERATURE_OPTION) && !equal_strings(argv[8], PH_OPTION))) {
            perror("Error en '-s': el tipo de sensor no es válido");
            exit(1);
        }

        // 4. El tiempo debe ser un número entero positivo.
        if (equal_strings(argv[1], "-t") && std::stoi(argv[2]) <= 0) {
            perror("Error en '-t': el tiempo debe ser un número entero positivo");
            exit(1);
        } else if (equal_strings(argv[3], "-t") && std::stoi(argv[4]) <= 0) {
            perror("Error en '-t': el tiempo debe ser un número entero positivo");
            exit(1);
        } else if (equal_strings(argv[5], "-t") && std::stoi(argv[6]) <= 0) {
            perror("Error en '-t': el tiempo debe ser un número entero positivo");
            exit(1);
        } else if (equal_strings(argv[7], "-t") && std::stoi(argv[8]) <= 0) {
            perror("Error en '-t': el tiempo debe ser un número entero positivo");
            exit(1);
        }

        // 5. El nombre del archivo debe ser una cadena válida para el formato de un archivo.
        if (equal_strings(argv[1], "-f") && !is_valid_file(argv[2])) {
            perror("Error en '-f': el nombre del archivo no es válido");
            exit(1);
        } else if (equal_strings(argv[3], "-f") && !is_valid_file(argv[4])) {
            perror("Error en '-f': el nombre del archivo no es válido");
            exit(1);
        } else if (equal_strings(argv[5], "-f") && !is_valid_file(argv[6])) {
            perror("Error en '-f': el nombre del archivo no es válido");
            exit(1);
        } else if (equal_strings(argv[7], "-f") && !is_valid_file(argv[8])) {
            perror("Error en '-f': el nombre del archivo no es válido");
            exit(1);
        }

        // 6. El nombre del pipe nominal debe ser una cadena válida para el formato de un pipe nominal.
        if (equal_strings(argv[1], "-p") && !is_valid_pipe(argv[2])) {
            perror("Error en '-p': el nombre del pipe nominal no es válido");
            exit(1);
        } else if (equal_strings(argv[3], "-p") && !is_valid_pipe(argv[4])) {
            perror("Error en '-p': el nombre del pipe nominal no es válido");
            exit(1);
        } else if (equal_strings(argv[5], "-p") && !is_valid_pipe(argv[6])) {
            perror("Error en '-p': el nombre del pipe nominal no es válido");
            exit(1);
        } else if (equal_strings(argv[7], "-p") && !is_valid_pipe(argv[8])) {
            perror("Error en '-p': el nombre del pipe nominal no es válido");
            exit(1);
        }

        // 7. las banderas deben ser válidas:
        if (!is_valid_flag("-s", argc, argv)) {
            perror("Error: la bandera '-s' no es válida.\n");
            exit(-1);
        } else if (!is_valid_flag("-t", argc, argv)) {
            perror("Error: la bandera '-t' no es válida.\n");
            exit(-1);
        } else if (!is_valid_flag("-f", argc, argv)) {
            perror("Error: la bandera '-f' no es válida.\n");
            exit(-1);
        } else if (!is_valid_flag("-p", argc, argv)) {
            perror("Error: la bandera '-p' no es válida.\n");
            exit(-1);
        }
    }

    // Obtener los argumentos recibidos:

    //1. Tipo de sensor:
    if (equal_strings(argv[2], TEMPERATURE_OPTION) || equal_strings(argv[4], TEMPERATURE_OPTION) ||
        equal_strings(argv[6], TEMPERATURE_OPTION) || equal_strings(argv[8], TEMPERATURE_OPTION)) {
        sensorType = '1';
    } else {
        sensorType = '2';
    }

    //2. Tiempo entre cada lectura:
    if (equal_strings(argv[1], "-t")) {
        handlerTime = std::stoi(argv[2]);
    } else if (equal_strings(argv[3], "-t")) {
        handlerTime = std::stoi(argv[4]);
    } else if (equal_strings(argv[5], "-t")) {
        handlerTime = std::stoi(argv[6]);
    } else if (equal_strings(argv[7], "-t")) {
        handlerTime = std::stoi(argv[8]);
    }

    // 3. Nombre del archivo:
    if (equal_strings(argv[1], "-f")) {
        fileName = argv[2];
    } else if (equal_strings(argv[3], "-f")) {
        fileName = argv[4];
    } else if (equal_strings(argv[5], "-f")) {
        fileName = argv[6];
    } else if (equal_strings(argv[7], "-f")) {
        fileName = argv[8];
    }

    // 3.3 Asegurar que el archivo tenga la extensión .txt:
    fileName = ensure_txt_extension(fileName);

    // 4. Nombre del pipe nominal:
    if (equal_strings(argv[1], "-p")) {
        pipeName = argv[2];
    } else if (equal_strings(argv[3], "-p")) {
        pipeName = argv[4];
    } else if (equal_strings(argv[5], "-p")) {
        pipeName = argv[6];
    } else if (equal_strings(argv[7], "-p")) {
        pipeName = argv[8];
    }

    // Crear un objeto de la clase Sensor con los argumentos recibidos:
    if(sensorType == '1'){
        sensor = new TemperatureSensor(sensorType, fileName, pipeName, handlerTime);
    } else {
        sensor = new PhSensor(sensorType, fileName, pipeName, handlerTime);
    }

    // Ejecutar el sensor:
    sensor->run();

    // Detener el sensor:
    sensor->stop();

    // Liberar la memoria del objeto sensor:
    delete sensor;

    // Salir del programa con un código de éxito:
    return EXIT_SUCCESS;
}