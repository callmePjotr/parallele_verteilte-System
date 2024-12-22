#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

// Funktion zur Initialisierung einer Matrix mit zufälligen Werten
void initialize_matrix(vector<vector<double>>& matrix, int size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
}

// Funktion zur Matrixmultiplikation
void multiply_matrices(const vector<vector<double>>& A,
                       const vector<vector<double>>& B,
                       vector<vector<double>>& C,
                       int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            C[i][j] = 0.0;
            for (int k = 0; k < size; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    // Matrixgröße definieren
    const int N = 1000; // Reduziere die Größe, wenn der Speicher begrenzt ist

    // Matrizen erstellen
    vector<vector<double>> A(N, vector<double>(N));
    vector<vector<double>> B(N, vector<double>(N));
    vector<vector<double>> C(N, vector<double>(N));

    // Zufällige Werte initialisieren
    initialize_matrix(A, N);
    initialize_matrix(B, N);

    // Zeitmessung starten
    auto start_time = chrono::high_resolution_clock::now();

    // Matrixmultiplikation durchführen
    multiply_matrices(A, B, C, N);

    // Zeitmessung stoppen
    auto end_time = chrono::high_resolution_clock::now();

    // FLOPs berechnen: 2 * N^3 Operationen für Matrix-Multiplikation
    double num_operations = 2.0 * N * N * N;
    chrono::duration<double> elapsed_time = end_time - start_time;

    // GFLOPS (10^9 FLOPs)
    double gflops = num_operations / elapsed_time.count() / 1e9;

    // Ergebnisse ausgeben
    cout << "Matrixgröße: " << N << "x" << N << endl;
    cout << "Elapsed Time: " << elapsed_time.count() << " seconds" << endl;
    cout << "GFLOPS (REAL(64)): " << gflops << endl;

    return 0;
}
