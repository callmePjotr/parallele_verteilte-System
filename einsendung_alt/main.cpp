#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_OPERATIONS 10000000000000000000  // Anzahl der Operationen
#define NUM_THREADS 16            // Anzahl der Threads (entspricht der Anzahl der Kerne)

typedef struct {
    long start;
    long end;
    double result;
} ThreadData;

// Funktion, die von jedem Thread ausgeführt wird
void* perform_operations(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    double a = 1.01, b = 1.01, local_result = 0.0;

    for (long i = data->start; i < data->end; i++) {
        local_result += a * b;
    }

    data->result = local_result;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    long operations_per_thread = NUM_OPERATIONS / NUM_THREADS;
    double total_result = 0.0;
    clock_t start, end;

    // Startzeit erfassen
    start = clock();

    // Threads erstellen
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = i * operations_per_thread;
        thread_data[i].end = (i + 1) * operations_per_thread;
        thread_data[i].result = 0.0;
        pthread_create(&threads[i], NULL, perform_operations, &thread_data[i]);
    }

    // Threads beenden
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_result += thread_data[i].result;  // Ergebnisse aufsummieren
    }

    // Endzeit erfassen
    end = clock();

    // Zeit in Sekunden berechnen
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;

    // FLOPS berechnen
    double flops = NUM_OPERATIONS / elapsed_time;

    printf("Ergebnisse:\n");
    printf("Dauer: %f Sekunden\n", elapsed_time);
    printf("FLOPS: %e\n", flops);

    return 0;
}