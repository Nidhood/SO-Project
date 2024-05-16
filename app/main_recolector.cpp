#include "monitor.h"
#include "stringValidations.h"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <string>

#include <unistd.h>

std::string pipe_nominal;
void recolector_data();

int main(int argc, char *argv[]) {
  int tam_buffer = 0;
  std::string file_temp;
  std::string file_ph;
  std::string pipe_nominal;
  if (argc != 9) {
    std::cout << "Uso: ./monitor -b tam_buffer -t file_temp -h file_temp -p "
                 "pipe_nominal\n"
                 "Donde:\n"
                 "tam_buffer: el tamaño del buffer de datos\n"
                 "file_temp: el nombre del archivo de temperatura\n"
                 "file_hp: nombre del archivo de ph\n"
                 "pipe_nominal: nombre del pipe nominal\n";
    perror("Cantidad de argumento no valida.\n ");
    exit(1);
  } else {
    if (check_duplicate_option("-b", argc, argv)) {
      perror("Solo puede haber un argumento '-b'");
      exit(1);
    }
    if (check_duplicate_option("-t", argc, argv)) {
      perror("Solo puede haber un argumento '-t'");
      exit(1);
    }
    if (check_duplicate_option("-h", argc, argv)) {
      perror("Solo puede haber un argumento '-h'");
      exit(1);
    }
    if (check_duplicate_option("-p", argc, argv)) {
      perror("Solo puede haber un argumento '-p'");
      exit(1);
    }

    if (equal_strings(argv[1], "-b") && equal_strings(argv[3], "-b") &&
        equal_strings(argv[5], "-b") && equal_strings(argv[7], "-b")) {
      perror("Solo puede haber un argumento '-b'");
      exit(1);
    } else if (equal_strings(argv[1], "-t") && equal_strings(argv[3], "-t") &&
               equal_strings(argv[5], "-t") && equal_strings(argv[7], "-t")) {
      perror("Solo puede haber un argumento '-t'");
      exit(1);
    } else if (equal_strings(argv[1], "-h") && equal_strings(argv[3], "-h") &&
               equal_strings(argv[5], "-h") && equal_strings(argv[7], "-h")) {
      perror("Solo puede haber un argumento '-h'");
      exit(1);
    } else if (equal_strings(argv[1], "-p") && equal_strings(argv[3], "-p") &&
               equal_strings(argv[5], "-p") && equal_strings(argv[7], "-p")) {
      perror("Solo puede haber un argumento '-p'");
      exit(1);
    }

    if (equal_strings(argv[1], "-b") && std::stoi(argv[2]) <= 0) {
      perror("error b: El tamaño del buffer debe ser entero positivo");
      exit(1);
    } else if (equal_strings(argv[3], "-b") && std::stoi(argv[4]) <= 0) {
      perror("error b: El tamaño del buffer debe ser entero positivo");
      exit(1);
    } else if (equal_strings(argv[5], "-b") && std::stoi(argv[6]) <= 0) {
      perror("error b: El tamaño del buffer debe ser entero positivo");
      exit(1);
    } else if (equal_strings(argv[7], "-b") && std::stoi(argv[8]) <= 0) {
      perror("error b: El tamaño del buffer debe ser entero positivo");
      exit(1);
    }
    // confirmar validacion de archivo para temperatura
    if (equal_strings(argv[1], "-t") && !is_valid_file(argv[2])) {
      perror("error t: nombre invalido del archivo");
      exit(1);
    } else if (equal_strings(argv[3], "-t") && !is_valid_file(argv[4])) {
      perror("error t: nombre invalido del archivo");
      exit(1);
    } else if (equal_strings(argv[5], "-t") && !is_valid_file(argv[6])) {
      perror("error t: nombre invalido del archivo");
      exit(1);
    } else if (equal_strings(argv[7], "-t") && !is_valid_file(argv[8])) {
      perror("error t: nombre invalido del archivo");
      exit(1);
    }
    // confirmar validacion de archivo para hp
    if (equal_strings(argv[1], "-h") && !is_valid_file(argv[2])) {
      perror("error h: nombre invalido del archivo");
      exit(1);
    } else if (equal_strings(argv[3], "-h") && !is_valid_file(argv[4])) {
      perror("error h: nombre invalido del archivo");
      exit(1);
    } else if (equal_strings(argv[5], "-h") && !is_valid_file(argv[6])) {
      perror("error h: nombre invalido del archivo");
      exit(1);
    } else if (equal_strings(argv[7], "-h") && !is_valid_file(argv[8])) {
      perror("error h: nombre invalido del archivo");
      exit(1);
    }
    // confirmar validacion de pipe nominal
    if (equal_strings(argv[1], "-p") && !is_valid_pipe(argv[2])) {
      perror("error p: nombre invalido del pipe nominal");
      exit(1);
    } else if (equal_strings(argv[3], "-p") && !is_valid_pipe(argv[4])) {
      perror("error p: nombre invalido del pipe nominal");
      exit(1);
    } else if (equal_strings(argv[5], "-p") && !is_valid_pipe(argv[6])) {
      perror("error p: nombre invalido del pipe nominal");
      exit(1);
    } else if (equal_strings(argv[7], "-p") && !is_valid_pipe(argv[8])) {
      perror("error p: nombre invalido del pipe nominal");
      exit(1);
    }
  }
    // asignando tamaño buffer
    if (equal_strings(argv[1], "-b")) {
      tam_buffer = std::stoi(argv[2]);
    } else if (equal_strings(argv[3], "-b")) {
      tam_buffer = std::stoi(argv[4]);
    } else if (equal_strings(argv[5], "-b")) {
      tam_buffer = std::stoi(argv[6]);
    } else if (equal_strings(argv[7], "-b")) {
      tam_buffer = std::stoi(argv[8]);
    }

    // asignando nombre archivo temperatura
    if (equal_strings(argv[1], "-t")) {
      file_temp = argv[2];
    } else if (equal_strings(argv[3], "-t")) {
      file_temp = argv[4];
    } else if (equal_strings(argv[5], "-t")) {
      file_temp = argv[6];
    } else if (equal_strings(argv[7], "-t")) {
      file_temp = argv[8];
    }
    // asignando nombre archivo ph
    if (equal_strings(argv[1], "-h")) {
      file_ph = argv[2];
    } else if (equal_strings(argv[3], "-h")) {
      file_ph = argv[4];
    } else if (equal_strings(argv[5], "-h")) {
      file_ph = argv[6];
    } else if (equal_strings(argv[7], "-h")) {
      file_ph = argv[8];
    }
    // asignando nombre del pipe nominal
    if (equal_strings(argv[1], "-p")) {
      pipe_nominal = argv[2];
    } else if (equal_strings(argv[3], "-p")) {
      pipe_nominal = argv[4];
    } else if (equal_strings(argv[5], "-p")) {
      pipe_nominal = argv[6];
    } else if (equal_strings(argv[7], "-p")) {
      pipe_nominal = argv[8];
    }
    Monitor monitor_sensores(file_temp, file_ph, pipe_nominal, tam_buffer);
    monitor_sensores.run();
    monitor_sensores.join();
}