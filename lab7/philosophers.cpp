#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#define N 5

pthread_mutex_t widelec[N];

void* filozof(void* arg) {
    long id = (long)arg;

    int lewy  = id;
    int prawy = (id + 1) % N;

    int pierwszy = lewy < prawy ? lewy : prawy;
    int drugi    = lewy < prawy ? prawy : lewy;

    for (int i = 0; i < 3; i++) {
        std::cout << "Filozof " << id << " mysli\n";
        usleep(150000);

        pthread_mutex_lock(&widelec[pierwszy]);
        pthread_mutex_lock(&widelec[drugi]);

        std::cout << "Filozof " << id << " je\n";
        usleep(100000);

        pthread_mutex_unlock(&widelec[drugi]);
        pthread_mutex_unlock(&widelec[pierwszy]);
    }
    return nullptr;
}

int main() {
    srand(42);

    for (int i = 0; i < N; i++)
        pthread_mutex_init(&widelec[i], nullptr);

    pthread_t t[N];
    for (long i = 0; i < N; i++)
        pthread_create(&t[i], nullptr, filozof, (void*)i);

    for (int i = 0; i < N; i++)
        pthread_join(t[i], nullptr);

    for (int i = 0; i < N; i++)
        pthread_mutex_destroy(&widelec[i]);

    return 0;
}
