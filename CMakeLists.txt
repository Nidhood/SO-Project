cmake_minimum_required(VERSION 3.28)
project(PROJECT_SM)

set(CMAKE_CXX_STANDARD 17)

add_executable(sensor
        main.cpp
        src/sensor.cpp
        src/phSensor.cpp
        src/temperatureSensor.cpp
        src/stringValidations.cpp
)

include_directories(include)
include_directories(src)
