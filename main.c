#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>



#define MAX_THREADS 25
#define second 1000000

int numero_h = 0, numero_o = 0, h_gerado = 0, o_gerado = 0, formados=0;
pthread_mutex_t lock;  // mutex para travar acesso às variáveis acima
sem_t sem_o, sem_h; // semaforos para travar hidrogenios e oxigenios

void* Oxigenio(void* v) {
  pthread_mutex_lock(&lock); // Pedindo acesso às variaveis
  numero_o++;

  printf("Hidrogenio: %d\tOxigenio: %d\t<-\tNovo Oxigenio \n", numero_h,
         numero_o);

  if (numero_o >= 1 && numero_h >= 2) {
    numero_o--;
    numero_h -= 2;
    sem_post(&sem_h);
    sem_post(&sem_h);

    printf("H20 Formado(%d)\nThread retornando (Oxigenio)\n", formados++);

    pthread_mutex_unlock(&lock); // Liberando acesso às variaveis
  } else {
    pthread_mutex_unlock(&lock); // Liberando acesso às variaveis
    sem_wait(&sem_o); // Como o semaforo é iniciado em 0, todo oxigenio que chega aqui trava
    printf("Thread retornando (Oxigenio)\n");
  }
  return NULL;
}

void* Hidrogenio(void* v) {
  pthread_mutex_lock(&lock); // Pedindo acesso às variaveis
  numero_h++;

  printf("Hidrogenio: %d\tOxigenio: %d\t<-\tNovo Hidrogenio \n", numero_h,
         numero_o);
  if (numero_o >= 1 && numero_h >= 2) {
    numero_o--;
    numero_h -= 2;

    sem_post(&sem_o); // Liberamos um oxigenio e um hidrogenio (somados com mais esse hidrogenio que está executando)
    sem_post(&sem_h);

    printf("H20 Formado(%d)\nThread retornando (Hidrogenio)\n", formados++);

    pthread_mutex_unlock(&lock); // Liberando acesso às variaveis
  } else {
    pthread_mutex_unlock(&lock);// Liberando acesso às variaveis
    sem_wait(&sem_h); // Como o semaforo é iniciado em 0, todo oxigenio que chega aqui trava
    printf("Thread retornando (Hidrogenio)\n");
  }

  return NULL;
}

int main() {
  srand(time(NULL));
  int tamanho = MAX_THREADS-1;
  pthread_t pthread[tamanho];
  sem_init(&sem_o, 1, 0);
  sem_init(&sem_h, 1, 0);

  while (tamanho >= 0) {
    if (rand() % 2) {
      pthread_create(&pthread[tamanho], NULL, Hidrogenio, NULL);
      h_gerado++;
    } else {
      o_gerado++;
      pthread_create(&pthread[tamanho], NULL, Oxigenio, NULL);
    }

    tamanho--;
  }
  usleep(0.2*second);
  while (numero_o > 0) {
    sem_post(&sem_o);
    numero_o--;
  }
  while (numero_h > 0) {
    sem_post(&sem_h);
    numero_h--;
  }
  for (tamanho = MAX_THREADS-1; tamanho >= 0; tamanho--) {
    pthread_join(pthread[tamanho], NULL);  
    printf("Join na thread %d\n", tamanho);
  }

  printf("Oxigenio produzido: %d\t Hidrogenio produzido: %d\t\n", o_gerado,
         h_gerado);
  int formado_esperado = h_gerado / 2;
  if (o_gerado <= h_gerado / 2) {  // menos O do que H2
    formado_esperado = o_gerado;
  }

  printf("H2O produzido: %d\t H2O esperado: %d\t\n", formados,
         formado_esperado);

  return 0;
}
