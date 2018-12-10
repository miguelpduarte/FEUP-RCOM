#include "connection.h"
#include "commands.h"
#include "parser.h"
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


static int login(int command_socketfd, const char* user, const char* password);
static void close_connection(int command_socketfd);
static int change_directory(int command_socketfd, const char* path);
static int set_binary_mode(int command_socketfd);
static int set_passive_mode(int command_socketfd, int* data_socketfd);
static int request_file(int command_socketfd, const char * file);
static int read_retrieve_final_response(int command_socketfd);
static int read_initial_response(int command_socketfd);


int hostname_to_ip(const char* hostname, char** ip) {
    struct hostent * h;

    if ((h = gethostbyname(hostname)) == NULL) {
        fprintf(stderr, "Could not translate hostname %s to an IP address\n", hostname);
        return HOSTNAME_TRANSLATION_ERROR;
    }

    char * temp_ip = inet_ntoa(*((struct in_addr *) h->h_addr_list[0]));
    *ip = strndup(temp_ip, strlen(temp_ip));

    return 0;
}

int transfer_file(const char* user, const char* password, const char* ip, const char* path, const char* file) {
    int command_socketfd = connect_to_ip(ip, FTP_CONTROL_PORT);
    
    if (command_socketfd < 0) {
        fprintf(stderr, "Error creating command socket\n");
        return SOCKET_ERROR;
    }

    // Read initial response
    if (read_initial_response(command_socketfd) != 0) {
        fprintf(stderr, "Failed to read initial response!\n");
        return READ_INITIAL_RESPONSE_ERROR;        
    }

    // Login to the server
    if (login(command_socketfd, user, password) != 0) {
        fprintf(stderr, "Login failed!\n");
        return LOGIN_ERROR;
    }

    // Change directory to the desired one
    if (change_directory(command_socketfd, path) != 0) {
        fprintf(stderr, "Change directory failed!\n");
        return CHANGE_DIR_ERROR;
    }

    // Change to binary mode
    if (set_binary_mode(command_socketfd) != 0) {
        fprintf(stderr, "Set binary mode failed!\n");
        return SET_BINARY_MODE_ERROR;        
    }

    // Enter passive mode
    int data_socketfd;
    if (set_passive_mode(command_socketfd, &data_socketfd) != 0) {
        fprintf(stderr, "Set passive mode failed!\n");
        return SET_PASISVE_MODE_ERROR;        
    }

    // Requesting file and reading initial response
    if (request_file(command_socketfd, file) != 0) {
        fprintf(stderr, "Failure in requesting file!\n");
        return REQUEST_FILE_FAILED;
    }

    // Downloading file
    if (copy_file(data_socketfd, file) != 0) {
        fprintf(stderr, "Failure in downloading file!\n");
        return DOWNLOAD_FILE_FAILED;
    }

    // Reading retrieve final response
    if (read_retrieve_final_response(command_socketfd) != 0) {
        fprintf(stderr, "Error reading retrieve final response!\n");
        return RETRIEVE_FINAL_RESPONSE_FAILED;
    }

    // Close connection
    close_connection(command_socketfd);

    // Close socket
    close(command_socketfd);
    close(data_socketfd);

    return 0;
}

static int login(int command_socketfd, const char* user, const char* password) {
    if (strncmp(user, "", 1) == 0) {
        // Unauthenticated server, no login necessary
        return 0;
    }

    char* username_cmd = malloc((strlen(user) + strlen(USER) + 2) * sizeof(*username_cmd));
    char* password_cmd = malloc((strlen(password) + strlen(PASS) + 2) * sizeof(*password_cmd));

    if (username_cmd == NULL || password_cmd == NULL) {
        free(username_cmd); 
        free(password_cmd);
        return MALLOC_ERROR;
    }

    // Building Username Command    
    username_cmd[0] = '\0';
    strcat(username_cmd, USER);
    strcat(username_cmd, " ");
    strcat(username_cmd, user);

    // Building Password Command
    password_cmd[0] = '\0';
    strcat(password_cmd, PASS);
    strcat(password_cmd, " ");
    strcat(password_cmd, password);

    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    // Send Username
    if (send_command(command_socketfd, username_cmd) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", username_cmd);
        free(username_cmd);
        free(password_cmd);
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", username_cmd);
    }
    free(username_cmd);

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read user command response\n");
        free(response);
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }

    if (response_code != USER_SUCCESS_CODE) {
        fprintf(stderr, "Login failed (user)\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return LOGIN_ERROR;
    }

    // Because we are reusing variables
    free(response);
    response = NULL;

    // Send Password
    if (send_command(command_socketfd, password_cmd) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", password_cmd);
        free(password_cmd);
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s ****\n", PASS);
    }
    free(password_cmd);

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read pass command response\n");
        free(response);
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }
    
    if (response_code != PASS_SUCCESS_CODE) {
        fprintf(stderr, "Login failed (pass)\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return LOGIN_ERROR;
    }

    free(response);
    return 0;
}

