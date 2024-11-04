// Tomar tiempos de cada hilo cuando se crea y termina, y de todos los tiempo sacar un promedio
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función que será ejecutada por los hilos
void* thread_function(void* arg) {
    pthread_exit(NULL);  // El hilo no realiza ninguna tarea significativa
}

int main(int argc, char* argv[]) {

    int num_threads;
    // Leer número de hilos desde el argumento del programa
    num_threads = strtol(argv[1], NULL, 10);

    // Arreglo para almacenar los identificadores de los hilos
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));

    // Variables para medir el tiempo
    struct timespec start, end;
    double total_time = 0.0;

    // Tomar el tiempo de inicio
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Crear y finalizar los hilos
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Tomar el tiempo de finalización
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calcular el tiempo total en milisegundos
    total_time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;

    // Calcular el tiempo promedio por hilo en milisegundos
    double average_time_per_thread = total_time / num_threads;

    printf("Tiempo total para crear y finalizar %d hilos: %f ms\n", num_threads, total_time);
    printf("Tiempo promedio por hilo: %f ms\n", average_time_per_thread);

    // Liberar la memoria asignada
    free(threads);

    return 0;
}
