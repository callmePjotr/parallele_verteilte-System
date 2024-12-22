// rtt_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

/*
- eigentlich ist dieses Programm ziemlich selbserklärend
- es handelt sich halt um einen Echo Server, der über einen Socket funktoniert
- dieser läuft so lange bis er beendet wird
- und sendet empfangene Pakete an den Sender zurück
- interessanter hingegen sind die beiden clients
*/

#define PORT 8080
#define BUFFER_SIZE 1024

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE];
    int opt = 1;

    // Signal Handler für sauberes Beenden
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Socket erstellen
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Socket-Optionen setzen (für schnelles Wiederverwenden des Ports)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    // Adresse vorbereiten
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    // Listen
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("Listen failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d\n", PORT);

    while (keep_running) {
        // Accept
        client_fd = accept(server_fd, (struct sockaddr*)&address, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;  // Signal unterbrach accept
            perror("Accept failed");
            break;
        }

        while (keep_running) {
            ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received <= 0) {
                if (bytes_received < 0 && errno == EINTR) continue;
                break;  // Client geschlossen oder Fehler
            }

            buffer[bytes_received] = '\0';
            // Echo zurück
            if (send(client_fd, buffer, bytes_received, 0) < 0) {
                perror("Send failed");
                break;
            }
        }

        close(client_fd);
    }

    close(server_fd);
    printf("\nServer beendet.\n");
    return EXIT_SUCCESS;
}