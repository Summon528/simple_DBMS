#include "UpdateState.h"
#include "WhereState.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

void update_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        if(strcmp(cmd->args[arg_idx], "user")) goto error;
        arg_idx++;
        if(strcmp(cmd->args[arg_idx], "set")) goto error;
        arg_idx++;
        char *s = cmd->args[arg_idx];
        if (!strncmp(s, "id", 2)) {
            strcpy(cmd->cmd_args.update_args.field, "id");
            s += 2;
        } else if (!strncmp(s, "name", 4)) {
            strcpy(cmd->cmd_args.update_args.field, "name");
            s += 4;
        } else if (!strncmp(s, "email", 5)) {
            strcpy(cmd->cmd_args.update_args.field, "email");
            s += 5;
        } else if (!strncmp(s, "age", 3)) {
            strcpy(cmd->cmd_args.update_args.field, "age");
            s += 3;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }

        if(*s == '\0') {
            arg_idx++;
            if (arg_idx >= cmd->args_len) goto error;
            s = cmd->args[arg_idx];
        }

        if(*s != '=') goto error;
        s++;

        if(*s == '\0') {
            arg_idx++;
            if (arg_idx >= cmd->args_len) goto error;
            s = cmd->args[arg_idx];
        }

        if(*s == '\"') {
            cmd->cmd_args.update_args.sval = strdup(s);
        } else {
            cmd->cmd_args.update_args.ival = atoi(s);
        }

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "where", 5)) {
            where_state_handler(cmd, arg_idx+1);
            return;
        }
    }
error:
    cmd->type = UNRECOG_CMD;
    return;
}
