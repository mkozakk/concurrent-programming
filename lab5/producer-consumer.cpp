#include <pthread.h>
#include <iostream>
#include <unistd.h>

#define BUFFER_SIZE 10
#define COUNT 20
#define CONSUMER_COUNT 3
#define PRODUCER_COUNT 5





pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consume_count_mutex = PTHREAD_MUTEX_INITIALIZER;
int consumed_total = 0;

class MonitorPK {
private:
    int buf[BUFFER_SIZE];
    int in = 0
    int out = 0
    int count = 0;

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
        int val = item + id * chunk;
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
        std::cout << "Konsument " << id << " pobral:" << item << "\n";
        pthread_mutex_unlock(&print_mutex);

        usleep(300000);
    }
}

int main() {
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

