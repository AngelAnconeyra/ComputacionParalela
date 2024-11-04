
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex principal
pthread_cond_t readers_ok = PTHREAD_COND_INITIALIZER;  // Condición para lectores
pthread_cond_t writers_ok = PTHREAD_COND_INITIALIZER;  // Condición para escritores

int readers = 0;  // Número de lectores activos
int writers = 0;  // Número de escritores activos
int waiting_writers = 0;  // Escritores esperando

// Estructura para datos compartidos
struct Data {
    int value;  // Valor compartido
};

// Función para lectores
void* reader(void* arg) {
    struct Data* data = (struct Data*)arg;
    
    pthread_mutex_lock(&rw_mutex);

    // Espera si hay escritores activos o escritores esperando (dar preferencia a los escritores)
    while (writers > 0 || waiting_writers > 0) {
        pthread_cond_wait(&readers_ok, &rw_mutex);
    }

    readers++;  // Incrementar el número de lectores activos
    pthread_mutex_unlock(&rw_mutex);

    // Simular lectura
    printf("Lector está leyendo el valor: %d\n", data->value);

    pthread_mutex_lock(&rw_mutex);
    readers--;  // Decrementar el número de lectores activos

    // Si no hay más lectores, permitir a los escritores continuar
    if (readers == 0) {
        pthread_cond_signal(&writers_ok);
    }

    pthread_mutex_unlock(&rw_mutex);
    pthread_exit(NULL);
}

// Función para escritores
void* writer(void* arg) {
    struct Data* data = (struct Data*)arg;

    pthread_mutex_lock(&rw_mutex);
    waiting_writers++;  // Incrementar el número de escritores esperando

    // Espera si hay lectores o escritores activos
    while (readers > 0 || writers > 0) {
        pthread_cond_wait(&writers_ok, &rw_mutex);
    }

    waiting_writers--;  // Decrementar el número de escritores esperando
    writers++;  // Incrementar el número de escritores activos
    pthread_mutex_unlock(&rw_mutex);

    // Simular escritura
    data->value++;  // Modificar el valor compartido
    printf("Escritor está escribiendo el nuevo valor: %d\n", data->value);

    pthread_mutex_lock(&rw_mutex);
    writers--;  // Decrementar el número de escritores activos

    // Dar preferencia a los escritores, si hay alguno esperando
    if (waiting_writers > 0) {
        pthread_cond_signal(&writers_ok);
    } else {
        pthread_cond_broadcast(&readers_ok);
    }

    pthread_mutex_unlock(&rw_mutex);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int num_readers = 5;
    int num_writers = 2;

    // Inicializar el valor compartido
    struct Data shared_data = {0};

    // Arreglos para almacenar los threads
    pthread_t reader_threads[num_readers];
    pthread_t writer_threads[num_writers];

    // Crear hilos de lectores
    for (int i = 0; i < num_readers; i++) {
        pthread_create(&reader_threads[i], NULL, reader, (void*)&shared_data);
    }

    // Crear hilos de escritores
    for (int i = 0; i < num_writers; i++) {
        pthread_create(&writer_threads[i], NULL, writer, (void*)&shared_data);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < num_readers; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    for (int i = 0; i < num_writers; i++) {
        pthread_join(writer_threads[i], NULL);
    }

    return 0;
}