static void close_connection(int command_socketfd) {
    if (send_command(command_socketfd, QUIT) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", QUIT);
        return;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", QUIT);
    }

    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read quit command response\n");
        free(response);
        return;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }

    if (response_code != QUIT_SUCCESS_CODE) {
        fprintf(stderr, "Quit failed\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return;
    }

    free(response);
}

static int change_directory(int command_socketfd, const char* path) {
    char* change_dir_command = malloc((strlen(path) + strlen(CWD) + 2) * sizeof(*change_dir_command));
    
    if (change_dir_command == NULL) {
        return MALLOC_ERROR;
    }

    // Building Username Command
    change_dir_command[0] = '\0';
    strcat(change_dir_command, CWD);
    strcat(change_dir_command, " ");
    strcat(change_dir_command, path);

    if (send_command(command_socketfd, change_dir_command) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", change_dir_command);
        free(change_dir_command);
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", change_dir_command);
    }
    free(change_dir_command);

    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read cwd command response\n");
        free(response);
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }

    if (response_code != CWD_SUCCESS_CODE) {
        fprintf(stderr, "CWD failed\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return CWD_ERROR;
    }

    free(response);
    return 0;
}

static int set_binary_mode(int command_socketfd) {
    if (send_command(command_socketfd, TYPE_BINARY) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", TYPE_BINARY);
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", TYPE_BINARY);
    }

    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read set binary mode command response\n");
        free(response);
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }

    if (response_code != TYPE_SUCCESS_CODE) {
        fprintf(stderr, "Set binary mode failed\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return INVALID_RESPONSE;
    }

    free(response);
    return 0;
}

static int set_passive_mode(int command_socketfd, int* data_socketfd) {
    if (send_command(command_socketfd, PASV) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", PASV);
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", PASV);
    }

    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read set passive mode command response\n");
        free(response);
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }

    if (response_code != PASV_SUCCESS_CODE) {
        fprintf(stderr, "Set passive mode failed\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return INVALID_RESPONSE;
    }

    char* ip_pasv = NULL;
    unsigned port_pasv;

    if (parsePASV(response, &ip_pasv, &port_pasv) != 0) {
        fprintf(stderr, "Failed to parse passive mode response\n");
        free(response);
        return PARSE_PASV_FAILED;
    }

    free(response);

    // Connecting to the data socket so that the process can resume
    if ((*data_socketfd = connect_to_ip(ip_pasv, port_pasv)) < 0) {
        fprintf(stderr, "Could not open connection to the data port\n");
        free(ip_pasv);
    }

    free(ip_pasv);
    return 0;
}

int request_file(int command_socketfd, const char * file) {
    char* request_file_command = malloc((strlen(file) + strlen(RETR) + 2) * sizeof(*request_file_command));
    
    if (request_file_command == NULL) {
        return MALLOC_ERROR;
    }

    // Building Username Command
    request_file_command[0] = '\0';
    strcat(request_file_command, RETR);
    strcat(request_file_command, " ");
    strcat(request_file_command, file);

    if (send_command(command_socketfd, request_file_command) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", request_file_command);
        free(request_file_command);
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", request_file_command);
    }
    free(request_file_command);

    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read retr initial command response\n");
        free(response);
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%s", response);
    }

    if (response_code != RETR_INITIAL_SUCCESS_CODE && response_code != RETR_INITIAL_SUCCESS_CODE_2) {
        fprintf(stderr, "RETR failed\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return RETR_ERROR;
    }

    free(response);

    return 0;
}

int read_retrieve_final_response(int command_socketfd) {
    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read retr final command response\n");
        free(response);
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }

    if (response_code != RETR_FINAL_SUCCESS_CODE) {
        fprintf(stderr, "RETR final failed\nResponse: %hd - %s\n", response_code, response);
        free(response);
        return RETR_FINAL_ERROR;
    }

    free(response);

    return 0;
}

int read_initial_response(int command_socketfd) {
    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Error reading initial response!\n");
        free(response);
        return ERROR_READING_INITIAL_RESPONSE;
    }

    if (DEBUG_MODE) {
        printf("%s\n", response);
    }

    if (response_code != INITIAL_CONNECTION_CODE) {
        fprintf(stderr, "Invalid initial response code\nResponse: %hd - %s", response_code, response);
        free(response);
        return INVALID_RESPONSE;
    }
    
    free(response);    

    return 0;
}