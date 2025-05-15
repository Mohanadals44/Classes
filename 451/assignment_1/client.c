#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 50000
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int sock;
    struct sockaddr_in server_addr;
    
    // Create the client socket.
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Set server address structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    
    // Connect to the server.
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to the server (%s:%d) successfully\n", argv[1], PORT);
    
    char command[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    int bytes_read;
    
    while (1) {
        printf("Enter command (or quit to exit): ");
        if (fgets(command, BUFFER_SIZE, stdin) == NULL)
            break;
        // Remove trailing newline.
        command[strcspn(command, "\n")] = '\0';
        
        send(sock, command, strlen(command), 0);
        
        if (strcmp(command, "quit") == 0) {
            break;
        }
        
        // Receive and display server output.
        memset(output, 0, BUFFER_SIZE);
        while ((bytes_read = read(sock, output, BUFFER_SIZE - 1)) > 0) {
            output[bytes_read] = '\0';
            printf("%s", output);
            if (strstr(output, " ") != NULL) {
                break;  
            }
        }
        printf("\n");
    }
    
    close(sock);
    return 0;
}
