#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

#define BUFFER_SIZE 1024  // Größe eines einzelnen Pakets (1 KB)
#define DATA_SIZE (1024 * 1024)  // Gesamte zu sendende Datenmenge (1 MB)

typedef struct {
    char* name;
    char* address;
    int port;
} Target;

// Zeitmessung in Millisekunden
double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0;
}

// Funktion zur Messung der Transfer-Rate
// erstellen und konfigurieren der sockets bleibt gleich
double measure_transfer_rate(const char* address, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    double start_time, end_time;
    ssize_t bytes_sent = 0, bytes_received = 0;

    // Socket erstellen
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Server-Adresse konfigurieren
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        printf("Invalid address: %s\n", address);
        close(sockfd);
        return -1;
    }

    // Verbindung aufbauen
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed to %s: %s\n", address, strerror(errno));
        close(sockfd);
        return -1;
    }

    // Datenpuffer initialisieren
    memset(buffer, 'A', BUFFER_SIZE);  // Puffer mit 'A' füllen

    // Transfer starten und Zeit messen
    start_time = get_time_ms();

    // Daten senden
    while (bytes_sent < DATA_SIZE) {
        ssize_t sent = send(sockfd, buffer, BUFFER_SIZE, 0);
        if (sent <= 0) {
            printf("Send failed to %s\n", address);
            close(sockfd);
            return -1;
        }
        bytes_sent += sent;
    }

    // Daten empfangen
    while (bytes_received < DATA_SIZE) {
        ssize_t received = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (received <= 0) {
            printf("Receive failed from %s\n", address);
            close(sockfd);
            return -1;
        }
        bytes_received += received;
    }

    end_time = get_time_ms();
    close(sockfd);

    // Transfer-Rate berechnen (in Mbps)
    // wie in der Doku beschrieben berechnen wir die Rate
    double transfer_time = (end_time - start_time) / 1000.0;  // in Sekunden
    double transfer_rate = (DATA_SIZE * 8) / (transfer_time * 1e6);  // in Mbps

    return transfer_rate;
}

int main() {
    // Testziele definieren
    // an sich haben wir das Gleiche Prinzip wie bei der client_rtt.c
    // nur senden wir diesmal etwas größere Pakete und auch nicht wiederholt, sondern 
    // nur ein mal 
    Target targets[] = {
        {"Localhost", "127.0.0.1", 8080},
        {"Local Network", "10.0.2.2", 80},  // Anpassen an lokalen Router
        {"Google DE", "8.8.8.8", 53},       // Google DNS
        {"US Server", "104.16.123.96", 80}  // Beispiel US Server
    };
    
    int num_targets = sizeof(targets) / sizeof(Target);

    // Für jedes Ziel die Transfer-Rate messen
    for (int i = 0; i < num_targets; i++) {
        printf("\nTesting %s (%s):\n", targets[i].name, targets[i].address);
        printf("----------------------------------------\n");
        
        double transfer_rate = measure_transfer_rate(targets[i].address, targets[i].port);
        
        if (transfer_rate > 0) {
            printf("Transfer Rate for %s: %.3f Mbps\n", targets[i].name, transfer_rate);
        } else {
            printf("Transfer Rate measurement failed for %s\n", targets[i].name);
        }
    }

    return 0;
}
