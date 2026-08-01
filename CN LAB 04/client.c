#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock;

    struct sockaddr_in serv_addr;

    char reg_no[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket failed");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET,
              "127.0.0.1",
              &serv_addr.sin_addr);

    if (connect(sock,
                (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return 1;
    }

    printf("Connected to Server.\n");

    while (1)
    {
        printf("\nEnter Registration Number (bye to exit): ");

        fgets(reg_no, BUFFER_SIZE, stdin);

        reg_no[strcspn(reg_no, "\n")] = '\0';

        send(sock,
             reg_no,
             strlen(reg_no),
             0);

        if (strcmp(reg_no, "bye") == 0)
        {
            printf("Connection Closed.\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);

        recv(sock,
             buffer,
             BUFFER_SIZE,
             0);

        printf("\nServer Response:\n%s\n", buffer);
    }

    close(sock);

    return 0;
}