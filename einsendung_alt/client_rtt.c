#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib") // Verlinken der WinSock-Bibliothek

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    DWORD start, end;  // Für Zeitmessung in Millisekunden
    DWORD elapsed_time;

    // WinSock initialisieren
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return -1;
    }

    // Socket erstellen
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return -1;
    }

    // Server-Adresse konfigurieren (Loopback)
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Loopback-Adresse
    server_addr.sin_port = htons(PORT);

    // Mit dem Server verbinden
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    // Nachricht senden
    strcpy(buffer, "Ping");

    // Startzeit messen
    start = GetTickCount();

    // Nachricht senden
    send(sockfd, buffer, strlen(buffer), 0);

    // Antwort empfangen
    recv(sockfd, buffer, BUFFER_SIZE, 0);

    // Endzeit messen
    end = GetTickCount();

    // Latenz berechnen (in Millisekunden)
    elapsed_time = end - start;
    printf("Round Trip Time (RTT): %lu milliseconds\n", elapsed_time);

    // Socket und WinSock bereinigen
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
