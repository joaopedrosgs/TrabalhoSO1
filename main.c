#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_THREADS 20
int numero_h = 0, numero_o = 0, h_gerado = 0, o_gerado = 0;
pthread_mutex_t lock;
sem_t sem_o, sem_h;
int formados = 0;

void *
Oxygen (void *v)
{
  pthread_mutex_lock (&lock);
  numero_o++;

  printf ("Hidrogenio: %d\tOxigenio: %d\t<-\tNovo Oxigenio \n", numero_h,
	  numero_o);

  if (numero_o >= 1 && numero_h >= 2)
    {
      numero_o--;
      numero_h -= 2;
      sem_post (&sem_h);
      sem_post (&sem_h);

      printf ("H20 Formado\nThread retornando (Oxigenio)\n");
      formados++;

      pthread_mutex_unlock (&lock);

    }
  else
    {
      pthread_mutex_unlock (&lock);
      sem_wait (&sem_o);
      printf ("Thread retornando (Oxigenio)\n");

    }
  return NULL;

}

void *
Hidrogen (void *v)
{
  pthread_mutex_lock (&lock);
  numero_h++;
  printf ("Hidrogenio: %d\tOxigenio: %d\t<-\tNovo Hidrogenio \n", numero_h,
	  numero_o);
  if (numero_o >= 1 && numero_h >= 2)
    {

      numero_o--;
      numero_h -= 2;

      sem_post (&sem_o);
      sem_post (&sem_h);

      printf ("H20 Formado\nThread retornando (Hidrogenio)\n");
      formados++;

      pthread_mutex_unlock (&lock);
    }
  else
    {
      pthread_mutex_unlock (&lock);
      sem_wait (&sem_h);
      printf ("Thread retornando (Hidrogenio)\n");

    }

  return NULL;
}

int
main ()
{
  srand (time (NULL));
  int tamanho = MAX_THREADS;
  pthread_t pthread[tamanho + 1];
  sem_init (&sem_o, 1, 0);
  sem_init (&sem_h, 1, 0);

  while (tamanho >= 0)
    {
      if (rand () % 2)
	{
	  pthread_create (&pthread[tamanho], NULL, Hidrogen, NULL);
	  h_gerado++;
	}
      else
	{
	  o_gerado++;
	  pthread_create (&pthread[tamanho], NULL, Oxygen, NULL);
	}

      tamanho--;
    }
  usleep (100000);
  while (numero_o > 0)
    {
      sem_post (&sem_o);
      numero_o--;
    }
  while (numero_h > 0)
    {
      sem_post (&sem_h);
      numero_h--;
    }
  for (tamanho = MAX_THREADS; tamanho >= 0; tamanho--)
    {
      pthread_join (pthread[tamanho], NULL);	// poderia ser join, mas podem haver threads paradas esperando serem conectadas
      printf ("Join na thread %d\n", tamanho);
    }


  printf ("Oxigenio produzido: %d\t Hidrogenio produzido: %d\t\n", o_gerado,
	  h_gerado);
  int formado_esperado = h_gerado / 2;
  if (o_gerado <= h_gerado / 2)
    {				// menos O do que H2
      formado_esperado = o_gerado;
    }

  printf ("H2O produzido: %d\t H2O esperado: %d\t\n", formados,
	  formado_esperado);


  return 0;
}
