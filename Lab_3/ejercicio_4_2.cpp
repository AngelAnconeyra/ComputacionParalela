/*
Ejecutar:
g++ -pthread -o ejercicio_4_2 ejercicio_4_2.cpp
./ejercicio_4_2
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Estructura para pasar parámetros a los hilos
typedef struct {
    long long int tosses_per_thread;
    long long int number_in_circle;
    unsigned int seed;
} thread_data_t;

// Función para generar un número aleatorio en el rango [-1, 1]
double rand_double(unsigned int *seed) {
    return 2.0 * ((double)rand_r(seed) / RAND_MAX) - 1.0;
}

// Función ejecutada por cada hilo
void* calculate_pi(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    long long int number_in_circle_local = 0;

    for (long long int toss = 0; toss < data->tosses_per_thread; toss++) {
        double x = rand_double(&data->seed);
        double y = rand_double(&data->seed);
        double distance_squared = x * x + y * y;

        if (distance_squared <= 1.0) {
            number_in_circle_local++;
        }
    }

    data->number_in_circle = number_in_circle_local;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int num_threads;
    long long int total_tosses;
    long long int number_in_circle_total = 0;
    double pi_estimate;

    // Leer el número de hilos y el número total de lanzamientos
    num_threads = strtol(argv[1], NULL, 10);
    total_tosses = 100; //Número de lanzamientos

    // Crear un arreglo para almacenar la información de cada hilo
    pthread_t threads[num_threads];
    thread_data_t thread_data[num_threads];

    // Calcular el número de lanzamientos por hilo
    long long int tosses_per_thread = total_tosses / num_threads;

    // Inicializar los hilos
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].tosses_per_thread = tosses_per_thread;
        thread_data[i].number_in_circle = 0;
        thread_data[i].seed = (unsigned int)time(NULL) ^ (i + 1); // Semilla única para cada hilo

        pthread_create(&threads[i], NULL, calculate_pi, (void*)&thread_data[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        number_in_circle_total += thread_data[i].number_in_circle;
    }

    // Calcular el valor estimado de pi
    pi_estimate = 4.0 * ((double)number_in_circle_total / (double)total_tosses);
    printf("Estimación de pi: %f\n", pi_estimate);

    return 0;
}
