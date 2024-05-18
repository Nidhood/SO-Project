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
 
void Monitor::run(std::string file_temp, std::string file_ph, std::string pipe_nominal)
{
    silos.emplace_back([this, pipe_nominal]() { recolector(pipe_nominal); });
    silos.emplace_back([this, file_ph]() { phThread(file_ph); });
    silos.emplace_back([this, file_temp]() { tempetarutaThread(file_temp); });

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
  const char* pipe = path.c_str(); 
  
  int fd, nbytes,  pid;
   
  char n[5];
   
  fd = open(pipe, O_RDONLY);
  if (fd == -1) {
      std::cerr << "Error al abrir el pipe: " << strerror(errno) << std::endl;
      return;
  }

    std::cout << "Pipe abierto correctamente en modo solo lectura." << std::endl;
  
  //Lectura Pipe 
   nbytes = read (fd, &n, sizeof(n));
   do{
    if(n[0] == '1'){
      bool negativo = n[1] == 1;
      if(negativo)
      {
        std::cout << "lei un valor negativo" << std::endl;
      }
      else{
        int temperature = (n[2] - '0') * 100 + (n[3] - '0') * 10 + (n[4] - '0');
        sem_wait(&semEmptyT);
        pthread_mutex_lock(&buffermutexT);
        buffTp[frontT] = temperature;
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
      bool negativo = n[1] == 1;
      if(negativo)
      {
        std::cout << "lei un valor negativo" << std::endl;
      }
      else{
        double ph = (n[2] - '0') * 10 + (n[3] - '0') + (n[4] - '0') / 10.0;
        sem_wait(&semEmptyP);
        pthread_mutex_lock(&buffermutex);
        buffPh[frontP] = ph;
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
   std::cout << "esperando..." << std::endl;
   sleep(10);
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

void Monitor::tempetarutaThread(std::string arch)
{
  std::ofstream salida(arch);
  if(!salida)
  {
    std::cerr << "Error al abrir el archivo:" << arch << std::endl;
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
    if(buffTp[frontT] < 20 || buffTp[frontT] > 31.6)
    {
      std::cout << "Temperatura fuera del rango" << std::endl;
    }
    salida << buffTp[frontT] << " " << obtenerHoraActual() << std::endl;
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

void Monitor::phThread(std::string arch)
{
  std::ofstream salida(arch);
  if(!salida)
  {
    std::cerr << "Error al abrir el archivo:" << arch << std::endl;
    exit(1);
  }
  while(1)
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
      std::cout << "Ph fuera del rango" << std::endl;
    }
    salida << buffPh[frontP] << " " << obtenerHoraActual() << std::endl;
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

