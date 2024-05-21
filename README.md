# Monitoreo de Sensores 🌊🔬

Este proyecto tiene como objetivo desarrollar un sistema de monitoreo de la calidad del agua, simulando la medición de dos parámetros esenciales: pH y temperatura. Los estudiantes aplicarán herramientas para la comunicación y sincronización de procesos e hilos, utilizando pipes nominales para la comunicación entre procesos y semáforos para sincronizar los hilos. Además, se emplearán llamadas al sistema para manipular archivos de texto. 📊💧

## Contexto 🌍

El agua es un recurso vital en nuestro planeta y su calidad es crucial para diversos usos, como la agricultura o la generación de energía. Este proyecto simula un sistema de monitoreo de la calidad del agua, centrándose en la medición de pH y temperatura. 🚰🌡️

## Descripción General del Sistema 🛠️

El sistema consta de tres componentes principales: sensores, monitor y mecanismos de sincronización y comunicación.

## Profesor:

- **Profesor:** Ing. Osberth Cristhian Luef De Castro Cuevas.
- **Correo Electrónico:** [decastro-oc@javeriana.edu.co]()

## Equipo de Desarrollo:

- Saul Arias Trejo - [@Eddie918](https://github.com/Eddie918) 🧑‍💻
- Iván Darío Orozco Ibáñez - [@Nidhood](https://github.com/Nidhood) 🧑‍💻

### Sensores 🎛️

Los sensores, simulados por procesos, miden la temperatura y el pH. Cada sensor se ejecuta con parámetros específicos, como el tipo de sensor, el tiempo entre mediciones, el archivo de datos y el pipe nominal para la comunicación con el monitor. 📌🔍

### Monitor 🖥️

El monitor recibe las mediciones de los sensores y las gestiona mediante tres hilos:

- **H-recolector:** Recibe las mediciones y las coloca en búferes separados según el tipo de medida. Si la medición es errónea, se descarta. Cuando no hay sensores conectados, espera antes de finalizar.
- **H-ph:** Recoge las mediciones de pH y las escribe en un archivo, anexando la hora actual. Si la medida está fuera de los rangos, emite una alerta.
- **H-temperatura:** Similar a H-ph, pero para mediciones de temperatura. 🌡️🕰️

### Mecanismos de Sincronización y Comunicación 🔄

- **Comunicación entre Hilos:** Se emplea el patrón productor/consumidor con buffers acotados y semáforos para la sincronización.
- **Comunicación entre Procesos:** Los sensores envían mediciones al monitor a través de pipes nominales.

## Procedimiento para usar el Repositorio:

1. **Clonar el Repositorio:**
   - Clone este repositorio a su máquina local utilizando el siguiente comando: 🖥️
     ```
     git clone https://github.com/Nidhood/SO-Project.git
     ```

2. **Explorar el Repositorio:**
   - Explore el contenido del repositorio para familiarizarse con la estructura y los archivos proporcionados. 📂
   - Revise los archivos de código fuente y los documentos de instrucciones para comprender cómo utilizar las diferentes funciones implementadas. 📚

3. **Configurar el Proyecto:**
   - Ejecute el archivo `init.sh` utilizando el siguiente comando en la terminal para configurar el proyecto y crear los ejecutables del sensor y el monitor:
     ```
     source init.sh
     ```

4. **Ejecutar el Programa:**
   - Luego de configurar el proyecto, ejecute el sensor y luego el monitor con los siguientes flags:

   - Para el sensor:
     ```
     ./sensor –s tipo-sensor –t tiempo –f archivo –p pipe-nominal
     ```
     - `tipo-sensor`: Especifica si el sensor reportará valores de pH (utilizar el número 1) o de temperatura (utilizar el número 2).
     - `tiempo`: Indica cada cuanto tiempo se enviará la medición al monitor.
     - `archivo`: Nombre del archivo con las medidas de temperatura o de pH.
     - `pipe-nominal`: Nombre del pipe nominal que permite la comunicación entre los sensores y el monitor.

   - Para el monitor:
     ```
     ./monitor –b tam_buffer –t file-temp –h file-ph -p pipe-nominal
     ```
     - `tam_buffer`: Tamaño de los búferes donde el recolector colocará las medidas.
     - `file-temp`: Nombre del archivo donde se colocarán las mediciones de temperatura.
     - `file-ph`: Nombre del archivo donde se colocarán las mediciones de pH.
     - `pipe-nominal`: Nombre del pipe nominal para la comunicación entre los sensores y el monitor.

## Demostración del Funcionamiento 📸

Encuentra fotos de ejemplo en la carpeta `photos` en la raíz del repositorio. Estas capturas muestran la ejecución del programa, incluyendo la interacción con los sensores y el procesamiento de las mediciones. Puedes acceder a las fotos [aquí](https://github.com/Nidhood/SO-Project/raw/main/photos).


![](https://github.com/Nidhood/SO-Project/raw/main/photos/test_4.png)

