#include "DeleteState.h"
#include "WhereState.h"
#include <stdlib.h>
#include <string.h>

void delete_state_handler(Command_t *cmd, size_t arg_idx) {
    arg_idx += 2;
    if(cmd->args_len == arg_idx) return;
    if(!strcmp(cmd->args[arg_idx], "where")) {
        where_state_handler(cmd, arg_idx+1);
        return;
    }
    cmd->type = UNRECOG_CMD;
    return;
}
