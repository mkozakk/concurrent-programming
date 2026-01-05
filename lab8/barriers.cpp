#include <pthread.h>
#include <iostream>
#include <unistd.h>

#define THREADS 4

pthread_barrier_t bariera;
int wyniki[THREADS];

void* worker(void* arg) {
    long id = (long)arg;

    usleep((id + 1) * 100000);
    wyniki[id] = id * id;
    std::cout << "Watek " << id << " skonczyl obliczenia\n";

    pthread_barrier_wait(&bariera);

    if (id == 0) {
        std::cout << "--- wyniki ---\n";
        for (int i = 0; i < THREADS; i++)
            std::cout << "wyniki[" << i << "] = " << wyniki[i] << "\n";
    }

    return nullptr;
}

int main() {
    pthread_barrier_init(&bariera, nullptr, THREADS);

    pthread_t t[THREADS];
    for (long i = 0; i < THREADS; i++)
        pthread_create(&t[i], nullptr, worker, (void*)i);

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], nullptr);

    pthread_barrier_destroy(&bariera);
    return 0;
}
