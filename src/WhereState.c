#include "WhereState.h"
#include "SelectState.h"
#include <stdlib.h>
#include <string.h>

void where_state_handler(Command_t *cmd, size_t arg_idx) {
    for(int comp_idx = 0; comp_idx < 2; comp_idx++) {
        char *s  = cmd->args[arg_idx];
        if (arg_idx < cmd->args_len) {
            if (!strncmp(s, "id", 2)) {
                s += 2;
                strcpy(cmd->where_args.l_operand[comp_idx], "id");
            } else if (!strncmp(s, "name", 4)) {
                s += 4;
                strcpy(cmd->where_args.l_operand[comp_idx], "name");
            } else if (!strncmp(s, "email", 5)) {
                s += 5;
                strcpy(cmd->where_args.l_operand[comp_idx], "email");
            } else if (!strncmp(s, "age", 3)) {
                s += 3;
                strcpy(cmd->where_args.l_operand[comp_idx], "age");
            } else {
                goto error;
            }

            if(*s == '\0') {
                arg_idx++;
                if (arg_idx >= cmd->args_len) goto error;
                s = cmd->args[arg_idx];
            }

            if (!strncmp(s, "!=", 2)) {
                s += 2;
                cmd->where_args.comp[comp_idx] = NEQ;
            } else if (!strncmp(s, ">=", 2)) {
                s += 2;
                cmd->where_args.comp[comp_idx] = GTQ;
            } else if (!strncmp(s, "<=", 2)) {
                s += 2;
                cmd->where_args.comp[comp_idx] = LEQ;
            } else if (!strncmp(s, "=", 1)) {
                s += 1;
                cmd->where_args.comp[comp_idx] = EQ;
            } else if (!strncmp(s, ">", 1)) {
                s += 1;
                cmd->where_args.comp[comp_idx] = GT;
            } else if (!strncmp(s, "<", 1)) {
                s += 1;
                cmd->where_args.comp[comp_idx] = LE;
            } else {
                goto error;
            }

            if(*s == '\0') {
                arg_idx++;
                if (arg_idx >= cmd->args_len) goto error;
                s = cmd->args[arg_idx];
            }

            if(*s == '\"') {
                cmd->where_args.r_operand[comp_idx].sval = strdup(s);
                cmd->where_args.type[comp_idx] = _string;
            } else if (strchr(s, '.') != NULL) {
                cmd->where_args.r_operand[comp_idx].dval = atof(s);
                cmd->where_args.type[comp_idx] = _double;
            } else {
                cmd->where_args.r_operand[comp_idx].ival = atoi(s);
                cmd->where_args.type[comp_idx] = _int;
            }

            arg_idx++;
            cmd->where_args.len++;
            
            if(arg_idx == cmd->args_len) {
                return;
            } else if (arg_idx < cmd->args_len && !strcmp(cmd->args[arg_idx], "offset")) {
                offset_state_handler(cmd, arg_idx+1);
                return;
            } else if (arg_idx < cmd->args_len && !strcmp(cmd->args[arg_idx], "limit")) {
                limit_state_handler(cmd, arg_idx+1);
                return;
            }
            if(comp_idx == 0) {
                if(!strcmp(cmd->args[arg_idx], "and")) {
                    cmd->where_args.logic = AND;
                } else if(!strcmp(cmd->args[arg_idx], "or")) {
                    cmd->where_args.logic = OR;
                } else {
                    goto error;
                }
                arg_idx++;
            }
        }
    }

error:
    cmd->type = UNRECOG_CMD;
    return;
}
