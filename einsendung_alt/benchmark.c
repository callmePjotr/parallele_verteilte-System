#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000 // Matrixgröße (anpassbar)

// Funktion zum Erstellen einer zufälligen Matrix
void fill_matrix(double* matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i * size + j] = ((double)rand() / RAND_MAX);
        }
    }
}

// Funktion für die Matrixmultiplikation
void matrix_multiply(double* A, double* B, double* C, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0.0;
            for (int k = 0; k < size; k++) {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = sum;
        }
    }
}

int main() {
    // Zufälligen Seed für die Matrixinhalte setzen
    srand((unsigned int)time(NULL));

    // Speicher für die Matrizen allokieren
    double* A = (double*)malloc(N * N * sizeof(double));
    double* B = (double*)malloc(N * N * sizeof(double));
    double* C = (double*)malloc(N * N * sizeof(double));

    if (!A || !B || !C) {
        printf("Speicherallokierung fehlgeschlagen.\n");
        return 1;
    }

    // Matrizen initialisieren
    fill_matrix(A, N);
    fill_matrix(B, N);

    // Zeitmessung starten
    clock_t start_time = clock();

    // Matrixmultiplikation durchführen
    matrix_multiply(A, B, C, N);

    // Zeitmessung stoppen
    clock_t end_time = clock();

    // Berechnete Zeit in Sekunden
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // FLOPs berechnen: 2 * N^3 Operationen
    double num_operations = 2.0 * N * N * N;
    double gflops = num_operations / elapsed_time / 1e9;

    // Ergebnisse ausgeben
    printf("Matrixgröße: %dx%d\n", N, N);
    printf("Elapsed Time: %.6f seconds\n", elapsed_time);
    printf("GFLOPS (REAL(64)): %.2f\n", gflops);

    // Speicher freigeben
    free(A);
    free(B);
    free(C);

    return 0;
}
