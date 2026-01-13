#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

using namespace std;

#define N 1000000
#define NUM_THREADS 4

int arr[N];

int main() {
    srand(time(0));
    for (int i = 0; i < N; i++)
        arr[i] = rand() % 100;

    omp_set_num_threads(NUM_THREADS);

    long long suma = 0;
    double start = omp_get_wtime();

    #pragma omp parallel for reduction(+:suma)
    for (int i = 0; i < N; i++)
        suma += arr[i];

    double end = omp_get_wtime();

    cout << "suma = " << suma << "\n";
    printf("czas = %f s\n", end - start);
    printf("NUM_THREADS = %d\n", NUM_THREADS);

    return 0;
}
