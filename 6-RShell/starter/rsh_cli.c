#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dshlib.h"
#include "rshlib.h"

// Function to clean up client resources
int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc) {
    if (cli_socket > 0) {
        close(cli_socket);
    }
    free(cmd_buff);
    free(rsp_buff);
    return rc;
}

int start_client(char *server_ip, int port) {
    int cli_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_socket < 0) {
        return ERR_RDSH_CLIENT;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(cli_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }

    return cli_socket;
}

int exec_remote_cmd_loop(char *address, int port) {
    int cli_socket = start_client(address, port);
    if (cli_socket < 0) {
        return cli_socket; // Return error code
    }

    char *cmd_buff = malloc(RDSH_COMM_BUFF_SZ);
    char *rsp_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!cmd_buff || !rsp_buff) {
        return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_MEMORY);
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, RDSH_COMM_BUFF_SZ, stdin) == NULL) {
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0'; // Remove trailing newline

        // Send command to server
        send(cli_socket, cmd_buff, strlen(cmd_buff) + 1, 0); // +1 for null terminator

        // Receive response from server
        int recv_size;
        while ((recv_size = recv(cli_socket, rsp_buff, RDSH_COMM_BUFF_SZ, 0)) > 0) {
            printf("%.*s", recv_size, rsp_buff);
            if (rsp_buff[recv_size - 1] == RDSH_EOF_CHAR) {
                break; // End of message
            }
        }
    }

    return client_cleanup(cli_socket, cmd_buff, rsp_buff, OK);
}
