#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_FILOSOFOS 5
#define HASHI 5

pthread_mutex_t hashi[HASHI];   
sem_t limite_comendo;                     

void* rotina_filosofo(void* arg) {
    int id_filosofo = *(int*)arg;

    int hashi_esquerdo = id_filosofo;                    
    int hashi_direito = (id_filosofo + 1) % HASHI;

    //ordenar os hashis para evitar starvation
    int primeiro = hashi_esquerdo < hashi_direito ? hashi_esquerdo : hashi_direito;
    int segundo  = hashi_esquerdo < hashi_direito ? hashi_direito : hashi_esquerdo;

    while (1) {
        printf("Filósofo %d está pensando!\n", id_filosofo + 1);
        sleep(1);

        sem_wait(&limite_comendo); //para impedir deadlock

        //pegar hashis (região critica)
        pthread_mutex_lock(&hashi[primeiro]); 
        pthread_mutex_lock(&hashi[segundo]);

        printf("Filósofo %d está comendo!\n", id_filosofo + 1);
        sleep(1);

        //soltar hashis
        pthread_mutex_unlock(&hashi[segundo]);
        pthread_mutex_unlock(&hashi[primeiro]);

        sem_post(&limite_comendo); //liberar espaço para outro filósofo tentar comer
    }
}

int main() {
    pthread_t threads_filosofos[NUM_FILOSOFOS];
    int ids_filosofos[NUM_FILOSOFOS];

    for (int i = 0; i < HASHI; i++) {
        pthread_mutex_init(&hashi[i], NULL);
    }

    sem_init(&limite_comendo, 0, 4);

    //cria cada filósofo como uma thread
    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        ids_filosofos[i] = i;
        pthread_create(&threads_filosofos[i], NULL, rotina_filosofo, &ids_filosofos[i]);
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_join(threads_filosofos[i], NULL);
    }

    return 0;
}
