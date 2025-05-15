Running the Server and Client with the Makefile

Compiling the Server and Client: Use the provided Makefile to compile both the server and client programs.

'make'

Cleaning Up: To remove the compiled server and client binaries, use:

make clean

Running the Server: Start the server on a terminal by running:

./server

Running the Client: In another terminal, run the client with the server's IP address as an argument:

./client <server_ip>

Server (server.c) - High Overview

handle_client(int client_sock, struct sockaddr_in client_addr):

This function manages communication with a client. It listens for commands sent from the client and executes them on the server. If a command is entered by the client, the server tries to execute it using execvp() and sends the output back. It also handles the "quit" command to terminate the connection.

main():

Initializes the server, binds to the specified port, listens for incoming client connections, and forks a child process to handle each client connection. Each client is processed independently by the handle_client() function.

Client (client.c) - High Overview

main(int argc, char *argv[]):

This function connects the client to the server using the server's IP address and port. It prompts the user to enter commands, sends them to the server, and displays the server's output. The client sends commands in a loop until the "quit" command is entered, at which point it closes the connection.

Key Concepts in Both Files

Server-Side: The server listens for incoming client connections on a specified port (50000). It forks a child process to handle each client independently, ensuring that multiple clients can interact with the server simultaneously. The server uses execvp() to execute the commands sent by the client and pipes the output back to the client.

Client-Side: The client connects to the server using the provided server IP and port. It sends commands to the server and receives the output to display on the terminal. The client allows continuous communication until the "quit" command is entered.

Both files involve socket programming for network communication between the server and client using TCP/IP.

