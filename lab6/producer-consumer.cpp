#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>

#define BUFFER_SIZE 10
#define COUNT 100
#define CONSUMER_COUNT 3
#define PRODUCER_COUNT 5



int arr[COUNT];
int prime_count = 0;
pthread_mutex_t prime_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;


int is_prime(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }

    return 1;
}

class MonitorPK {
private:
    int buf[BUFFER_SIZE];
    int in = 0, out = 0, count = 0;
    bool finished = false;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t mozna_produkowac = PTHREAD_COND_INITIALIZER;
    pthread_cond_t mozna_konsumowac = PTHREAD_COND_INITIALIZER;


public:
    void zapis(int item) {
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE)
            pthread_cond_wait(&mozna_produkowac, &mutex);

        buf[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&mozna_konsumowac);
        pthread_mutex_unlock(&mutex);
    }

    int odczyt(bool &ok) {
        pthread_mutex_lock(&mutex);

        while (count == 0 && !finished)
            pthread_cond_wait(&mozna_konsumowac, &mutex);

        if (count == 0 && finished) {
            ok = false;
            pthread_mutex_unlock(&mutex);
            return -1;
        }

        int item = buf[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;

        pthread_cond_signal(&mozna_produkowac);
        pthread_mutex_unlock(&mutex);

        ok = true;
        return item;
    }

    void set_finished() {
        pthread_mutex_lock(&mutex);
        finished = true;
        pthread_cond_broadcast(&mozna_konsumowac);
        pthread_mutex_unlock(&mutex);
    }
};
MonitorPK monitor;

void* producent(void* arg) {
    long id = (long)arg;
    int chunk = COUNT / PRODUCER_COUNT;

    for (int i = 0; i < chunk; i++) {
        int num = arr[id * chunk + i];
        if (is_prime(num)) {
            monitor.zapis(num);
        }
    }
    return nullptr;
}

void* konsument(void* arg) {
    long id = (long)arg;

    while (true) {
        bool ok;
        int item = monitor.odczyt(ok);
        if (!ok)
            break;

        pthread_mutex_lock(&prime_count_mutex);
        prime_count++;
        pthread_mutex_unlock(&prime_count_mutex);

        pthread_mutex_lock(&print_mutex);
        std::cout << "Konsument " << id << " odczytal: " << item << "\n";
        pthread_mutex_unlock(&print_mutex);

        usleep(300000);
    }
    return nullptr;
}

int main() {
    pthread_t p[PRODUCER_COUNT], k[CONSUMER_COUNT];

    for (int i = 0; i < COUNT; i++)
        arr[i] = rand() % 100;

    for (long i = 0; i < PRODUCER_COUNT; i++)
        pthread_create(&p[i], nullptr, producent, (void*)i);

    for (long i = 0; i < CONSUMER_COUNT; i++)
        pthread_create(&k[i], nullptr, konsument, (void*)i);

    for (int i = 0; i < PRODUCER_COUNT; i++)
        pthread_join(p[i], nullptr);

    monitor.set_finished();

    for (int i = 0; i < CONSUMER_COUNT; i++)
        pthread_join(k[i], nullptr);

    std::cout << "pierwszych:" << prime_count << "\n";
    return 0;
}
