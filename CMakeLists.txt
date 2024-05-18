cmake_minimum_required(VERSION 3.22)
project(PROJECT_SM)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_executable(sensor
        app/sensor_run.cpp
        src/sensor.cpp
        src/phSensor.cpp
        src/temperatureSensor.cpp
        src/stringValidations.cpp
)

add_executable(monitor
        src/stringValidations.cpp
        app/monitor_run.cpp
        src/monitor.cpp
)

add_executable(test_sensor_receive_data
       app/main_process.cpp
)

# add_executable(monitor
#         app/monitor_run.cpp
#         src/monitor.cpp
#         src/... .cpp -> Añadir los archivos que se necesiten !!!
# )

include_directories(include)    # Headers       (.h)
include_directories(src)        # Source files  (.cpp)
include_directories(app)        # Main files    (.cpp)