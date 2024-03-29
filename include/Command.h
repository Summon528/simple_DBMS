#ifndef COMMAND_H
#define COMMAND_H
#include <stddef.h>

enum { 
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD,
    UPDATE_CMD,
    DELETE_CMD
};

typedef enum Comp { EQ, NEQ, GT, LE, GTQ, LEQ } Comp_t;
typedef enum Type { _double, _int, _string } Type_t;
typedef enum Logic { AND, OR } Logic_t;
typedef enum Aggr { SUM, AVG, COUNT, NO_AGGR } Aggr_t;
typedef enum Tabl { USER, LIKE } Tabl_t;

typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct SelectArgs {
    char **fields;
    size_t fields_len;
    int offset;
    int limit;
    Aggr_t *aggrs;
    Tabl_t tabl;
} SelectArgs_t;

typedef union IDC {
    int ival;
    double dval;
    char* sval;
} IDC_t;

typedef struct WhereArgs {
    char l_operand[2][16];
    IDC_t r_operand[2];
    Comp_t comp[2];
    Type_t type[2];
    int len;
    Logic_t logic;
} WhereArgs_t;

typedef struct JoinArgs {
    char l_operand[16];
    char r_operand[16];
    int used;
} JoinArgs_t;

typedef struct UpdateArgs {
    char field[16];
    int ival;
    char* sval;
} UpdateArgs_t;

typedef union {
    SelectArgs_t sel_args;
    UpdateArgs_t update_args;
} CmdArg_t;

typedef struct Command {
    unsigned char type;
    char **args;
    size_t args_len;
    size_t args_cap;
    CmdArg_t cmd_args;
    WhereArgs_t where_args;
    JoinArgs_t join_args;
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *argument, Aggr_t aggr);
void cleanup_Command(Command_t *cmd);

#endif
