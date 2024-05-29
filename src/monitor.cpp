#include "monitor.h"

Monitor::Monitor(int tam_buffer)
{
    frontP = 0;
    finalP = 0;
    frontT = 0;
    finalT = 0;
    pthread_mutex_init(&buffermutex, nullptr);
    pthread_mutex_init(&buffermutexT, nullptr);
    sem_init(&semEmptyP, 0, tam_buffer);
    sem_init(&semFullP, 0, 0);
    sem_init(&semEmptyT, 0, tam_buffer);
    sem_init(&semFullT, 0, 0);
    buffPh = new double[tam_buffer];
    buffTp = new int[tam_buffer];
    tamBuff = tam_buffer;
}

void Monitor::run(const std::string& file_temp, const std::string& file_ph, const std::string& pipe_nominal)
{
    silos.emplace_back([this, pipe_nominal]() { recolector(pipe_nominal); });
    silos.emplace_back([this, file_ph]() { phThread("files/" + file_ph); });
    silos.emplace_back([this, file_temp]() { temperaturaThread("files/" + file_temp); });

    for(int i = 0; i < 3; i++)
    {
        silos[i].join();
    }
    sem_destroy(&semFullP);
    sem_destroy(&semEmptyP);
    pthread_mutex_destroy(&buffermutex);
    pthread_mutex_destroy(&buffermutexT);
}

void Monitor::recolector(std::string pipe_nominal)
{
    std::cout << "Entro a MONITOR" << std::endl;
    std::string path = "/tmp/"+pipe_nominal;
    int temperatureData = 0;
    double phData = 0.0;
    const char* pipe = path.c_str();
    int fd, nbytes,  pid;
    uint8_t n[5];


    fd = open(pipe, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error al abrir el pipe: " << strerror(errno) << std::endl;
        exit(-1);
    }

    std::cout << "Pipe abierto correctamente en modo solo lectura." << std::endl;

    //Lectura Pipe
    nbytes = read (fd, &n, sizeof(n));
    do{
        if(n[0] == '1'){
            temperatureData = n[2] * 100 + n[3] * 10 + n[4];
            if(n[1] == 1)
            {
                temperatureData *= -1;
                std::cerr << "[Error]\t\tTemperatura negativa:\t\t" << temperatureData << " °F" << "\t\t" << obtenerHoraActual() << std::endl;
            }
            else{
                sem_wait(&semEmptyT);
                pthread_mutex_lock(&buffermutexT);
                buffTp[frontT] = temperatureData;
                if(finalT == tamBuff - 1)
                {
                    finalT = 0;
                }else{

                    finalT ++;
                }
                pthread_mutex_unlock(&buffermutexT);
                sem_post(&semFullT);
            }
        }else if(n[0] == '2'){
            phData = std::ceil((n[2] * 10 + n[3] + (float)n[4] / 10) * 10) / 10;
            if(n[1] == 1)
            {
                phData *= -1;
                std::cerr << "[Error]\t\tpH negativo:\t\t\t" << std::fixed << std::setprecision(1) << phData << "\t\t" << obtenerHoraActual() << std::endl;
            }
            else{

                sem_wait(&semEmptyP);
                pthread_mutex_lock(&buffermutex);
                buffPh[frontP] = phData;
                if(finalP == tamBuff - 1)
                {
                    finalP = 0;
                }else
                {
                    finalP ++;
                }
                pthread_mutex_unlock(&buffermutex);
                sem_post(&semFullP);
            }
        }
        nbytes = read (fd, &n, sizeof(n));
    } while(nbytes > 0);
    std::cout << "Esperando";
    std::cout.flush();
    for (int i = 1; i <= 10; ++i) {
        sleep(1);
        std::cout << "..." << i;
        std::cout.flush();
    }
    std::cout << std::endl;
    std::cout << "Saliendo..." << std::endl;
    pthread_mutex_lock(&buffermutex);
    buffPh[frontP] = -1;
    pthread_mutex_unlock(&buffermutex);
    sem_post(&semFullP);
    pthread_mutex_lock(&buffermutexT);
    buffTp[frontT] = -1;
    pthread_mutex_unlock(&buffermutexT);
    sem_post(&semFullT);

    if (nbytes) {
        perror("proceso lector:");
        return;
    }

    close(fd);

    if (unlink(pipe) == -1){
        perror("unlink");
        return;
    }

    return;
}

void Monitor::temperaturaThread(const std::string& arch)
{
    std::ofstream salida(arch, std::ios::trunc);
    if(!salida)
    {
        std::cerr << "Error al abrir el archivo: '" << arch << "'." << std::endl;
        exit(1);
    }
    while(1)
    {
        sem_wait(&semFullT);
        pthread_mutex_lock(&buffermutexT);
        if(buffTp[frontT] == -1)
        {
            std::cout << "Temperatura salio" << std::endl;
            salida.close();
            return;
        }
        //cout << "Recibi una Temperatura: " << buffTp[frontT] << endl;
        if(buffTp[frontT] < 68 || buffTp[frontT] > 89)
        {
            std::cerr << "[Alerta]\tTemperatura fuera de rango:\t" << buffTp[frontT] << " °F" << "\t\t" << obtenerHoraActual() << std::endl;
        }
        salida << "Hora actual:\t" << obtenerHoraActual() << "\tTemperatura:\t" << buffTp[frontT] << " C°"<< std::endl;
        if(frontT == tamBuff - 1)
        {
            frontT = 0;
        }else
        {
            frontT ++;
        }
        pthread_mutex_unlock(&buffermutexT);
        sem_post(&semEmptyT);
    }
}

void Monitor::phThread(const std::string& arch)
{
    std::ofstream salida(arch, std::ios::trunc);
    if(!salida.is_open())
    {
        std::cerr << "Error al abrir el archivo: '" << arch << "'." << std::endl;
        exit(1);
    }
    while(true)
    {
        sem_wait(&semFullP);
        pthread_mutex_lock(&buffermutex);
        if(buffPh[frontP] == -1)
        {
            std::cout << "Ph salio" << std::endl;
            salida.close();
            return;
        }
        //cout << "Recibi un Ph: " << buffPh[frontP] << endl;
        if(buffPh[frontP] < 6.0 || buffPh[frontP] > 8.0)
        {
            std::cerr << "[Alerta]\tPH fuera de rango:\t\t" << std::fixed << std::setprecision(1) << buffPh[frontP] << "\t\t" << obtenerHoraActual() << std::endl;
        }
        salida << "Hora actual:\t" << obtenerHoraActual() << "\tPh:\t" << std::setprecision(1) << buffPh[frontP] << std::endl;

        if(frontP == tamBuff - 1)
        {
            frontP = 0;
        }else
        {
            frontP ++;
        }
        pthread_mutex_unlock(&buffermutex);
        sem_post(&semEmptyP);
    }
}

std::string Monitor::obtenerHoraActual()
{
    time_t now = time(NULL); // Obtiene el tiempo actual en formato UNIX.
    struct tm *tm_struct = localtime(
            &now); // Convierte el tiempo UNIX a una estructura de tiempo legible.
    static char timeString[64]; // Almacena la hora en una cadena de texto.
    strftime(
            timeString, sizeof(timeString), "%H:%M:%S",
            tm_struct);    // Convierte la estructura de tiempo a una cadena de texto.
    return timeString; // Devuelve la cadena de texto con la hora.
}