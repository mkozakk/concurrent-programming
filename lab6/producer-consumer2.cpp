#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <random>

#define BUFFER_SIZE 5
#define COUNT 50
#define CONSUMER_COUNT 2
#define PRODUCER_COUNT 2

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consume_count_mutex = PTHREAD_MUTEX_INITIALIZER;

int consumed_total = 0;

class MonitorPK {
private:
    int buf[BUFFER_SIZE];
    int in = 0, out = 0, count = 0;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t mozna_produkowac = PTHREAD_COND_INITIALIZER;
    pthread_cond_t mozna_konsumowac = PTHREAD_COND_INITIALIZER;

public:
    void zapis(int item) {
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&mozna_produkowac, &mutex);
        }

        buf[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&mozna_konsumowac);
        pthread_mutex_unlock(&mutex);
    }

    int odczyt() {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            pthread_cond_wait(&mozna_konsumowac, &mutex);
        }

        int item = buf[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;

        pthread_cond_signal(&mozna_produkowac);
        pthread_mutex_unlock(&mutex);
        return item;
    }
};

MonitorPK monitor;

void* producent(void* arg) {
    long id = (long)arg;

    int chunk = COUNT / PRODUCER_COUNT;

    for (int item = 0; item < chunk; item++) {
        int val = 10 + random() % 90;
        monitor.zapis(val);

        pthread_mutex_lock(&print_mutex);
        std::cout << "Producent " << id << " wyprodukowal: " << val << "\n";
        pthread_mutex_unlock(&print_mutex);

        usleep(200000);
    }
    return nullptr;
}

void* konsument(void* arg) {
    long id = (long)arg;

    while (true) {
        pthread_mutex_lock(&consume_count_mutex);
        if (consumed_total >= COUNT) {
            pthread_mutex_unlock(&consume_count_mutex);
            return nullptr;
        }
        consumed_total++;
        pthread_mutex_unlock(&consume_count_mutex);

        int item = monitor.odczyt();

        pthread_mutex_lock(&print_mutex);
        std::cout << "Konsument " << id << " pobral:      " << item << "\n";
        pthread_mutex_unlock(&print_mutex);

        usleep(300000);
    }
}

int main() {
    srand(time(NULL));
    
    
    pthread_t p[PRODUCER_COUNT], k[CONSUMER_COUNT];

    for (long i = 0; i < PRODUCER_COUNT; i++)
        pthread_create(&p[i], nullptr, producent, (void*)i);

    for (long j = 0; j < CONSUMER_COUNT; j++)
        pthread_create(&k[j], nullptr, konsument, (void*)j);

    for (int i = 0; i < PRODUCER_COUNT; i++)
        pthread_join(p[i], nullptr);

    for (int j = 0; j < CONSUMER_COUNT; j++)
        pthread_join(k[j], nullptr);

    return 0;
}

