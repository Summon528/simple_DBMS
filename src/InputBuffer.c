#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InputBuffer.h"

///
/// Allocate InputBuffer_t and initialize some attributes
/// Return: ptr of InputBuffer_t
///
InputBuffer_t* new_InputBuffer(char *s) {
    InputBuffer_t *input_buffer = (InputBuffer_t*) malloc(sizeof(InputBuffer_t));
    input_buffer->buffer = NULL;
    input_buffer->buffer_len = 0;
    input_buffer->input_len = 0;
    input_buffer->input_ptr = s;
    return input_buffer;
}

///
/// Read the input from stdin, then store into InputBuffer_t
/// TODO: accept different input source
///
void read_input(InputBuffer_t *input_buffer) {
    ssize_t input_len = 0;
    if (*input_buffer->input_ptr == EOF) {
        exit(1);
    }

    char *start = input_buffer->input_ptr;
    while (*input_buffer->input_ptr != '\n' && *input_buffer->input_ptr != EOF && *input_buffer->input_ptr != '\0') {
        input_buffer->input_ptr++;
        input_len++;
    }
    input_buffer->input_ptr++;
    input_buffer->buffer_len = input_len;
    input_buffer->buffer = (char *) realloc(input_buffer->buffer, sizeof(char) * input_buffer->buffer_len + 1);
    strncpy(input_buffer->buffer, start, sizeof(char) * input_len);
    input_buffer->buffer[input_len] = '\0';

    input_buffer->input_len = input_len;
}

///
/// Free the allocated buffer to store input string
///
void clean_InputBuffer(InputBuffer_t *input_buffer) {
    free(input_buffer->buffer);
    input_buffer->buffer = NULL;
    input_buffer->buffer_len = 0;
    input_buffer->input_len = 0;
}
