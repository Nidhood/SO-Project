#include "stringValidations.h"

bool equal_strings(const std::string &str1, const std::string &str2){

    // Primero vamos a comparar que el tamaño de las dos cadenas sean las mismas:
    if(str1.size() != str2.size())
        return false;

    // Ahora si vamos a comparar las dos cadenas:
    auto str1_it = str1.begin();
    auto str2_it = str2.begin();

    // Mientras no lleguemos al final de las dos cadenas:
    while(str1_it != str1.end() && str2_it != str2.end()){
        if(*str1_it != *str2_it)
            return false;
        str1_it++;
        str2_it++;
    }

    // Si llegamos a este punto, significa que las dos cadenas son iguales:
    return true;
}

bool is_valid_flag(const std::string &str, int argc, char* argv[]){

    // Definimos una expresión regular para verificar si el nombre de la bandera es válida.
    std::regex flag_regex("^-[a-zA-Z]$");

    // Verificar que la bandera sea válida en las posiciones impares:
    for(int i = 1; i < argc; i += 2){
        if(equal_strings(argv[i], str)){
            return std::regex_match(argv[i], flag_regex);
        }
    }
    return false;
}

bool is_valid_file(const std::string &str){

    // Definimos una expresión regular para verificar si el nombre del archivo es válido, permitiendo opcionalmente la extensión .txt.
    std::regex file_regex("^[a-zA-Z0-9_]+(\\.txt)?$");

    // Verificar si el nombre del archivo cumple con la expresión regular:
    return std::regex_match(str, file_regex);
}

// Función para asegurar que el archivo tenga la extensión .txt.
std::string ensure_txt_extension(const std::string &str) {
    // Verificar si el nombre del archivo ya tiene la extensión .txt.
    if (str.size() >= 4 && str.substr(str.size() - 4) == ".txt") {
        return str; // Si ya tiene la extensión .txt, devolver el nombre original.
    } else {
        return str + ".txt"; // Si no tiene la extensión .txt, agregarla y devolver el nuevo nombre.
    }
}


bool is_valid_pipe(const std::string &str){

    // Definimos una expresión regular para verificar si el nombre del pipe nominal es válido.
    std::regex pipe_regex("^[a-zA-Z][a-zA-Z0-9_]+$");

    // Verificar si el nombre del pipe nominal cumple con la expresión regular:
    return std::regex_match(str, pipe_regex);
}

bool check_duplicate_option(const char* option, int argc, char* argv[]) {
    int count = 0;
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]) == option) {
            count++;
            if (count > 1) {
                return true; // Se encontró una duplicación
            }
        }
    }
    return false; // No se encontraron duplicaciones
}