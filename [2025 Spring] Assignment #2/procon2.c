#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N_THREADS 2
#define N_COUNTER 4
#define MILLI 1000

volatile int level[N_THREADS];
volatile int victim[N_THREADS - 1];

sem_t semWrite, semRead;

int queue[N_COUNTER];
int wptr = 0, rptr = 0;

void filter_lock(int thread_id) {
    for (int l = 1; l < N_THREADS; l++) {
        level[thread_id] = l;
        victim[l - 1] = thread_id;

        int conflict;
        do {
            conflict = 0;
            for (int other = 0; other < N_THREADS; other++) {
                if (other == thread_id) continue;
                if (level[other] >= l && victim[l - 1] == thread_id) {
                    conflict = 1;
                    break;
                }
            }
        } while (conflict);
    }
}

void filter_unlock(int thread_id) {
    level[thread_id] = 0;
}

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        sem_wait(&semWrite);
        filter_lock(id);
        queue[wptr] = i;
        wptr = (wptr + 1) % N_COUNTER;
        printf("producer: wrote %d\n", i);
        filter_unlock(id);
        sem_post(&semRead);
        usleep(MILLI * (rand() % 10) * 10);
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        sem_wait(&semRead);
        filter_lock(id);
        int value = queue[rptr];
        rptr = (rptr + 1) % N_COUNTER;
        printf("\tconsumer: read %d\n", value);
        filter_unlock(id);
        sem_post(&semWrite);
        usleep(MILLI * (rand() % 10) * 10);
    }
    return NULL;
}

int main() {
    pthread_t threads[N_THREADS];
    int ids[N_THREADS] = {0, 1};
    srand(time(NULL));

    for (int i = 0; i < N_THREADS; i++)
        level[i] = 0;

    sem_init(&semWrite, 0, N_COUNTER);
    sem_init(&semRead, 0, 0);

    pthread_create(&threads[0], NULL, producer, &ids[0]);
    pthread_create(&threads[1], NULL, consumer, &ids[1]);

    for (int i = 0; i < N_THREADS; i++)
        pthread_join(threads[i], NULL);

    sem_destroy(&semWrite);
    sem_destroy(&semRead);

    return 0;
}
