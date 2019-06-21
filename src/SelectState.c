#include <string.h>
#include <stdlib.h>
#include "Command.h"
#include "SelectState.h"
#include "WhereState.h"

void field_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.sel_args.fields = NULL;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
    while(arg_idx < cmd->args_len) {
        char *s = cmd->args[arg_idx];
        int len = strlen(s);
        Aggr_t aggr = NO_AGGR;
        if (!strncmp(s, "count(", 6) && s[len - 1] == ')') {
            s[len - 1] = '\0';
            s += 6;
            aggr = COUNT;
        } else if (!strncmp(s, "avg(", 4) && s[len - 1] == ')') {
            s[len - 1] = '\0';
            s += 4;
            aggr = AVG;
        } else if (!strncmp(s, "sum(", 4) && s[len - 1] == ')') {
            s[len - 1] = '\0';
            s += 4;
            aggr = SUM;
        }
        if (!strncmp(s, "*", 1)) {
            add_select_field(cmd, s, aggr);
        } else if (!strncmp(s, "id", 2)) {
            add_select_field(cmd, s, aggr);
        } else if (!strncmp(s, "name", 4)) {
            add_select_field(cmd, s, aggr);
        } else if (!strncmp(s, "email", 5)) {
            add_select_field(cmd, s, aggr);
        } else if (!strncmp(s, "age", 3)) {
            add_select_field(cmd, s, aggr);
        } else if (!strncmp(s, "from", 4)) {
            table_state_handler(cmd, arg_idx+1);
            return;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void table_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len
            && !strncmp(cmd->args[arg_idx], "user", 5)) {

        arg_idx++;
        if (arg_idx == cmd->args_len) {
            return;
        } else if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
            offset_state_handler(cmd, arg_idx+1);
            return;
        } else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
            limit_state_handler(cmd, arg_idx+1);
            return;
        } else if (!strncmp(cmd->args[arg_idx], "where", 5)) {
            where_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void offset_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {

            limit_state_handler(cmd, arg_idx+1);
            return;
        }  else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "where", 5)) {

            where_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "where", 5)) {

            where_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}
