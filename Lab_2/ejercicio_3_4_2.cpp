#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, comm_sz;
    int local_value, sum;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Asignar un valor local a cada proceso (usamos el rank para simplificar)
    local_value = rank;
    sum = local_value;

    // Encontrar la mayor potencia de dos menor o igual a comm_sz
    int active_procs = 1;
    while (active_procs <= comm_sz / 2) {
        active_procs *= 2;
    }

    // Procesos sobrantes (que no forman parte de la potencia de dos más cercana)
    if (rank >= active_procs) {
        // Procesos sobrantes envían su valor a procesos activos en el rango [0, active_procs - 1]
        MPI_Send(&sum, 1, MPI_INT, rank - active_procs, 0, MPI_COMM_WORLD);
    } else {
        if (rank < comm_sz - active_procs) {
            int extra_value;
            MPI_Recv(&extra_value, 1, MPI_INT, rank + active_procs, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += extra_value;
        }

        // Realizar la suma en mariposa para los procesos activos
        for (int i = 0; i < (int)log2(active_procs); i++) {
            int partner = rank ^ (1 << i);
            int received_value;

            // Enviar y recibir el valor del socio
            MPI_Sendrecv(&sum, 1, MPI_INT, partner, 0, &received_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Sumar el valor recibido al acumulador
            sum += received_value;
        }
    }

    // Imprimir la suma global en cada proceso activo
    if (rank < active_procs) {
        printf("Proceso %d tiene la suma global: %d\n", rank, sum);
    }

    MPI_Finalize();
    return 0;
}
