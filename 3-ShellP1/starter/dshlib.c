#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (cmd_line == NULL || clist == NULL) {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }

    // Trim leading and trailing spaces from the command line
    char *trimmed_cmd = cmd_line;
    while (isspace(*trimmed_cmd)) trimmed_cmd++;
    char *end = trimmed_cmd + strlen(trimmed_cmd) - 1;
    while (end > trimmed_cmd && isspace(*end)) end--;
    *(end + 1) = '\0';

    // If the command line is empty, return WARN_NO_CMDS
    if (strlen(trimmed_cmd) == 0) {
        return WARN_NO_CMDS;
    }

    // Split the command line into individual commands using '|'
    char *commands[CMD_MAX];
    int num_commands = 0;
    char *token = strtok(trimmed_cmd, PIPE_STRING);
    while (token != NULL) {
        // Trim leading and trailing spaces from each command
        while (isspace(*token)) token++;
        char *token_end = token + strlen(token) - 1;
        while (token_end > token && isspace(*token_end)) token_end--;
        *(token_end + 1) = '\0';

        // Check if the number of commands exceeds CMD_MAX
        if (num_commands >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        commands[num_commands++] = token;
        token = strtok(NULL, PIPE_STRING);
    }

    // Parse each command into executable and arguments
    for (int i = 0; i < num_commands; i++) {
        char *cmd = commands[i];
        char *exe = strtok(cmd, " ");
        char *args = strtok(NULL, "");

        if (exe == NULL) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // Copy the executable name into the command structure
        if (strlen(exe) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strncpy(clist->commands[i].exe, exe, EXE_MAX);

        // Copy the arguments into the command structure
        if (args != NULL) {
            if (strlen(args) >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strncpy(clist->commands[i].args, args, ARG_MAX);
        } else {
            clist->commands[i].args[0] = '\0';
        }
    }

    clist->num = num_commands;
    return OK;
}
