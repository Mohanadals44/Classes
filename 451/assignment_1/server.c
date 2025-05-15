#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 50000
#define BUFFER_SIZE 1024

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("Client (%s:%d) Connected to the server successfully\n", 
           client_ip, ntohs(client_addr.sin_port));
    
    char buffer[BUFFER_SIZE];
    int bytes_read;
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_read = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0) { 
            break; 
        }
        buffer[bytes_read] = '\0';
        buffer[strcspn(buffer, "\n")] = '\0';        
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
        
        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe");
            continue;
        }
        
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }
        if (pid == 0) {
            // Child process: execute command.
            close(fd[0]);  
            dup2(fd[1], STDOUT_FILENO);
            dup2(fd[1], STDERR_FILENO);
            close(fd[1]);
            
            char *args[100];
            int i = 0;
            char *token = strtok(buffer, " ");
            while (token != NULL && i < 99) {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;
            
            if (execvp(args[0], args) < 0) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        } else {
            close(fd[1]);  
            char out[BUFFER_SIZE];
            int n;
            while ((n = read(fd[0], out, BUFFER_SIZE)) > 0) {
                send(client_sock, out, n, 0);
            }
            close(fd[0]);
            wait(NULL);
            send(client_sock, " ", 7, 0);
        }
    }
    close(client_sock);
    exit(0);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    // Create TCP socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Allow reuse of the address.
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind the socket to the specified port.
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections.
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    // Accept connections and fork a child for each client.
    while (1) {
        client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }
        
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_sock);
            continue;
        }
        if (pid == 0) {
            close(server_fd);
            handle_client(client_sock, client_addr);
        } else {
            close(client_sock);
        }
    }
    
    close(server_fd);
    return 0;
}
