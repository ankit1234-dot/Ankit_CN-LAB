#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {

    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect
    if (connect(sock, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {

        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("Connected to server.\n");

    while (1) {

        printf("Client : ");

        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        // Send message
        send(sock, buffer, strlen(buffer), 0);

        // Exit if bye
        if (strcmp(buffer, "bye") == 0)
            break;

        memset(buffer, 0, BUFFER_SIZE);

        // Receive reply
        recv(sock, buffer, BUFFER_SIZE, 0);

        printf("Server : %s\n", buffer);
    }

    close(sock);

    return 0;
}