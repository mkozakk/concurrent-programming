#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <unistd.h>
#define COUNT 10

sem_t slots;
sem_t data;



int buf = 0;

void* producent(void* arg) {
    for (int i = 0; i < COUNT; i++) {
        sem_wait(&slots);
        buf = i * 2;
        std::cout << "producent wpisal:" << buf << "\n";
        sem_post(&data);
        usleep(100000);
    }
    return nullptr;
}

void* konsument(void* arg) {
    for (int i = 0; i < COUNT; i++) {
        sem_wait(&data);
        std::cout << "konsument odczytal: " << buf << "\n";
        sem_post(&slots);
        usleep(200000);
    }
    return nullptr;
}

int main() {
    sem_init(&slots, 0, 1);
    sem_init(&data, 0, 0);

    pthread_t p, k;
    pthread_create(&p, nullptr, producent, nullptr);
    pthread_create(&k, nullptr, konsument, nullptr);

    pthread_join(p, nullptr);
    pthread_join(k, nullptr);

    sem_destroy(&slots);
    sem_destroy(&data);
    return 0;
}
