#!/bin/bash

# Borramos los archivos anteriores
rm -rf cmake-build-debug
rm -f sensor
rm -f test_sensor_receive_data

# Compilamos el proyecto
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make
cd ..
echo "La inicialización del proyecto ha finalizado. Los ejecutables están en la carpeta raíz."