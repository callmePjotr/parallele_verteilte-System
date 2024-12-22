#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Matrix-Größe definieren
#define N 1024

// Funktion zur Zeitmessung in Sekunden
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);  // Ruft die aktuelle Zeit ab
    return tv.tv_sec + tv.tv_usec * 1e-6;  // Gibt die Zeit in Sekunden
}

int main() {
    int i, j, k;
    double start_time, end_time, total_time;
    double gflops;
    
    // Dynamische Speicherzuweisung für Matrizen A, B und C
    // Speicherung der Matrizen erfolgt im Heap!
    double **A = (double**)malloc(N * sizeof(double*));  
    double **B = (double**)malloc(N * sizeof(double*));  
    double **C = (double**)malloc(N * sizeof(double*));  
    
    // Für jede Zeile der Matrizen A, B, und C wird der Speicher für die Spalten zugewiesen
    // Es wird ein Array von Pointern erzeugt, wobei jede Zeile einen eigenen Speicherbereich für die Spalten hat.
    for(i = 0; i < N; i++) {
        A[i] = (double*)malloc(N * sizeof(double));
        B[i] = (double*)malloc(N * sizeof(double));
        C[i] = (double*)malloc(N * sizeof(double));
    }
    
    // Füllen der Matrizen mit zufälligen Werten im Bereich [0, 1]
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            A[i][j] = (double)rand() / RAND_MAX;
            B[i][j] = (double)rand() / RAND_MAX;
            C[i][j] = 0.0;  // Initialisiere Matrix C mit 0 (Ergebnismatrix)
        }
    }
    
    // Start der Zeitmessung, bevor die Matrix-Multiplikation beginnt
    start_time = get_time();
    
    // Matrix-Multiplikation: C = A * B
    // zwei äußere Schleifen für Zeilen und Spalten, innere Schleife führt Addition aus
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            for(k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    // Ende der Zeitmessung nach der Matrix-Multiplikation
    end_time = get_time();
    total_time = end_time - start_time;
    
    // Berechnung der GFLOPS
    // Eine Matrix-Multiplikation besteht aus 2 * N^3 Operationen:
    // - N^3 Multiplikationen und N^3 Additionen.
    // Da eine Multiplikation und eine Addition pro innerer Schleife durchgeführt werden, 
    // haben wir insgesamt 2 * N^3 Operationen.
    // Um die FLOPS zu berechnen, teilen wir die Anzahl der Operationen durch die benötigte Zeit (in Sekunden).
    gflops = (2.0 * N * N * N) / (total_time * 1e9);  // 1e9 konvertiert in GFLOPS (Giga FLOPS)


    printf("Matrix-Größe: %d x %d\n", N, N);
    printf("Benötigte Zeit: %.2f Sekunden\n", total_time);
    printf("Performance: %.2f GFLOPS\n", gflops);
    
    // und wieder alles freigeben
    for(i = 0; i < N; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    
    return 0;
}
