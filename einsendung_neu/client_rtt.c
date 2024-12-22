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

#define BUFFER_SIZE 1024
#define NUM_PINGS 10  // Anzahl der Messungen pro Ziel

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

// hier ist einfach das Gegenstück zum Server
// wir senden kleine Datenmengen und warten auf eine Antwort
// RTT für ein einzelnes Ziel messen
double measure_rtt(const char* address, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    double start_time, end_time;

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

    // Timeout für connect setzen
    struct timeval tv;
    tv.tv_sec = 5;  // 5 Sekunden Timeout
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);

    // Verbindung aufbauen
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed to %s: %s\n", address, strerror(errno));
        close(sockfd);
        return -1;
    }

    strncpy(buffer, "Ping", BUFFER_SIZE - 1);
    buffer[BUFFER_SIZE - 1] = '\0';

    start_time = get_time_ms();
    
    // nachdem der Socket erstellt und konfiguriert wurde, können wir Pakete senden
    // wir starten erst nach Erstellung und Konfiguration die Zeitmessung, um die Zeit des Overheads
    // nicht im Ergebnis zu haben
    if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
        printf("Send failed to %s\n", address);
        close(sockfd);
        return -1;
    }

    if (recv(sockfd, buffer, BUFFER_SIZE - 1, 0) < 0) {
        printf("Receive failed from %s\n", address);
        close(sockfd);
        return -1;
    }
    // wir warten auf die Antwort des Echo-Servers und messen die vergangene Zeit zwischen
    // senden und Empfangen
    end_time = get_time_ms();
    close(sockfd);

    return end_time - start_time;
}

int main() {
    // Testziele definieren
    Target targets[] = {
        {"Localhost", "127.0.0.1", 8080},
        {"Local Network", "10.0.2.2", 80},     // Anpassen an lokalen Router
        {"Google DE", "8.8.8.8", 53},             // Google DNS
        {"US Server", "104.16.123.96", 80}        // Beispiel US Server
    };
    
    int num_targets = sizeof(targets) / sizeof(Target);

    // Für jedes Ziel mehrere Messungen durchführen
    // für jedes ziel werden 10 Messungen erstellt
    // jede Iteration wird measure_rtt() erneut aufgerufen
    for (int i = 0; i < num_targets; i++) {
        printf("\nTesting %s (%s):\n", targets[i].name, targets[i].address);
        printf("----------------------------------------\n");
        
        double min_rtt = 999999, max_rtt = 0, avg_rtt = 0;
        int successful_pings = 0;

        for (int j = 0; j < NUM_PINGS; j++) {
            double rtt = measure_rtt(targets[i].address, targets[i].port);
            
            // Berechnung von Durchschnittszeit, Minimum und Maximum
            // hochzählen der erfolgreichen Pings
            if (rtt > 0) {
                successful_pings++;
                avg_rtt += rtt;
                if (rtt < min_rtt) min_rtt = rtt;
                if (rtt > max_rtt) max_rtt = rtt;
                printf("Ping %d: %.3f ms\n", j + 1, rtt);
            }
            
            sleep(1);  // 1 Sekunde Pause zwischen Pings
        }

        if (successful_pings > 0) {
            avg_rtt /= successful_pings;
            printf("\nStatistics for %s:\n", targets[i].name);
            printf("  Minimum = %.3f ms\n", min_rtt);
            printf("  Maximum = %.3f ms\n", max_rtt);
            printf("  Average = %.3f ms\n", avg_rtt);
            printf("  Success rate: %d/%d\n", successful_pings, NUM_PINGS);
        } else {
            printf("\nNo successful pings to %s\n", targets[i].name);
        }
    }

    return 0;
}
