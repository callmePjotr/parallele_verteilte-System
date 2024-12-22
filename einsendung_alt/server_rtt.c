#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[1024];

    // Socket erstellen
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Note the htonl() conversion
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Server listening on port %d\n", PORT);

    // Accept und empfangen
    client_fd = accept(server_fd, (struct sockaddr*)&address, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        return -1;
    }

    while (1) {
        // Empfangen
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }
        // Antwort senden
        send(client_fd, buffer, bytes_received, 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}