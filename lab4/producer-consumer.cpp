#include <pthread.h>
#include <iostream>
#include <unistd.h>



#define BUFFER_SIZE 10 // rozmiar bufora
#define COUNT 20 // liczba produkowanych elementow



class MonitorPK {
private:
    int buf[BUFFER_SIZE];
    int in = 0, out = 0, count = 0;
    int produkuje = 0, konsumuje = 0;
    int chce_produkowac = 0
    int chce_konsumowac = 0;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t mozna_produkowac = PTHREAD_COND_INITIALIZER;
    pthread_cond_t mozna_konsumowac = PTHREAD_COND_INITIALIZER;


public:
    MonitorPK() {}

    void poczatek_produkcji() {
        pthread_mutex_lock(&mutex);
        chce_produkowac++;

        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&mozna_produkowac, &mutex);
        }

        chce_produkowac--;
        produkuje++;

        pthread_mutex_unlock(&mutex);
    }

    void koniec_produkcji(int item) {
        pthread_mutex_lock(&mutex);
        buf[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        produkuje--;

        if (chce_konsumowac > 0) {
            pthread_cond_signal(&mozna_konsumowac);
        }
        else
            pthread_cond_signal(&mozna_produkowac);

        pthread_mutex_unlock(&mutex);
    }

    int poczatek_konsumpcji() {
        pthread_mutex_lock(&mutex);

        chce_konsumowac++;

        while (count == 0) 
            pthread_cond_wait(&mozna_konsumowac, &mutex);
        

        chce_konsumowac--;
        konsumuje++;

        int item = buf[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;

        pthread_mutex_unlock(&mutex);
        return item;
    }

    void koniec_konsumpcji() {
        pthread_mutex_lock(&mutex);

        konsumuje--;

        if (chce_produkowac > 0)
            pthread_cond_signal(&mozna_produkowac);
        else
            pthread_cond_signal(&mozna_konsumowac);

        pthread_mutex_unlock(&mutex);
    }
};


MonitorPK monitor;

void* producent(void*) {
    for (int item = 0; item < COUNT; item++) {
        monitor.poczatek_produkcji();
        monitor.koniec_produkcji(item);

        std::cout << "Producent wyprodukowal:" << item << "\n";
        usleep(200000);
    }
    return nullptr;
}

void* konsument(void*) {
    for (int i = 0; i < COUNT; i++) {
        int item = monitor.poczatek_konsumpcji();
        monitor.koniec_konsumpcji();

        std::cout << "Konsument pobral:" << item << "\n";
        usleep(300000);
    }
    return nullptr;
}

int main() {
    pthread_t p, k;

    pthread_create(&p, nullptr, producent, nullptr);
    pthread_create(&k, nullptr, konsument, nullptr);

    pthread_join(p, nullptr);
    pthread_join(k, nullptr);



    return 0;
}
