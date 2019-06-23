#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InputBuffer.h"
#include "Util.h"
#include "Table.h"
#include "Run.h"

int main(int argc, char **argv) {
    char *s = malloc(sizeof(char) * 10000000);
    memset(s, 0, sizeof(char) * 10000000);
    char filename[100] = {};
    int off = read_run(s, filename);
    int save_out = 0;
    if (off != 0 && filename[0] != '\0') {
        freopen(filename, "a", stdout);
        save_out = 1;
    }
    s += off;
    if (!strncmp(s, ".exit", 5)) return 0;
    
    InputBuffer_t *input_buffer = new_InputBuffer(s);
    Command_t *cmd = new_Command();
    State_t *state = new_State();
    if(save_out) state->saved_stdout = 0;
    Table_t *table = NULL;
    int cmd_type;
    if (argc != 2) {
        table = new_Table(NULL);
    } else {
        table = new_Table(argv[1]);
    }
    if (table == NULL) {
        return 1;
    }
    for (;;) {
        read_input(input_buffer);
        cmd_type = parse_input(input_buffer->buffer, cmd);
        if (cmd_type == BUILT_IN_CMD) {
            handle_builtin_cmd(table, cmd, state);
        } else if (cmd_type == QUERY_CMD) {
            handle_query_cmd(table, cmd);
        } else if (cmd_type == UNRECOG_CMD) {
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);
        }
        cleanup_Command(cmd);
        clean_InputBuffer(input_buffer);
    }
    return 0;
}
