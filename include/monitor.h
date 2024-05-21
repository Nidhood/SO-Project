#ifndef MONITOR_H
#define MONITOR_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <ctime>
#include <pthread.h>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cstdint>

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
    void run(const std::string& file_temp, const std::string& file_ph, const std::string& pipe_nominal);

    private:
    void recolector(std::string pipe_nominal); 
    void temperaturaThread(const std::string& arch);
    void phThread(const std::string& arch);
    std::string obtenerHoraActual();
};

#endif