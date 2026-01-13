#include <cstdio>
#include <omp.h>
#include <cstdlib>
#include <ctime>

#define N 512
#define NUM_THREADS 4

double A[N][N], B[N][N], C[N][N];

int main() {
    srand(time(0));

    for (int i = 0; i < N; i++)
      for (int j = 0; j < N; j++) {
        A[i][j] = rand() % 10;
        B[i][j] = rand() % 10;
        C[i][j] = 0;
      }

    omp_set_num_threads(NUM_THREADS);

    double start = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
    }

    double end = omp_get_wtime();

    printf("C[0][0] = %f\n", C[0][0]);
    printf("czas    = %f s\n", end - start);
    printf("watkow  = %d\n", NUM_THREADS);

    return 0;
}
