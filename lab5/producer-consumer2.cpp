#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <random>

#define BUFFER_SIZE 10
#define COUNT 100
#define CONSUMER_COUNT 3
#define PRODUCER_COUNT 5




int prime_count = 0;
int arr[COUNT];
int consumed_total = 0;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consume_count_mutex = PTHREAD_MUTEX_INITIALIZER;


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
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&mozna_produkowac, &mutex);
        }

        buf[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&mozna_konsumowac);
        pthread_mutex_unlock(&mutex);
    }

    int odczyt(bool &ok) {
        pthread_mutex_lock(&mutex);

        while (count == 0 && !finished) {
            pthread_cond_wait(&mozna_konsumowac, &mutex);
        }

        if (count == 0 && finished) {
            ok = false;
            pthread_mutex_unlock(&mutex);
            return -1;
        }

        int item = buf[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        prime_count++;

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

    for (int item = 0; item < chunk; item++) {
        int num = arr[id * chunk + item];
        
        if (is_prime(num)) {
            monitor.zapis(num);
    
            usleep(200000);
        }
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

        bool ok;
        int item = monitor.odczyt(ok);
        if (!ok) break;

        pthread_mutex_lock(&print_mutex);
        std::cout << "Konsument " << id << "odczytal: " << item << "\n";
        pthread_mutex_unlock(&print_mutex);

        usleep(300000);
    }
    
    return nullptr;
}


int main() {
    pthread_t p[PRODUCER_COUNT], k[CONSUMER_COUNT];
    for (int o = 0; o < COUNT; o++) arr[o] = rand() % 100;

    for (long i = 0; i < PRODUCER_COUNT; i++)
        pthread_create(&p[i], nullptr, producent, (void*)i);

    for (long j = 0; j < CONSUMER_COUNT; j++)
        pthread_create(&k[j], nullptr, konsument, (void*)j);


    for (int i = 0; i < PRODUCER_COUNT; i++)
        pthread_join(p[i], nullptr);
        


    monitor.set_finished();

    for (int j = 0; j < CONSUMER_COUNT; j++)
        pthread_join(k[j], nullptr);



    std::cout << "primes:" << prime_count << "\n";

    return 0;
}

