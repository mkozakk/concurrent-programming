#include <pthread.h>
#include <iostream>
#include <unistd.h>


#define WORKERS 3
#define TASKS   10

struct Task {
    int id;
    bool valid;
};



struct Queue {
    Task tasks[TASKS + 1];
    int head = 0, tail = 0, count = 0;
    bool done = false;

    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

    void push(Task t) {
        pthread_mutex_lock(&mtx);
        tasks[tail] = t;
        tail = (tail + 1) % (TASKS + 1);
        count++;
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mtx);
    }

    Task pop() {
        pthread_mutex_lock(&mtx);
        while (count == 0 && !done)
            pthread_cond_wait(&not_empty, &mtx);

        Task t;
        if (count == 0) {
            t.valid = false;
            pthread_mutex_unlock(&mtx);
            return t;
        }
        t = tasks[head];
        t.valid = true;
        head = (head + 1) % (TASKS + 1);
        count--;
        pthread_mutex_unlock(&mtx);
        return t;
    }

    void finish() {
        pthread_mutex_lock(&mtx);
        done = true;
        pthread_cond_broadcast(&not_empty);
        pthread_mutex_unlock(&mtx);
    }
};


Queue kolejka;

void* worker(void* arg) {
    long id = (long)arg;
    while (true) {
        Task t = kolejka.pop();
        if (!t.valid) break;
        std::cout << id << " przetwarza zadanie " << t.id << "\n";
        usleep(200000);
    }
    return nullptr;
}

int main() {
    pthread_t workers[WORKERS];
    for (long i = 0; i < WORKERS; i++)
        pthread_create(&workers[i], nullptr, worker, (void*)i);

    for (int i = 0; i < TASKS; i++) {
        Task t;
        t.id = i;
        kolejka.push(t);
        usleep(50000);
    }

    kolejka.finish();

    for (int i = 0; i < WORKERS; i++)
        pthread_join(workers[i], nullptr);

    return 0;
}
