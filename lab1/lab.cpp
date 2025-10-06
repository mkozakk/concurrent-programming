#include <pthread.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define THREADS 5

void* hello(void* arg) {
    long id = (long)arg;
    usleep((THREADS - id) * 100000);
    cout << "Watek " << id << " mowi hej!\n";
    return nullptr;
}

int main() {
    pthread_t t[THREADS];

    for (long i = 0; i < THREADS; i++)
        pthread_create(&t[i], nullptr, hello, (void*)i);

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], nullptr);

    cout << "wszystkie watki skonczone\n";
    return 0;
}
