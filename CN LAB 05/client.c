#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    // Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connected to server! Type 'ANALYZE <text>' or 'bye' to exit.\n\n");

    while (1) {
        printf("Enter command: ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // Strip trailing newline
        input[strcspn(input, "\r\n")] = 0;

        // Send input to server
        send(sock, input, strlen(input), 0);

        // Break if user entered 'bye'
        if (strcmp(input, "bye") == 0) {
            printf("Disconnecting...\n");
            break;
        }

        // Receive response from server
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            printf("Response: %s\n\n", buffer);
        } else {
            printf("Server disconnected.\n");
            break;
        }
    }

    close(sock);
    return 0;
}