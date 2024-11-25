#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Estructura para pasar el índice del hilo y su tiempo
typedef struct {
    int thread_id;
    double creation_time;
    double completion_time;
} thread_info_t;

// Función que será ejecutada por los hilos
void* thread_function(void* arg) {
    thread_info_t* info = (thread_info_t*)arg;
    struct timespec start, end;

    // Medir el tiempo de creación del hilo
    clock_gettime(CLOCK_MONOTONIC, &start);
    info->creation_time = (start.tv_sec * 1000.0) + (start.tv_nsec / 1e6);

    // Simulación de trabajo del hilo (sin hacer nada en este caso)
    pthread_exit(NULL);

    // Medir el tiempo de finalización del hilo
    clock_gettime(CLOCK_MONOTONIC, &end);
    info->completion_time = (end.tv_sec * 1000.0) + (end.tv_nsec / 1e6);
    
    return NULL;
}

int main(int argc, char* argv[]) {

    int num_threads;
    // Leer número de hilos desde el argumento del programa
    num_threads = strtol(argv[1], NULL, 10);

    // Arreglo para almacenar los identificadores de los hilos
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    thread_info_t* thread_info = (thread_info_t*)malloc(num_threads * sizeof(thread_info_t));

    // Variables para calcular el tiempo promedio
    double total_time = 0.0;
    double thread_creation_total_time = 0.0;
    double thread_completion_total_time = 0.0;

    // Crear los hilos
    for (int i = 0; i < num_threads; i++) {
        thread_info[i].thread_id = i;
        pthread_create(&threads[i], NULL, thread_function, (void*)&thread_info[i]);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        thread_creation_total_time += thread_info[i].creation_time;
        thread_completion_total_time += thread_info[i].completion_time;
    }

    // Calcular el tiempo total de creación y finalización
    double average_creation_time = thread_creation_total_time / num_threads;
    double average_completion_time = thread_completion_total_time / num_threads;
    total_time = average_completion_time - average_creation_time;

    // Mostrar los tiempos
    printf("Tiempo total de creación y finalización de los hilos: %f ms\n", total_time);
    printf("Tiempo promedio de creación por hilo: %f ms\n", average_creation_time);
    printf("Tiempo promedio de finalización por hilo: %f ms\n", average_completion_time);

    // Liberar la memoria asignada
    free(threads);
    free(thread_info);

    return 0;
}
