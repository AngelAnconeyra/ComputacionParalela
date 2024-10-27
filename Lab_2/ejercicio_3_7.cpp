#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int rank, comm_sz;
    int pingpong_count = 100000; // Número de ping-pongs repetidos para obtener un tiempo medible
    int data = 0;
    double start_time, end_time;
    clock_t clock_start, clock_end;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (comm_sz < 2) {
        if (rank == 0) {
            fprintf(stderr, "Este programa necesita al menos 2 procesos\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // Usando MPI_Wtime
        start_time = MPI_Wtime();
        for (int i = 0; i < pingpong_count; i++) {
            MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // Enviar a proceso 1 (ping)
            MPI_Recv(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibir de proceso 1 (pong)
        }
        end_time = MPI_Wtime();
        printf("Tiempo con MPI_Wtime en Proceso 0: %f segundos\n", end_time - start_time);

        // Usando clock
        clock_start = clock();
        for (int i = 0; i < pingpong_count; i++) {
            MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // Enviar a proceso 1 (ping)
            MPI_Recv(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibir de proceso 1 (pong)
        }
        clock_end = clock();
        double cpu_time_used = ((double)(clock_end - clock_start)) / CLOCKS_PER_SEC;
        printf("Tiempo con clock en Proceso 0: %f segundos\n", cpu_time_used);
    } else if (rank == 1) {
        for (int i = 0; i < pingpong_count; i++) {
            MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibir de proceso 0 (ping)
            MPI_Send(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // Enviar a proceso 0 (pong)
        }
    }

    MPI_Finalize();
    return 0;
}
