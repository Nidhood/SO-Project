#ifndef MONITOR_H
#define MONITOR_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <ctime>
#include <pthread.h>
#include <vector>


class Monitor {
    std::vector<std::thread> silos;
    sem_t semEmptyP;
    sem_t semFullP;
    sem_t semEmptyT;
    sem_t semFullT;
    pthread_mutex_t buffermutex;
    pthread_mutex_t buffermutexT;
    double *buffPh;
    int frontP;
    int finalP;
    int *buffTp;
    int frontT;
    int finalT;
    int tamBuff;

    public:
    Monitor(int tam_buffer);
    void run(std::string file_temp, std::string file_ph, std::string pipe_nominal);

    private:
    void recolector(std::string pipe_nominal); 
    void tempetarutaThread(std::string arch);
    void phThread(std::string arch);
    std::string obtenerHoraActual();
};

#endif