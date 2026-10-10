// Roll: 24105142019 Service Type: B d = 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 30019
#define BUFFER_SIZE 2048
#define ROLL "24105142019"

static void send_reply(int sockfd, const char *reply,
                       const struct sockaddr_in *client, socklen_t client_len) {
    sendto(sockfd, reply, strlen(reply), 0,
           (const struct sockaddr *)client, client_len);
}

static void process_request(char *request, char *reply, size_t reply_size) {
    char *op, *n_text, *numbers_text, *saveptr;
    char *hash = strchr(request, '#');

    if (hash == NULL) {
        snprintf(reply, reply_size, "ERROR: Invalid message format");
        return;
    }

    *hash = '\0';
    if (strcmp(request, ROLL) != 0) {
        snprintf(reply, reply_size, "ERROR: Unauthorized client");
        return;
    }

    char *payload = hash + 1;
    if (strcmp(payload, "EXIT") == 0) {
        snprintf(reply, reply_size, "EXIT");
        return;
    }

    // Parse OP|n|a1 a2 ... an
    op = strtok_r(payload, "|", &saveptr);
    n_text = strtok_r(NULL, "|", &saveptr);
    numbers_text = strtok_r(NULL, "|", &saveptr);

    if (op == NULL || n_text == NULL || numbers_text == NULL) {
        snprintf(reply, reply_size, "ERROR: Expected OP|n|integer list");
        return;
    }

    char *endptr;
    errno = 0;
    long n_long = strtol(n_text, &endptr, 10);
    if (errno != 0 || *n_text == '\0' || *endptr != '\0' ||
        n_long < 1 || n_long > 500) {
        snprintf(reply, reply_size, "ERROR: n must be between 1 and 500");
        return;
    }
    int n = (int)n_long;

    long long sum = 0;
    long long maximum = LLONG_MIN;
    long long minimum = LLONG_MAX;
    int count = 0;

    char *token = strtok_r(numbers_text, " \t\r\n", &saveptr);
    while (token != NULL) {
        errno = 0;
        long long value = strtoll(token, &endptr, 10);
        if (errno != 0 || *token == '\0' || *endptr != '\0') {
            snprintf(reply, reply_size, "ERROR: Invalid integer in list");
            return;
        }
        if (count >= n) {
            snprintf(reply, reply_size, "ERROR: More integers supplied than n");
            return;
        }
        sum += value;
        if (value > maximum) maximum = value;
        if (value < minimum) minimum = value;
        count++;
        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }

    if (count != n) {
        snprintf(reply, reply_size, "ERROR: Expected %d integers, received %d", n, count);
        return;
    }

    if (strcmp(op, "SUM") == 0) {
        snprintf(reply, reply_size, "%lld", sum);
    } else if (strcmp(op, "MAX") == 0) {
        snprintf(reply, reply_size, "%lld", maximum);
    } else if (strcmp(op, "MIN") == 0) {
        snprintf(reply, reply_size, "%lld", minimum);
    } else if (strcmp(op, "AVG") == 0) {
        snprintf(reply, reply_size, "%.2f", (double)sum / n);
    } else {
        snprintf(reply, reply_size, "ERROR: Unsupported operation. Use SUM, MAX, MIN, or AVG");
    }
}

int main(void) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("Server running on port %d\n", PORT);
    printf("Service Type B: Integer List Processor\n");

    for (;;) {
        char buffer[BUFFER_SIZE];
        char reply[BUFFER_SIZE];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        ssize_t received = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&client_addr, &client_len);
        if (received < 0) {
            perror("recvfrom");
            continue;
        }
        buffer[received] = '\0';

        char logged_message[BUFFER_SIZE];
        snprintf(logged_message, sizeof(logged_message), "%s", buffer);
        char *hash = strchr(logged_message, '#');
        if (hash != NULL) memmove(logged_message, hash + 1, strlen(hash + 1) + 1);

        printf("\nClient address: %s:%u\n",
               inet_ntoa(client_addr.sin_addr), (unsigned)ntohs(client_addr.sin_port));
        printf("Received message: %s\n", logged_message);

        process_request(buffer, reply, sizeof(reply));
        send_reply(sockfd, reply, &client_addr, client_len);
        printf("Sent response: %s\n", reply);

        if (strcmp(reply, "EXIT") == 0) {
            printf("Exit command received. Shutting down server.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
