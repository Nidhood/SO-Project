#ifndef PROJECT_SM_STRING_VALIDATIONS_H
#define PROJECT_SM_STRING_VALIDATIONS_H

#include <iostream>
#include <regex>

// Función para comparar dos cadenas de caracteres:
bool equal_strings(const std::string &str1, const std::string &str2);

// Función para determinar si una cadena de caracteres cumple con el formato de un archivo:
bool is_valid_file(const std::string &str);

// Función para determinar si una cadena de caracteres cumple con el formato de un pipe nominal:
bool is_valid_pipe(const std::string &str);

// Función para verificar si una opción se encuentra duplicada en los argumentos de la línea de comandos:
bool check_duplicate_option(const char* option, int argc, char* argv[]);

// Función para asegurar que el archivo tenga la extensión .txt:
std::string ensure_txt_extension(const std::string &str);

// Función para verificar si una bandera es válida en los argumentos de la línea de comandos:
bool is_valid_flag(const std::string &str, int argc, char* argv[]);

#endif