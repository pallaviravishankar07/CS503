#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"

extern void print_dragon();  // Declaration of the dragon function

int last_return_code = 0;  // Global variable to store the last return code

// Allocate memory for cmd_buff
int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    clear_cmd_buff(cmd_buff);
    return OK;
}

// Free memory for cmd_buff
int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    return OK;
}

// Clear/reset cmd_buff
int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    return OK;
}

// Parse input into cmd_buff
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    clear_cmd_buff(cmd_buff);
    
    // Skip leading spaces
    while (isspace(*cmd_line)) cmd_line++;
    
    if (*cmd_line == '\0') {
        return WARN_NO_CMDS;
    }

    // Copy command line to buffer
    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    
    char *token = cmd_buff->_cmd_buffer;
    bool in_quotes = false;
    
    // Parse arguments
    while (*token && cmd_buff->argc < CMD_ARGV_MAX - 1) {
        // Skip spaces between arguments
        while (isspace(*token) && !in_quotes) token++;
        
        if (*token == '\0') break;
        
        // Start of new argument
        cmd_buff->argv[cmd_buff->argc++] = token;
        
        // Process until end of argument
        while (*token) {
            if (*token == '"') {
                in_quotes = !in_quotes;
                // Remove quotes
                memmove(token, token + 1, strlen(token));
                continue;
            }
            if (isspace(*token) && !in_quotes) {
                *token = '\0';
                token++;
                break;
            }
            token++;
        }
    }
    
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

// Execute built-in commands
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], EXIT_CMD) == 0) {
        exit(0);
    }
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
                last_return_code = 1;
            } else {
                last_return_code = 0;
            }
        } else {
            last_return_code = 0;  // No argument, do nothing
        }
        return BI_EXECUTED;
    }
    if (strcmp(cmd->argv[0], "dragon") == 0) {
        print_dragon();
        last_return_code = 0;
        return BI_EXECUTED;
    }
    if (strcmp(cmd->argv[0], "rc") == 0) {
        printf("%d\n", last_return_code);
        last_return_code = 0;
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

// Main command loop
int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc;

    if (alloc_cmd_buff(&cmd) != OK) {
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove trailing newline
        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        rc = build_cmd_buff(cmd_line, &cmd);
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // Check for built-in commands
        if (exec_built_in_cmd(&cmd) == BI_EXECUTED) {
            continue;
        }

        // Fork and exec for external commands
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            last_return_code = 1;
            continue;
        }
        
        if (pid == 0) {
            // Child process
            execvp(cmd.argv[0], cmd.argv);
            if (errno == ENOENT) {
                fprintf(stderr, "Command not found in PATH\n");
                exit(127);
            } else if (errno == EACCES) {
                fprintf(stderr, "Permission denied\n");
                exit(126);
            } else {
                perror("execvp");
                exit(1);
            }
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                last_return_code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                last_return_code = 128 + WTERMSIG(status);
            } else {
                last_return_code = 1;
            }
        }
    }

    free_cmd_buff(&cmd);
    return OK;
}
