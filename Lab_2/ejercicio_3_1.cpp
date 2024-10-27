#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//Calcula los límites superiores de cada bin para un histograma
void calculate_bin_maxes(float min_meas, float bin_width, int bin_count, float bin_maxes[]) {
    for (int b = 0; b < bin_count; b++) {
        bin_maxes[b] = min_meas + bin_width * (b + 1);
    }
}

//Realiza la búsqueda del bin al que pertenece un valor
int find_bin(float value, float bin_maxes[], int bin_count, float min_meas) {
    int bin = 0;
    while (bin < bin_count && value >= bin_maxes[bin]) {
        bin++;
    }
    return bin;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int data_count = 100; // Número total de datos
    int bin_count = 10;   // Número de bins
    float min_meas = 0.0, max_meas = 20.0; // Rango de los datos
    float data[data_count];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Tiempo de inicio de la ejecución
    double start_time, end_time;
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    int local_data_count = data_count / size; // Número de datos por proceso
    float bin_width = (max_meas - min_meas) / bin_count;
    float bin_maxes[bin_count];
    int bin_counts[bin_count];
    int loc_bin_cts[bin_count];
    
    for (int i = 0; i < bin_count; i++) {
        loc_bin_cts[i] = 0;
    }

    if (rank == 0) {
        // Inicializar la semilla para generar números aleatorios
        srand(42);  // Fija la semilla para asegurar los mismos números cada vez

        // Generar 100 números aleatorios entre min_meas y max_meas
        for (int i = 0; i < data_count; i++) {
            data[i] = min_meas + ((float)rand() / RAND_MAX) * (max_meas - min_meas);
        }

        // Calcular los límites de los bins
        calculate_bin_maxes(min_meas, bin_width, bin_count, bin_maxes);

        // Inicializar el histograma global
        for (int i = 0; i < bin_count; i++) {
            bin_counts[i] = 0;
        }
    }

    // Broadcast de bin_maxes y bin_width a todos los procesos
    MPI_Bcast(bin_maxes, bin_count, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bin_width, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Distribuir datos entre procesos
    float local_data[local_data_count];
    MPI_Scatter(data, local_data_count, MPI_FLOAT, local_data, local_data_count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Calcular el histograma local
    for (int i = 0; i < local_data_count; i++) {
        int bin = find_bin(local_data[i], bin_maxes, bin_count, min_meas);
        loc_bin_cts[bin]++;
    }

    // Reducir los resultados locales en el proceso 0
    MPI_Reduce(loc_bin_cts, bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Tiempo de finalización de la ejecución
    if (rank == 0) {
        end_time = MPI_Wtime();
        double elapsed_time_ms = (end_time - start_time) * 1000.0; // Conversión a milisegundos
        printf("Tiempo de ejecución con %d procesos: %f ms\n", size, elapsed_time_ms);

        // Imprimir el histograma
        //printf("Histograma:\n");
        //for (int i = 0; i < bin_count; i++) {
        //    printf("Bin %d: %d\n", i, bin_counts[i]);
        //}
    }

    MPI_Finalize();
    return 0;
}

