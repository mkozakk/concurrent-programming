#include <pthread.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define THREADS 4
#define ITERATIONS 100000

int counter = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* increment(void* arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mtx);
        counter++;
        pthread_mutex_unlock(&mtx);
    }
    return nullptr;
}

int main() {
    pthread_t t[THREADS];

    for (int i = 0; i < THREADS; i++)
        pthread_create(&t[i], nullptr, increment, nullptr);

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], nullptr);

    cout << "licznik=" << counter << "\n";
    cout << "val="<< THREADS*ITERATIONS << "\n";
    return 0;
}
