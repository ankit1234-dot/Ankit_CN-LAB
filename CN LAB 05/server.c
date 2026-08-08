#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

// Helper function to count words in a string
int count_words(char *str) {
    int count = 0, in_word = 0;
    while (*str) {
        if (isspace(*str)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        str++;
    }
    return count;
}

// Helper function to count vowels in a string
int count_vowels(char *str) {
    int count = 0;
    while (*str) {
        char ch = tolower(*str);
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
            count++;
        }
        str++;
    }
    return count;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Create Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow address reuse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Initialize address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) break;

        // Remove newline character if present
        buffer[strcspn(buffer, "\r\n")] = 0;

        // Exit if client sends "bye"
        if (strcmp(buffer, "bye") == 0) {
            printf("Client sent 'bye'. Closing connection.\n");
            break;
        }

        char command[20] = {0};
        char text[BUFFER_SIZE] = {0};

        // Parse command and text (e.g., "ANALYZE Hello World")
        sscanf(buffer, "%s %[^\n]", command, text);

        if (strcmp(command, "ANALYZE") == 0) {
            int chars = strlen(text);
            int words = count_words(text);
            int vowels = count_vowels(text);

            char response[BUFFER_SIZE];
            sprintf(response, "Chars=%d, Words=%d, Vowels=%d", chars, words, vowels);
            send(new_socket, response, strlen(response), 0);
        } else {
            char *msg = "Invalid Command";
            send(new_socket, msg, strlen(msg), 0);
        }
    }

    close(new_socket);
    close(server_fd);
    printf("Server shut down.\n");
    return 0;
}