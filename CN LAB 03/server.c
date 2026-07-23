#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    char *message;

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Define address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); 
    
    // 3. Bind socket
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  
    // 4. Listen for connections
    listen(server_fd, 3);

    printf("Server waiting on port %d...\n", PORT);
while(1){
    mainset(buffer,0,size);
    // 5. Accept connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    // 6. Read from client
    
    read(new_socket, buffer, 1024); 
    strcmp((buffer,"bye")==0);
    printf("Client: %s\n", buffer);

    // 7. Send response
    
    send(new_socket, message, strlen(message), 0);
}
    close(new_socket);
    close(server_fd);

    return 0; 
}

