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

// Declare the external function from dragon.c
extern void print_dragon();

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

// Build command list from input line
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    char *saveptr;
    char *command = strtok_r(cmd_line, PIPE_STRING, &saveptr);

    while (command != NULL && clist->num < CMD_MAX) {
        cmd_buff_t *cmd_buff = &clist->commands[clist->num];
        if (alloc_cmd_buff(cmd_buff) != OK) {
            return ERR_MEMORY;
        }
        if (build_cmd_buff(command, cmd_buff) == OK) {
            clist->num++;
        }
        command = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    if (clist->num >= CMD_MAX) {
        return ERR_TOO_MANY_COMMANDS;
    }

    return OK;
}

// Execute built-in commands
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
            }
        }
        return BI_EXECUTED;
    }
    if (strcmp(cmd->argv[0], "dragon") == 0) {
        print_dragon();
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

// Main command loop
int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    command_list_t cmd_list;
    int rc;

    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove trailing newline
        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        rc = build_cmd_list(cmd_line, &cmd_list);
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // Execute the pipeline of commands
        execute_pipeline(&cmd_list);
    }

    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipe_fds[2 * (num_cmds - 1)];
    pid_t pids[num_cmds];

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipe_fds + i * 2) < 0) {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        // Check for built-in commands
        if (exec_built_in_cmd(&clist->commands[i]) == BI_EXECUTED) {
            continue;
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return ERR_MEMORY;
        }

        if (pids[i] == 0) {
            // Child process
            if (i > 0) {
                // Not the first command, get input from the previous pipe
                dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < num_cmds - 1) {
                // Not the last command, output to the next pipe
                dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                close(pipe_fds[j]);
            }

            // Handle redirection
            for (int j = 0; j < clist->commands[i].argc; j++) {
                if (strcmp(clist->commands[i].argv[j], "<") == 0) {
                    int fd = open(clist->commands[i].argv[j + 1], O_RDONLY);
                    if (fd < 0) {
                        perror("open");
                        exit(ERR_EXEC_CMD);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    clist->commands[i].argv[j] = NULL;
                    clist->commands[i].argv[j + 1] = NULL;
                } else if (strcmp(clist->commands[i].argv[j], ">") == 0) {
                    int fd = open(clist->commands[i].argv[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open");
                        exit(ERR_EXEC_CMD);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    clist->commands[i].argv[j] = NULL;
                    clist->commands[i].argv[j + 1] = NULL;
                } else if (strcmp(clist->commands[i].argv[j], ">>") == 0) {
                    int fd = open(clist->commands[i].argv[j + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd < 0) {
                        perror("open");
                        exit(ERR_EXEC_CMD);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    clist->commands[i].argv[j] = NULL;
                    clist->commands[i].argv[j + 1] = NULL;
                }
            }

            // Remove NULL entries from argv
            int k = 0;
            for (int j = 0; j < clist->commands[i].argc; j++) {
                if (clist->commands[i].argv[j] != NULL) {
                    clist->commands[i].argv[k++] = clist->commands[i].argv[j];
                }
            }
            clist->commands[i].argv[k] = NULL;

            // Execute the command
            cmd_buff_t *cmd = &clist->commands[i];
            if (execvp(cmd->argv[0], cmd->argv) < 0) {
                perror("execvp");
                exit(ERR_EXEC_CMD);
            }
        }
    }

    // Parent process: close all pipe file descriptors
    for (int i = 0; i < 2 * (num_cmds - 1); i++) {
        close(pipe_fds[i]);
    }

    // Wait for all child processes
    for (int i = 0; i < num_cmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return OK;
}
