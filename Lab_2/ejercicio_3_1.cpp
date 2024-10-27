#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void calculate_bin_maxes(float min_meas, float bin_width, int bin_count, float bin_maxes[]) {
    for (int b = 0; b < bin_count; b++) {
        bin_maxes[b] = min_meas + bin_width * (b + 1);
    }
}

int find_bin(float value, float bin_maxes[], int bin_count, float min_meas) {
    int bin = 0;
    while (bin < bin_count && value >= bin_maxes[bin]) {
        bin++;
    }
    return bin;
}

int main(int argc, char *argv[]) {
    int data_count, bin_count, rank, size;
    float min_meas, max_meas, bin_width;
    float *data = NULL;
    int *bin_counts = NULL, *loc_bin_cts = NULL;
    float *bin_maxes = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Leer los datos de entrada
        // Aquí se asignan valores a data_count, min_meas, max_meas y bin_count.
        // data se debe llenar con valores de entrada.
        
        // Cálculo del ancho de los bins
        bin_width = (max_meas - min_meas) / bin_count;

        // Inicializar bin_maxes y bin_counts
        bin_maxes = malloc(bin_count * sizeof(float));
        bin_counts = malloc(bin_count * sizeof(int));
        for (int i = 0; i < bin_count; i++) bin_counts[i] = 0;

        calculate_bin_maxes(min_meas, bin_width, bin_count, bin_maxes);
    }

    // Compartir datos entre procesos
    MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bin_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&min_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&max_meas, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        bin_maxes = malloc(bin_count * sizeof(float));
    }
    MPI_Bcast(bin_maxes, bin_count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Distribuir datos
    int local_data_count = data_count / size;
    float *local_data = malloc(local_data_count * sizeof(float));
    MPI_Scatter(data, local_data_count, MPI_FLOAT, local_data, local_data_count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Calcular histograma local
    loc_bin_cts = malloc(bin_count * sizeof(int));
    for (int i = 0; i < bin_count; i++) loc_bin_cts[i] = 0;

    for (int i = 0; i < local_data_count; i++) {
        int bin = find_bin(local_data[i], bin_maxes, bin_count, min_meas);
        loc_bin_cts[bin]++;
    }

    // Reducir los resultados locales en el proceso 0
    MPI_Reduce(loc_bin_cts, bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Imprimir el histograma en el proceso 0
    if (rank == 0) {
        printf("Histograma:\n");
        for (int i = 0; i < bin_count; i++) {
            printf("Bin %d: %d\n", i, bin_counts[i]);
        }
    }

    // Liberar memoria
    free(local_data);
    free(loc_bin_cts);
    if (rank == 0) {
        free(data);
        free(bin_counts);
    }
    free(bin_maxes);

    MPI_Finalize();
    return 0;
}
