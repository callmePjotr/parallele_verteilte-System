#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024 * 1024  // 1 MB

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char *buffer = malloc(BUFFER_SIZE); // 1 MB
    struct timespec start, end;
    long long elapsed_time;

    // Socket erstellen
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(PORT);

    // Verbinden zum Server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    // Fülle den Puffer mit zufälligen Daten
    memset(buffer, 'A', BUFFER_SIZE);

    clock_gettime(CLOCK_MONOTONIC, &start);  // Startzeit messen
    send(sockfd, buffer, BUFFER_SIZE, 0);    // Daten senden
    recv(sockfd, buffer, BUFFER_SIZE, 0);    // Antwort empfangen
    clock_gettime(CLOCK_MONOTONIC, &end);    // Endzeit messen

    // Transferrate berechnen (Bytes pro Sekunde)
    elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    double transfer_rate = (double)BUFFER_SIZE / elapsed_time * 1e9;  // Bytes pro Sekunde
    printf("Transfer Rate: %.2f bytes per second\n", transfer_rate);

    close(sockfd);
    free(buffer);
    return 0;
}