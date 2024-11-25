#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Definir nodo de lista enlazada
typedef struct node {
    int data;
    struct node* next;
} node_t;

// Variables compartidas para sincronización
pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t readers_ok = PTHREAD_COND_INITIALIZER;
pthread_cond_t writers_ok = PTHREAD_COND_INITIALIZER;

int readers = 0;           // Número de lectores activos
int writers = 0;           // Número de escritores activos
int waiting_writers = 0;   // Número de escritores esperando

node_t* head = NULL;       // Cabeza de la lista enlazada

// Función para insertar en la lista enlazada (escritura)
void list_insert(int value) {
    node_t* new_node = (node_t*)malloc(sizeof(node_t));  // Conversión explícita
    if (new_node == NULL) {  // Verificar que malloc no falle
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    new_node->data = value;
    new_node->next = head;
    head = new_node;
}


// Función para buscar en la lista enlazada (lectura)
int list_search(int value) {
    node_t* current = head;
    while (current != NULL) {
        if (current->data == value) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Función del lector
void* reader(void* arg) {
    int value_to_search = *(int*)arg;

    pthread_mutex_lock(&rw_mutex);

    // Espera si hay escritores activos o esperando
    while (writers > 0 || waiting_writers > 0) {
        pthread_cond_wait(&readers_ok, &rw_mutex);
    }

    readers++;  // Incrementar lectores activos
    pthread_mutex_unlock(&rw_mutex);

    // Realizar operación de lectura
    if (list_search(value_to_search)) {
        printf("Lector encontró el valor: %d\n", value_to_search);
    } else {
        printf("Lector no encontró el valor: %d\n", value_to_search);
    }

    pthread_mutex_lock(&rw_mutex);
    readers--;  // Decrementar lectores activos

    // Si no hay más lectores, permitir a los escritores continuar
    if (readers == 0) {
        pthread_cond_signal(&writers_ok);
    }

    pthread_mutex_unlock(&rw_mutex);
    pthread_exit(NULL);
}

// Función del escritor
void* writer(void* arg) {
    int value_to_insert = *(int*)arg;

    pthread_mutex_lock(&rw_mutex);
    waiting_writers++;  // Incrementar escritores esperando

    // Espera si hay lectores o escritores activos
    while (readers > 0 || writers > 0) {
        pthread_cond_wait(&writers_ok, &rw_mutex);
    }

    waiting_writers--;  // Decrementar escritores esperando
    writers++;  // Incrementar escritores activos
    pthread_mutex_unlock(&rw_mutex);

    // Realizar operación de escritura
    list_insert(value_to_insert);
    printf("Escritor insertó el valor: %d\n", value_to_insert);

    pthread_mutex_lock(&rw_mutex);
    writers--;  // Decrementar escritores activos

    // Dar preferencia a escritores si están esperando
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

    // Valores para buscar e insertar
    int reader_values[] = {10, 20, 30, 40, 50};
    int writer_values[] = {25, 35};

    // Arreglos para almacenar los threads
    pthread_t reader_threads[num_readers];
    pthread_t writer_threads[num_writers];

    // Crear hilos de lectores
    for (int i = 0; i < num_readers; i++) {
        pthread_create(&reader_threads[i], NULL, reader, &reader_values[i]);
    }

    // Crear hilos de escritores
    for (int i = 0; i < num_writers; i++) {
        pthread_create(&writer_threads[i], NULL, writer, &writer_values[i]);
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
