#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>


int numero_h = 0, numero_o = 0, esperando_h = 0, esperando_o = 0;
pthread_mutex_t lock;
sem_t sem_o, sem_h;

void *Oxygen(void *v) {

    pthread_mutex_lock(&lock);
    numero_o++;

    printf("Hidrogenio: %d\tOxigenio: %d <- Novo Oxigenio \n", numero_h, numero_o);


    if (numero_o >= 1 && numero_h >= 2) {

        numero_o--;
        numero_h -= 2;

        sem_post(&sem_o);
        sem_post(&sem_h);
        sem_post(&sem_h);

        printf("H20 Formado\n");

        pthread_mutex_unlock(&lock);


    } else {
        pthread_mutex_unlock(&lock);
        sem_wait(&sem_o);


    }
    printf("Thread retornando\n");


}

void *Hidrogen(void *v) {
    pthread_mutex_lock(&lock);
    numero_h++;
    printf("Hidrogenio: %d\tOxigenio: %d <- Novo Hidrogenio \n", numero_h, numero_o);


    if (numero_o >= 1 && numero_h >= 2) {

        numero_o--;
        numero_h -= 2;

        sem_post(&sem_o);
        sem_post(&sem_h);
        sem_post(&sem_h);

        printf("H20 Formado\n");

        pthread_mutex_unlock(&lock);


    } else {
        pthread_mutex_unlock(&lock);
        sem_wait(&sem_h);


    }

    printf("Thread retornando\n");


}

int main() {

    srand(time(NULL));   // should only be called once
    int r = rand() % 2;
    int tamanho = 300;
    pthread_t pthread[tamanho];
    sem_init(&sem_o, 1, 1);
    sem_init(&sem_h, 1, 2);


    int i = tamanho - 1;
    while (i >= 0) {
        if (r)
            pthread_create(&pthread[i], NULL, Hidrogen, NULL);
        else
            pthread_create(&pthread[i], NULL, Oxygen, NULL);
        r = rand() % 2;


        i--;
    }
    return 0;
}