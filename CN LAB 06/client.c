#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_in server_addr;

    char file_data[BUFFER_SIZE];
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];

   
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

  
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    
    if (inet_pton(AF_INET, "127.0.0.1",
                  &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        exit(1);
    }

    
    if (connect(sock,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(1);
    }

    printf("Connected to server.\n");

    
    FILE *fp = fopen("data.txt", "r");

    if (fp == NULL)
    {
        perror("Unable to open data.txt");
        close(sock);
        exit(1);
    }

   
    int len = fread(file_data, 1,
                    sizeof(file_data) - 1, fp);

    file_data[len] = '\0';

    fclose(fp);

  
    int student_id;

    printf("Enter Student ID to search : ");
    scanf("%d", &student_id);

    
    sprintf(request, "%d\n%s",
            student_id, file_data);

    write(sock, request, strlen(request));

    
    int bytes_read = read(sock, response,
                          sizeof(response) - 1);

    if (bytes_read > 0)
    {
        response[bytes_read] = '\0';

        printf("Server Response : %s\n",
               response);
    }

    
    close(sock);

    return 0;
}