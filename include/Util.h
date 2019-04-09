#ifndef DB_UTIL_H
#define DB_UTIL_H
#include <stdlib.h>
#include "Command.h"
#include "Table.h"
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct State {
    int saved_stdout;
} State_t;

State_t *new_State();
void print_prompt(State_t *state);
void print_user(User_t *user, char **fields, size_t fields_cnt);
void print_user_field(User_t *user, size_t idx);
int parse_input(char *input, Command_t *cmd);
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state);
int handle_query_cmd(Table_t *table, Command_t *cmd);
int handle_insert_cmd(Table_t *table, Command_t *cmd);
int handle_select_cmd(Table_t *table, Command_t *cmd);
void print_help_msg();

#endif
