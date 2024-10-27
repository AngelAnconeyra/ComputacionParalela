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

    // Número de etapas en la estructura en mariposa es log2(comm_sz)
    int steps = 0;
    int temp_comm_sz = comm_sz;
    while (temp_comm_sz > 1) {
        temp_comm_sz /= 2;
        steps++;
    }

    // Realizar la suma en mariposa
    for (int i = 0; i < steps; i++) {
        int partner = rank ^ (1 << i); // Calcular pareja en el paso i usando XOR
        int received_value;

        // Enviar y recibir el valor del socio
        MPI_Sendrecv(&sum, 1, MPI_INT, partner, 0, &received_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Sumar el valor recibido al acumulador
        sum += received_value;
    }

    // Imprimir la suma global en cada proceso
    printf("Proceso %d tiene la suma global: %d\n", rank, sum);

    MPI_Finalize();
    return 0;
}
