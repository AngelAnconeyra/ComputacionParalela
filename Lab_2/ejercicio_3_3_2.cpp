#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, comm_sz;
    int local_value, sum;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Asignar un valor local a cada proceso
    local_value = rank;
    sum = local_value;

    // Ajustar cuando comm_sz no es potencia de dos
    int active_procs = comm_sz;
    while (active_procs & (active_procs - 1)) {
        active_procs--; // Reduce a la mayor potencia de dos menor o igual a comm_sz
    }

    // Si hay procesos "extra" fuera de la potencia de dos, ellos envían su valor al primer proceso
    if (rank >= active_procs) {
        MPI_Send(&sum, 1, MPI_INT, rank - active_procs, 0, MPI_COMM_WORLD);
    } else {
        if (rank < comm_sz - active_procs) {
            int extra_value;
            MPI_Recv(&extra_value, 1, MPI_INT, rank + active_procs, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += extra_value;
        }

        // Realizar la suma en árbol
        int step = 1;
        while (step < active_procs) {
            if (rank % (2 * step) == 0) {
                int received_value;
                MPI_Recv(&received_value, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum += received_value;
            } else if ((rank - step) % (2 * step) == 0) {
                MPI_Send(&sum, 1, MPI_INT, rank - step, 0, MPI_COMM_WORLD);
                break; // Este proceso ya no participa en los siguientes pasos
            }
            step *= 2;
        }
    }

    // El proceso 0 contiene la suma global
    if (rank == 0) {
        printf("La suma global es: %d\n", sum);
    }

    MPI_Finalize();
    return 0;
}
