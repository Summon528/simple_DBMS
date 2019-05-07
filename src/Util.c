#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"
#include "UpdateState.h"
#include "DeleteState.h"

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}

///
/// Print the users for given offset and limit restriction
///
void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;
    User_t *usr_ptr;

    if (offset == -1) {
        offset = 0;
    }

    if (idxList) {
        for (idx = offset; idx < idxListLen; idx++) {
            if (limit != -1 && (idx - offset) >= limit) {
                break;
            }
            usr_ptr = get_User(table, idxList[idx]);
            if(check_condition(usr_ptr, &(cmd->where_args))) {
                print_user(usr_ptr, &(cmd->cmd_args.sel_args));
            }
        }
    } else {
        for (idx = offset; idx < table->len; idx++) {
            if (limit != -1 && (idx - offset) >= limit) {
                break;
            }
            usr_ptr = get_User(table, idx);
            if(check_condition(usr_ptr, &(cmd->where_args))) {
                print_user(usr_ptr, &(cmd->cmd_args.sel_args));
            }
        }
    }
}

int check_condition(User_t* usr_ptr, WhereArgs_t* w) {
    if (w->len == 0) return 1;
    int result[2];
    for (int i = 0; i < w->len; i++) {
        if (!strncmp(w->l_operand[i], "id", 2)) {
            result[i] = _check_condition_num(usr_ptr->id, w, i);
        } else if (!strncmp(w->l_operand[i], "name", 4)) {
            result[i] = _check_condition_str(usr_ptr->name, w, i);
        } else if (!strncmp(w->l_operand[i], "email", 5)) {
            result[i] = _check_condition_str(usr_ptr->email, w, i);
        } else if (!strncmp(w->l_operand[i], "age", 3)) {
            result[i] = _check_condition_num(usr_ptr->age, w, i);
        }
    }
    if (w->len == 1) return result[0];
    if (w->len == 2 && w->logic == AND) return result[0] && result[1];
    if (w->len == 2 && w->logic == OR) return result[0] || result[1];
    return 0;
}

int _check_condition_num(int val, WhereArgs_t* w, int idx) {
    switch (w->type[idx]) {
        case _string:
            return 0;
        case _int:
            switch (w->comp[idx]) {
                case LEQ:
                    return val <= w->r_operand[idx].ival;
                case LE:
                    return val < w->r_operand[idx].ival;
                case GT:
                    return val > w->r_operand[idx].ival;
                case GTQ:
                    return val >= w->r_operand[idx].ival;
                case EQ:
                    return val == w->r_operand[idx].ival;
                case NEQ:
                    return val != w->r_operand[idx].ival;
            }
        case _double:
            switch (w->comp[idx]) {
                case LEQ:
                    return val <= w->r_operand[idx].dval;
                case LE:
                    return val < w->r_operand[idx].dval;
                case GT:
                    return val > w->r_operand[idx].dval;
                case GTQ:
                    return val >= w->r_operand[idx].dval;
                case EQ:
                    return val == w->r_operand[idx].dval;
                case NEQ:
                    return val != w->r_operand[idx].dval;
            }
    }
    return 0;
}

int _check_condition_str(char* val, WhereArgs_t* w, int idx) {
    int cmp_r;
    switch (w->type[idx]) {
        case _string:
            cmp_r = strcmp(val, w->r_operand[idx].sval);
            switch (w->comp[idx]) {
                case EQ:
                    return !cmp_r;
                case NEQ:
                    return cmp_r;
                default:
                    return 0;
            }
        default:
            return 0;
    }
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table, cmd);
        return SELECT_CMD;
    } else if (!strncmp(cmd->args[0], "update", 6)) {
        handle_update_cmd(table, cmd);
        return UPDATE_CMD;
    } else if (!strncmp(cmd->args[0], "delete", 6)) {
        handle_delete_cmd(table, cmd);
        return DELETE_CMD;
    } else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);
    if (cmd->cmd_args.sel_args.aggrs[0] == NO_AGGR) {
        print_users(table, NULL, 0, cmd);
    } else {
        print_aggr(table, cmd);
    }
    return table->len;
}

void handle_update_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = UPDATE_CMD;
    update_state_handler(cmd, 1);
    update_users(table, cmd);
}

void update_users(Table_t* table, Command_t *cmd) {
    User_t** users = (User_t**)malloc(sizeof(User_t*) * table->len);
    int udx = 0, id_exist = 0;
    for (int i = 0; i < table->len; i++) {
        User_t* usr_ptr = get_User(table, i);
        if(usr_ptr->id == cmd->cmd_args.update_args.ival) id_exist = 1;
        if(check_condition(usr_ptr, &(cmd->where_args)))
            users[udx++] = usr_ptr;
    }
    char* field = cmd->cmd_args.update_args.field;
    if (!strncmp(field, "id", 2) && udx == 1 && !id_exist) {
        users[0]->id = cmd->cmd_args.update_args.ival;
    } else if (!strncmp(field, "name", 4)) {
        for (int i = 0; i < udx; i++)
            strncpy(users[i]->name, cmd->cmd_args.update_args.sval, MAX_USER_NAME);
    } else if (!strncmp(field, "email", 5)) {
        for (int i = 0; i < udx; i++)
            strncpy(users[i]->email, cmd->cmd_args.update_args.sval, MAX_USER_EMAIL);
    } else if (!strncmp(field, "age", 3)) {
        for (int i = 0; i < udx; i++)
            users[i]->age = cmd->cmd_args.update_args.ival;
    }
    free(users);
}

void handle_delete_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = DELETE_CMD;
    delete_state_handler(cmd, 1);
    int old_idx = 0, new_idx = 0, len = table->len;
    for (;old_idx < len; old_idx++) {
        if(!check_condition(get_User(table, old_idx), &(cmd->where_args))) {
            if(old_idx != new_idx) {
                memmove(table->users + new_idx, table->users + old_idx, sizeof(User_t));
                memmove(table->cache_map + new_idx, table->cache_map + old_idx, sizeof(unsigned char));
            }
            new_idx++;
        } else {
            table->len--;
        }    
    }

    for (;new_idx < len; new_idx++) {
        memset(table->users + new_idx, 0, sizeof(User_t));
        memset(table->cache_map + new_idx, 0, sizeof(unsigned char));
    }
}

void print_aggr(Table_t* table, Command_t *t) {
    if (t->cmd_args.sel_args.offset > 0) return;
    if (t->cmd_args.sel_args.limit == 0) return;
    printf("(");
    for (int i = 0; i < t->cmd_args.sel_args.fields_len; i++) {
        if(t->cmd_args.sel_args.aggrs[i] == NO_AGGR) continue;
        int sun = 0, cnt = 0;
        for (int j = 0; j < table->len; j++) {
            User_t* usr_ptr = get_User(table, j);
            if(!check_condition(usr_ptr, &(t->where_args))) continue;
            cnt++;
            if (!strncmp(t->cmd_args.sel_args.fields[i], "id", 2)) {
                sun += usr_ptr->id;
            } else if (!strncmp(t->cmd_args.sel_args.fields[i], "age", 3)) {
                sun += usr_ptr->age;
            }
        }
        if (i != 0) printf(", ");
        switch (t->cmd_args.sel_args.aggrs[i]) {
            case AVG:
                if (cnt != 0) {
                    printf("%.3lf", (double)sun / cnt);
                } else {
                    printf("%.3lf", 0.0);
                }
                break;
            case COUNT:
                printf("%d", cnt);
                break;
            case SUM:
                printf("%d", sun);
                break;
            default:
                break;
        }
    }
    printf(")\n");
}

///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}
