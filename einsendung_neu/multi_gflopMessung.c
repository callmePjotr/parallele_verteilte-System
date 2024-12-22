#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>  // OpenMP-Bibliothek zur parallelen Verarbeitung, wird dem linker mit -fopenmp übergeben

// Matrixgröße (N x N)
#define N 2048  
#define BLOCK_SIZE 32  // Blockgröße, hier werden die Matrizen in Blöcken multipliziert

// die altebekannte Zeitmessung
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL); 
    return tv.tv_sec + tv.tv_usec * 1e-6; 
}

// Parallele Matrixmultiplikation mit Block-Ansatz
void matrix_multiply_parallel(double *A, double *B, double *C, int n) {
    // **Schritt 1: Initialisiere die Ergebnismatrix C mit Nullen**
    // OpenMP-Direktive für parallele Schleifen. Die `collapse(2)`-Option fusioniert die Schleifen `i` und `j` für bessere Parallelität.
    #pragma omp parallel for collapse(2)
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            C[i * n + j] = 0.0;  // und wieder die Matrix mit Nullen füllen
        }
    }

    // **Schritt 2: Blockweise Matrixmultiplikation**
    // Die äußeren Schleifen teilen die Matrizen in Blöcke der Größe BLOCK_SIZE x BLOCK_SIZE auf.
    #pragma omp parallel for collapse(2)
    for(int ii = 0; ii < n; ii += BLOCK_SIZE) {  // Zeilen
        for(int jj = 0; jj < n; jj += BLOCK_SIZE) {  //Spalten
            for(int kk = 0; kk < n; kk += BLOCK_SIZE) {  // Blockstart in der gemeinsamen Dimension
                // Schleifen für Elemente innerhalb eines Blocks
                for(int i = ii; i < ii + BLOCK_SIZE && i < n; i++) {  // Begrenzung durch Blockgröße und Matrixgröße
                    for(int j = jj; j < jj + BLOCK_SIZE && j < n; j++) {
                        double sum = C[i * n + j];  // Zwischenspeicher für die Summe
                        for(int k = kk; k < kk + BLOCK_SIZE && k < n; k++) {
                            sum += A[i * n + k] * B[k * n + j];  // Berechnung des Produkts und Aufsummieren
                        }
                        C[i * n + j] = sum;  // Speichere das Ergebnis
                    }
                }
            }
        }
    }
}

int main() {
    double start_time, end_time, total_time;  
    double gflops; 

    
    omp_set_num_threads(4);  // Setzt die maximale Anzahl der Threads auf 4
    printf("Anzahl verwendeter Threads: %d\n", omp_get_max_threads()); 

    // optimierte Allozierung des Speichers für die oben festgelegte Blockgröße 32
    double *A = (double*)aligned_alloc(32, N * N * sizeof(double));
    double *B = (double*)aligned_alloc(32, N * N * sizeof(double));
    double *C = (double*)aligned_alloc(32, N * N * sizeof(double));


    // OpenMP-Direktive für parallele Initialisierung
    #pragma omp parallel for
    for(int i = 0; i < N * N; i++) {
        A[i] = (double)rand() / RAND_MAX;  // zufäälige Werte in die MAtritzen schreiben, wieder 0 bis 1
        B[i] = (double)rand() / RAND_MAX;
    }

    matrix_multiply_parallel(A, B, C, N);


    const int NUM_RUNS = 3;  // Anzahl der Durchläufe
    start_time = get_time(); 

    for(int run = 0; run < NUM_RUNS; run++) {
        matrix_multiply_parallel(A, B, C, N);  // Wiederhole die Multiplikation für NUM_RUNS
    }

    end_time = get_time();
    total_time = (end_time - start_time) / NUM_RUNS;  // Durchschnittliche Laufzeit über alle Durchläufe

    // **Berechnung der GFLOPS**
    gflops = (2.0 * N * N * N) / (total_time * 1e9);


    printf("Matrix-Größe: %d x %d\n", N, N);
    printf("Block-Größe: %d\n", BLOCK_SIZE);
    printf("Durchschnittliche Zeit pro Durchlauf: %.2f Sekunden\n", total_time);
    printf("Performance: %.2f GFLOPS\n", gflops);
    printf("Performance pro Kern: %.2f GFLOPS\n", gflops / 4);


    free(A);  
    free(B); 
    free(C);  

    return 0;
}
