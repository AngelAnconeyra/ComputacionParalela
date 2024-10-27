#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, comm_sz;
    int local_value, sum;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Asignar un valor local a cada proceso (aquí simplemente igualamos el valor al rank)
    local_value = rank;
    sum = local_value;

    // Realizar la suma en árbol
    int step = 1;
    while (step < comm_sz) {
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

    // El proceso 0 contiene la suma global
    if (rank == 0) {
        printf("La suma global es: %d\n", sum);
    }

    MPI_Finalize();
    return 0;
}
