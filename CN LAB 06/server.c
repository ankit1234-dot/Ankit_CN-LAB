#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

   
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    
    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

   
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server started...\n");
    printf("Waiting for clients on port %d...\n", PORT);

    
    while (1)
    {
        client_fd = accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &client_len);

        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

      
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            close(client_fd);
            continue;
        }

      
        if (pid == 0)
        {
            char buffer[BUFFER_SIZE];
            int student_id;
            int found = 0;

            close(server_fd);

            
            int bytes_read = read(client_fd, buffer,
                                  sizeof(buffer) - 1);

            if (bytes_read <= 0)
            {
                close(client_fd);
                exit(0);
            }

            buffer[bytes_read] = '\0';

           
            sscanf(buffer, "%d", &student_id);

          
            char *line = strtok(buffer, "\n");

            while (line != NULL)
            {
                int id;
                char name[50];
                char branch[50];

                if (sscanf(line, "%d %s %s",
                           &id, name, branch) == 3)
                {
                    if (id == student_id)
                    {
                        char response[BUFFER_SIZE];

                        sprintf(response,
                                "Record Found : %d %s %s",
                                id, name, branch);

                        write(client_fd, response,
                              strlen(response));

                        found = 1;
                        break;
                    }
                }

                line = strtok(NULL, "\n");
            }

            
            if (!found)
            {
                char response[] = "Record Not Found";

                write(client_fd, response,
                      strlen(response));
            }

            close(client_fd);
            exit(0);
        }

        
        else
        {
            close(client_fd);
        }
    }

    close(server_fd);

    return 0;
}