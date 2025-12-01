#include <pthread.h>
#include <iostream>
#include <unistd.h>

#define READERS 4
#define WRITERS 2

int zasob = 0;
int aktywni_czytelnicy = 0;
bool pisze = false;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mozna_czytac = PTHREAD_COND_INITIALIZER;
pthread_cond_t mozna_pisac = PTHREAD_COND_INITIALIZER;

void* czytelnik(void* arg) {
    long id = (long)arg;

    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&mutex);
        while (pisze)
            pthread_cond_wait(&mozna_czytac, &mutex);
        aktywni_czytelnicy++;
        pthread_mutex_unlock(&mutex);

        std::cout << "Czytelnik " << id << " czyta: " << zasob << "\n";
        usleep(200000);

        pthread_mutex_lock(&mutex);
        aktywni_czytelnicy--;
        if (aktywni_czytelnicy == 0)
            pthread_cond_signal(&mozna_pisac);
        pthread_mutex_unlock(&mutex);

        usleep(100000);
    }
    return nullptr;
}

void* pisarz(void* arg) {
    long id = (long)arg;

    for (int i = 0; i < 2; i++) {
        pthread_mutex_lock(&mutex);
        while (pisze || aktywni_czytelnicy > 0)
            pthread_cond_wait(&mozna_pisac, &mutex);
        pisze = true;
        pthread_mutex_unlock(&mutex);

        zasob++;
        std::cout << "Pisarz " << id << " zapisal: " << zasob << "\n";
        usleep(300000);

        pthread_mutex_lock(&mutex);
        pisze = false;
        pthread_cond_broadcast(&mozna_czytac);
        pthread_cond_signal(&mozna_pisac);
        pthread_mutex_unlock(&mutex);

        usleep(200000);
    }
    return nullptr;
}

int main() {
    pthread_t r[READERS], w[WRITERS];

    for (long i = 0; i < READERS; i++)
        pthread_create(&r[i], nullptr, czytelnik, (void*)i);

    for (long i = 0; i < WRITERS; i++)
        pthread_create(&w[i], nullptr, pisarz, (void*)i);

    for (int i = 0; i < READERS; i++)
        pthread_join(r[i], nullptr);

    for (int i = 0; i < WRITERS; i++)
        pthread_join(w[i], nullptr);

    return 0;
}
