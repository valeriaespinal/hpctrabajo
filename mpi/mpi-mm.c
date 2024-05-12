#include <stdio.h>
#include <time.h>
#include "mpi.h"
#include <stdlib.h>

MPI_Status status;

int main(int argc, char const *argv[]) {
    int numtasks, taskid, numworkers, source, dest, rows, offset, i, j, k;
    double start, stop;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    numworkers = numtasks - 1;

    int N = atoi(argv[1]);
    double *a, *b, *c;
    a = (double *)malloc(N * N * sizeof(double));
    b = (double *)malloc(N * N * sizeof(double));
    c = (double *)malloc(N * N * sizeof(double));

    /*---------------------------- master ----------------------------*/
    if (taskid == 0) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                a[i * N + j] = (rand() % 4) + 1;
                b[i * N + j] = (rand() % 4) + 1;
            }
        }

        start = MPI_Wtime();

        /* send matrix data to the worker tasks */
        rows = N / numworkers;
        offset = 0;

        for (dest = 1; dest <= numworkers; dest++) {
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&a[offset * N], rows * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            MPI_Send(b, N * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            offset = offset + rows;
        }

        /* wait for results from all worker tasks */
        for (i = 1; i <= numworkers; i++) {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset * N], rows * N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
        }

        stop = MPI_Wtime();
     
        printf("TOTAL time: %lf\n", stop - start);
    }

    /*---------------------------- worker----------------------------*/
    if (taskid > 0) {
        source = 0;
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        double *local_a = (double *)malloc(rows * N * sizeof(double));
        double *local_b = (double *)malloc(N * N * sizeof(double));
        double *local_c = (double *)malloc(rows * N * sizeof(double));
        MPI_Recv(local_a, rows * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(local_b, N * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

        /* Matrix multiplication */
        for (k = 0; k < N; k++) {
            for (i = 0; i < rows; i++) {
                local_c[i * N + k] = 0.0;
                for (j = 0; j < N; j++)
                    local_c[i * N + k] += local_a[i * N + j] * local_b[j * N + k];
            }
        }

        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(local_c, rows * N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);

        free(local_a);
        free(local_b);
        free(local_c);
    }

    free(a);
    free(b);
    free(c);
    MPI_Finalize();
    return 0;
}

