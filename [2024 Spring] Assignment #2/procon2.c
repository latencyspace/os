#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

volatile int flag[2] = {0, 0};
volatile int turn = 0;

void mfence() {
    // asm volatile("mfence" ::: "memory");
    __sync_synchronize();
}

void peterson_lock(int self) {
    int other = 1 - self;
    flag[self] = 1;
    turn = other;
    mfence();
    while (flag[other] && turn == other) {
        // Busy-wait
    }
}

void peterson_unlock(int self) {
    flag[self] = 0;
}

#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int count = 0;

void *producer(void *param) {
    int self = *(int *)param;
    for (int i = 0; i < 10; i++) {
        peterson_lock(self);
        if (count < BUFFER_SIZE) {
            buffer[count++] = i;
            printf("Produced: %d\n", i);
        }
        peterson_unlock(self);
    }
    return NULL;
}

void *consumer(void *param) {
    int self = *(int *)param;
    for (int i = 0; i < 10; i++) {
        peterson_lock(self);
        if (count > 0) {
            int item = buffer[--count];
            printf("Consumed: %d\n", item);
        }
        peterson_unlock(self);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    int id1 = 0, id2 = 1;

    clock_t start = clock();

    pthread_create(&tid1, NULL, producer, &id1);
    pthread_create(&tid2, NULL, consumer, &id2);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}

