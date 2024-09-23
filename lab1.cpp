/*
El primer bucle tiene menor tiempo de ejecucion debido a que accede a los datos
en la matriz bidimensionalmente en bloques contiguos, lo que no ocurre con el segundo buble
debido a que lo recorremos en forma de columnas y filas, ocurriendo lo contrario.

1. Ejercicio
// 100
Primer bucle: 125 microsegundos
Segundo bucle: 132 microsegundos
// 200
Primer bucle: 321 microsegundos
Segundo bucle: 333 microsegundos
// 300
Primer bucle: 538 microsegundos
Segundo bucle: 553 microsegundos
// 500
Primer bucle: 2028 microsegundos
Segundo bucle: 3420 microsegundos
// 1000
Primer bucle: 5993 microsegundos
Segundo bucle: 7572 microsegundos
// 10000 
Primer par de bucles: 726 866 microsegundos
Segundo par de bucles: 1 643 761 microsegundos

2. Ejercicio
// 10
Tiempo:64 microsegundos
// 20
Tiempo:249 microsegundos
// 30
Tiempo:1222 microsegundos
// 50
Tiempo:4741 microsegundos


*/

#include <iostream>
#include <chrono>
#include <random>
using namespace std;

const int MAX = 50;

double A[MAX][MAX], x[MAX], y[MAX];

void primer_ejercicio(){
    cout<<"Tamaño: "<<MAX<<endl;
    // Inicializa A y x, asigna y = 0
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            A[i][j] = 0.0;
        }
        x[i] = 0.0;
        y[i] = 0.0;
    }

    //tiempo
    auto start_time = chrono::high_resolution_clock::now();
    // First pair of loops
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            y[i] += A[i][j] * x[j];
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Primer bucle: " << duration.count() << " microsegundos" << endl;

    // Asigna y = 0
    for (int i = 0; i < MAX; i++) {
        y[i] = 0.0;
    }

    // Tiempo
    start_time = chrono::high_resolution_clock::now();

    //Second pair of loops
    for (int j = 0; j < MAX; j++) {
        for (int i = 0; i < MAX; i++) {
            y[i] += A[i][j] * x[j];
        }
    }

    end_time = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Segundo bucle: " << duration.count() << " microsegundos" << endl;
}

void segundo_ejercicio(){
    vector<vector<int>> A (MAX,vector<int>(MAX, 0));
    vector<vector<int>> B (MAX,vector<int>(MAX, 0));
    vector<vector<int>> C(MAX, vector<int>(MAX, 0));

    //Llenado aleatorio
    for (int i = 0; i < MAX; ++i) {
        for (int j = 0; j < MAX; ++j) {
            A[i][j] = rand() % 100;  
        }
    }

    for (int i = 0; i < MAX; ++i) {
        for (int j = 0; j < MAX; ++j) {
            B[i][j] = rand() % 100;  
        }
    }

    //Tiempo
    auto start_time = chrono::high_resolution_clock::now();
    //Multiplicación
    for (int i = 0; i < MAX; ++i) {
        for (int j = 0; j < MAX; ++j) {
            for (int k = 0; k < MAX; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Tiempo - "<<MAX<<": " << duration.count() << " microsegundos" << endl;
    /*
    // Mostramos el resultado
    cout << "Resultado de la multiplicación de matrices:" << std::endl;
    for (int i = 0; i < MAX; ++i) {
        for (int j = 0; j < MAX; ++j) {
            cout << C[i][j] << " ";
        }
        cout << endl;
    }
    */
}



int main() {
    //primer_ejercicio();
    segundo_ejercicio();

    return 0;
}
