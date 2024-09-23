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

3. Ejercicio
// 50 - 5 block
Tiempo: 6908 microsegundos
// 50 - 10 block
Tiempo: 6524 microsegundos
// 50 - 25 block
Tiempo: 6790 microsegundos
// 100 - 10 block
Tiempo: 57 001 microsegundos
// 100 - 20 block
Tiempo: 51 125 microsegundos
// 100 - 25 block
Tiempo: 45 255 microsegundos
// 1000 - 20 block
Tiempo: 48 350 158 microsegundos
// 1000 - 200 block
Tiempo: 45 969 910 microsegundos

*/

#include <iostream>
#include <chrono>
#include <random>
using namespace std;

const int MAX = 1000;

double A[MAX][MAX], x[MAX], y[MAX];
int blockSize = 200;

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

void segundo_tercer_ejercicio(){
    vector<vector<int>> A (MAX,vector<int>(MAX, 0));
    vector<vector<int>> B (MAX,vector<int>(MAX, 0));
    vector<vector<int>> C (MAX, vector<int>(MAX, 0));

    if (MAX % blockSize != 0){
        cout<<"Error: 'MAX' debe ser divisible por 'blockSize'"<<endl;
        return;
    }
    /*
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
    */

    //Tiempo
    auto start_time = chrono::high_resolution_clock::now();
    //Multiplicación clásica
    /*
    for (int i = 0; i < MAX; ++i) {
        for (int j = 0; j < MAX; ++j) {
            for (int k = 0; k < MAX; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    */
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Tiempo multipliacion clasica- "<<MAX<<": " << duration.count() << " microsegundos" << endl;

    //Multiplicación por bloques
    start_time = chrono::high_resolution_clock::now();
    for(int i0 = 0; i0 < MAX; i0 += blockSize){
        for(int j0 = 0; j0 < MAX; j0 += blockSize){
            for(int k0 = 0; k0 < MAX; k0 += blockSize){
                //Multiplicación
                for(int i = i0; i < min(i0 + blockSize, MAX); ++i){
                    for(int j = j0; j < min(j0 + blockSize, MAX); ++j){
                        for(int k = k0; k < min(k0 + blockSize, MAX); ++k){
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
        }
    }
    end_time = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Tiempo multiplicacion por bloques "<<blockSize<<" - "<<MAX<<": " << duration.count() << " microsegundos" << endl;

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
    segundo_tercer_ejercicio();

    return 0;
}
