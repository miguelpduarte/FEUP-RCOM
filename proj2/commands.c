#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>


int read_command_reply(int socketfd, unsigned short* response_code, char** response_str, size_t * response_str_size) {    
    char code_str[CODE_SIZE];
    *response_str = calloc(RESPONSE_MAX_SIZE, sizeof(**response_str));

    if (*response_str == NULL) {
        fprintf(stderr, "Could not allocate buffer\n");
        return MALLOC_ERROR;
    }

    if (read(socketfd, code_str, CODE_SIZE * sizeof(*code_str)) < CODE_SIZE) {
        fprintf(stderr, "Error reading command reply code!\n");
        return READ_CMD_ERROR;
    }

    *response_code = atoi(code_str);
    *response_str_size = read(socketfd, *response_str, RESPONSE_MAX_SIZE * sizeof(**response_str));

    if (*response_str_size == -1) {
        fprintf(stderr, "Error reading extra response\n");
        return ERROR_READING_EXTRA_RESPONSE;
    }

    if (*response_str_size == 0) {
        fprintf(stderr, "No extra response was received\n");
    }

    (*response_str)[RESPONSE_MAX_SIZE-1] = '\0';

    return 0;
}

int send_command(int socketfd, const char* command) {
    const size_t command_len = strlen(command);

    if (write(socketfd, command, command_len) != command_len) {
        fprintf(stderr, "Error sending command!\n");
        return SENDING_COMMAND_ERROR;
    }

    if (write(socketfd, "\n", 1) != 1) {
        fprintf(stderr, "Error sending command terminator!\n");
        return SENDING_COMMAND_ERROR;
    }

    return 0;
}