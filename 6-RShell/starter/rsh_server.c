#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dshlib.h"
#include "rshlib.h"

// Function to send EOF character to the client
int send_message_eof(int cli_socket) {
    int bytes_sent = send(cli_socket, &RDSH_EOF_CHAR, 1, 0);
    if (bytes_sent == 1) {
        return OK; // Successfully sent EOF
    }
    return ERR_RDSH_COMMUNICATION; // Error sending EOF
}

// Function to boot the server
int boot_server(char *ifaces, int port) {
    int svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0) {
        return ERR_RDSH_COMMUNICATION;
    }

    int enable = 1;
    setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ifaces);
    server_addr.sin_port = htons(port);

    if (bind(svr_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    if (listen(svr_socket, 5) < 0) {
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    return svr_socket;
}

// Function to execute client requests
int exec_client_requests(int cli_socket) {
    char *io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!io_buff) {
        return ERR_RDSH_COMMUNICATION;
    }

    while (1) {
        int recv_size = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);
        if (recv_size <= 0) {
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        }

        io_buff[recv_size - 1] = '\0'; // Null terminate the command
        printf("Received command: %s\n", io_buff);

        // Here you would execute the command and send back the result
        // For simplicity, we will just echo the command back
        send(cli_socket, io_buff, strlen(io_buff), 0);
        send_message_eof(cli_socket); // Send EOF character
    }

    free(io_buff);
    return OK;
}

// Function to process client requests
int process_cli_requests(int svr_socket) {
    while (1) {
        int cli_socket = accept(svr_socket, NULL, NULL);
        if (cli_socket < 0) {
            return ERR_RDSH_COMMUNICATION;
        }

        exec_client_requests(cli_socket);
        close(cli_socket); // Close the client socket after handling
    }
}

// Function to start the server
int start_server(char *ifaces, int port, int is_threaded) {
    (void)is_threaded; // Marking the parameter as unused

    int svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0) {
        return svr_socket; // Return error code
    }

    return process_cli_requests(svr_socket);
}
