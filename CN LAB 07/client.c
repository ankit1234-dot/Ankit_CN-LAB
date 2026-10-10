// Roll: 24105142019 Service Type: B d = 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 30019
#define BUFFER_SIZE 2048

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <roll>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *server_ip = argv[1];
    const char *roll = argv[2];

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid IPv4 address: %s\n", server_ip);
        close(sockfd);
        return EXIT_FAILURE;
    }

    char input[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    printf("UDP client started. Enter SUM|n|numbers, MAX|n|numbers, MIN|n|numbers, AVG|n|numbers, or EXIT.\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            // EOF: send exit request and terminate.
            snprintf(message, sizeof(message), "%s#EXIT", roll);
            sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&server_addr, sizeof(server_addr));
            // Wait briefly for the server's acknowledgment.
            struct timeval timeout = {2, 0};
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            ssize_t n = recvfrom(sockfd, response, sizeof(response) - 1, 0, NULL, NULL);
            if (n > 0) {
                response[n] = '\0';
                printf("Server: %s\n", response);
            }
            break;
        }

        input[strcspn(input, "\r\n")] = '\0';
        if (input[0] == '\0') continue;

        int is_exit = (strcmp(input, "EXIT") == 0);
        snprintf(message, sizeof(message), "%s#%s", roll, input);

        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto");
            continue;
        }

        ssize_t n = recvfrom(sockfd, response, sizeof(response) - 1, 0, NULL, NULL);
        if (n < 0) {
            perror("recvfrom");
            break;
        }
        response[n] = '\0';
        printf("Server: %s\n", response);

        if (is_exit) break;
    }

    close(sockfd);
    return 0;
}
