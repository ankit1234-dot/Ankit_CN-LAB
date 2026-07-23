#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_PORT 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message;
    char buffer[BUFFER_PORT];

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Define server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    while(1){
     // 3. Connect to server
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // 4. Send message
  
    send(sock, message, strlen(message), 0);

    // 5. Receive response
    read(sock, buffer, 1024);
    
    printf("Server: %s\n", buffer);
    }
    close(sock);

    return 0;
}



   
