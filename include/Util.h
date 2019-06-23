#ifndef DB_UTIL_H
#define DB_UTIL_H
#include "Command.h"
#include "Table.h"
#include "InputBuffer.h"

typedef struct State {
    int saved_stdout;
} State_t;

State_t* new_State();
void print_prompt(State_t *state, InputBuffer_t *t);
void print_user(User_t *user, SelectArgs_t *sel_args);
void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd);
void print_aggr(Table_t* table, Command_t *t);
int parse_input(char *input, Command_t *cmd);
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state);
int handle_query_cmd(Table_t *table, Command_t *cmd);
int handle_insert_cmd(Table_t *table, Command_t *cmd);
int handle_select_cmd(Table_t *table, Command_t *cmd);
void handle_delete_cmd(Table_t *table, Command_t *cmd);
void handle_update_cmd(Table_t *table, Command_t *cmd);
void update_users(Table_t* table, Command_t *cmd);
int check_condition(User_t *user, WhereArgs_t* where_args);
void print_help_msg();
int _check_condition_str(char* val, WhereArgs_t* w, int idx);
int _check_condition_num(int val, WhereArgs_t* w, int idx);
void print_likes(Table_t *table, Command_t *cmd);

#endif
