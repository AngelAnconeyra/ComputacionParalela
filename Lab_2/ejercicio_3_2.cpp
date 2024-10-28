#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Función para generar un número aleatorio en el rango [-1, 1]
double rand_double() {
    return 2.0 * ((double)rand() / RAND_MAX) - 1.0;
}

int main(int argc, char *argv[]) {
    int rank, size;
    long long int tosses_per_proc, number_in_circle_local = 0, number_in_circle_total = 0;
    long long int total_tosses;
    double pi_estimate;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Tiempo de inicio de la ejecución
    total_tosses = 10;
    double start_time, end_time;
    if (rank == 0) {
        // Leer el número total de lanzamientos de usuario
        total_tosses = 10;
        start_time = MPI_Wtime();
        //printf("Ingrese el número total de lanzamientos: ");
        //scanf("%lld", &total_tosses);
    }

    // Broadcast del número total de lanzamientos a todos los procesos
    MPI_Bcast(&total_tosses, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    // Calcular el número de lanzamientos que cada proceso realizará
    tosses_per_proc = total_tosses / size;

    // Semilla para la generación de números aleatorios
    srand(rank + 1);

    // Calcular el número de puntos dentro del círculo
    for (long long int toss = 0; toss < tosses_per_proc; toss++) {
        double x = rand_double();
        double y = rand_double();
        double distance_squared = x * x + y * y;

        if (distance_squared <= 1.0) {
            number_in_circle_local++;
        }
    }

    // Reducir el número de puntos dentro del círculo de todos los procesos al proceso 0
    MPI_Reduce(&number_in_circle_local, &number_in_circle_total, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Calcular y mostrar el valor estimado de pi en el proceso 0
    if (rank == 0) {
        end_time = MPI_Wtime();
        double elapsed_time_ms = (end_time - start_time) * 1000.0; // Conversión a milisegundos
        printf("Tiempo de ejecución con %d procesos: %f ms\n", size, elapsed_time_ms);

        pi_estimate = 4.0 * ((double)number_in_circle_total / (double)total_tosses);
        printf("Estimación de pi: %f\n", pi_estimate);
    }

    // Finalizar MPI
    MPI_Finalize();
    return 0;
}
