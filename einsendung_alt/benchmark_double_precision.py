import time
import numpy as np

# np.__config__.show()
# Matrixgröße definieren
N = 20000  # Matrixgröße

# Zufällige Matrizen mit doppelter Genauigkeit erstellen
A = np.random.rand(N, N).astype(np.float64)
B = np.random.rand(N, N).astype(np.float64)

# Zeitmessung starten
start_time = time.time()

# Matrixmultiplikation durchführen
C = np.dot(A, B)

# Zeitmessung stoppen
end_time = time.time()

# FLOPs berechnen: 2 * N^3 Operationen für Matrix-Multiplikation
num_operations = 2 * (N ** 3)
elapsed_time = end_time - start_time

# GFLOPS (10^9 FLOPs)
gflops = num_operations / elapsed_time / 1e9

print(f"Matrixgröße: {N}x{N}")
print(f"Elapsed Time: {elapsed_time:.6f} seconds")
print(f"GFLOPS (REAL(64)): {gflops:.2f}")