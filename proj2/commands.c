#include "commands.h"
#include "connection.h"
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int connect_to_ip(const char * ip, unsigned port) {
    struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    // 32 bit Internet address network byte ordered
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        fprintf(stderr, "Error creating socket\n");
        return SOCKET_CREATE_ERROR;
    }

    // Open connection to the server
    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Error connecting to the given ip\n");
		return SOCKET_CONNECT_ERROR;
    }

    return socket_fd;
}

int read_command_reply(int socketfd, unsigned short* response_code, char** response_str, size_t * response_str_size) {    
    *response_str = calloc(RESPONSE_MAX_SIZE, sizeof(**response_str));

    if (*response_str == NULL) {
        fprintf(stderr, "Could not allocate buffer\n");
        return MALLOC_ERROR;
    }

    *response_str[0] = '\0';
    
    int socketfd_dup = dup(socketfd);

    if (socketfd_dup == -1) {
        return READ_CMD_ERROR;
    }

    FILE* socket_fileptr = fdopen(socketfd_dup, "r");

    if (socket_fileptr == NULL) {
        close(socketfd_dup);
        return READ_CMD_ERROR;
    }

    char* buf = NULL;
    size_t num_bytes = 0;
    *response_str_size = 0;
    while ((num_bytes = getline(&buf, &num_bytes, socket_fileptr)) >= 0) {
        strncat(*response_str, buf, num_bytes);
        response_str_size += num_bytes;

        // Last line in multi line responses have a space character after the code
        if (buf[CODE_SIZE] == ' ') {
            break;
        }
    }

    free(buf);
    fclose(socket_fileptr);

    if (num_bytes < 0) {
        fprintf(stderr, "Error reading command reply!\n");
        return READ_CMD_ERROR;
    }

    if (*response_str_size == 0) {
        fprintf(stderr, "No extra response was received\n");
    }

    char* code_str = strndup(*response_str, 3);
    *response_code = atoi(code_str);
    free(code_str);
    (*response_str)[RESPONSE_MAX_SIZE-1] = '\0';

    return 0;
}

int send_command(int socketfd, const char* command) {
    const size_t command_len = strlen(command);

    if (write(socketfd, command, command_len) != command_len) {
        fprintf(stderr, "Error sending command!\n");
        return SENDING_COMMAND_ERROR;
    }

    if (write(socketfd, COMMAND_TERMINATOR, COMMAND_TERMINATOR_SIZE) != COMMAND_TERMINATOR_SIZE) {
        fprintf(stderr, "Error sending command terminator!\n");
        return SENDING_COMMAND_ERROR;
    }

    return 0;
}