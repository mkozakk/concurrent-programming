#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

#define N 90000
#define NUM_THREADS 11


double a[N];

int main() {
srand(time(0));

    for (int i = 0; i < N; ++i) a[i] = rand() % 100;

    omp_set_num_threads(NUM_THREADS);


    int maxT = NUM_THREADS;
    double *thread_min_val = new double[maxT];
    int *thread_min_idx = new int[maxT];

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        for (int i = 0; i < N - 1; ++i) {
            double loc_min = a[i];
            int loc_k = i;

            #pragma omp for nowait
            for (int j = i + 1; j < N; ++j) {
                if (a[j] < loc_min) {
                    loc_min = a[j];
                    loc_k = j;
                }
            }

            thread_min_val[tid] = loc_min;
            thread_min_idx[tid] = loc_k;

            #pragma omp barrier

            #pragma omp single
            {
                double gmin = a[i];
                int gk = i;
                int nthreads = omp_get_num_threads();
                for (int t = 0; t < nthreads; ++t) {
                    if (thread_min_val[t] < gmin) {
                        gmin = thread_min_val[t];
                        gk = thread_min_idx[t];
                    }
                }
                if (gk != i) {
                    double tmp = a[i];
                    a[i] = a[gk];
                    a[gk] = tmp;
                }
            }

            #pragma omp barrier
        }
    }

    double end = omp_get_wtime();
    double czas = end - start;

    delete[] thread_min_val;
    delete[] thread_min_idx;

    printf("NUM_THREADS = %d\n", NUM_THREADS);
    printf("czas=%lf\n", czas);

    return 0;
}