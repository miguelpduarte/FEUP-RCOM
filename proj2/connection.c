#include "connection.h"
#include "commands.h"
#include "parser.h"
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
static int set_passive_mode(int command_socketfd, char** ip, unsigned* port);

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
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    // 32 bit Internet address network byte ordered
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(FTP_CONTROL_PORT);

    int command_socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (command_socketfd < 0) {
        fprintf(stderr, "Error creating socket\n");
        exit(SOCKET_ERROR);
    }

    // Open connection to the server
    if (connect(command_socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Error connecting to the given ip\n");
		exit(CONNECTION_ERROR);
    }

    // Read initial response
    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    printf("Ask teacher about server taking time to respond\n");
    sleep(1);

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Error reading initial response!\n");
        return ERROR_READING_INITIAL_RESPONSE;
    }

    if (DEBUG_MODE) {
        printf("%hd - %s\n", response_code, response);
    }

    if (response_code != INITIAL_CONNECTION_CODE) {
        fprintf(stderr, "Invalid initial response code\nResponse: %hd - %s", response_code, response);
        return INVALID_RESPONSE;
    }
    free(response);

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
    char* ip_pasv = NULL;
    unsigned port_pasv;
    if (set_passive_mode(command_socketfd, &ip_pasv, &port_pasv) != 0) {
        fprintf(stderr, "Set passive mode failed!\n");
        return SET_BINARY_MODE_ERROR;        
    }
    printf("\nPASV IP:   %s\n", ip_pasv);
    printf("PASV PORT: %d\n\n", port_pasv);
    free(ip_pasv);
    
    // Download the file

    // Close connection
    close_connection(command_socketfd);

    // Close socket
    close(command_socketfd);

    return -1;
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
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", username_cmd);
    }

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read user command response\n");
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%hd - %s\n", response_code, response);
    }

    if (response_code != USER_SUCCESS_CODE) {
        fprintf(stderr, "Login failed (user)\nResponse: %hd - %s\n", response_code, response);
        return LOGIN_ERROR;
    }

    // Because we are reusing variables
    free(response);
    response = NULL;

    // Send Password
    if (send_command(command_socketfd, password_cmd) != 0) {
        fprintf(stderr, "Failed to send command: %s\n", password_cmd);
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s ****\n", PASS);
    }

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read pass command response\n");
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%hd - %s\n", response_code, response);
    }
    
    if (response_code != PASS_SUCCESS_CODE) {
        fprintf(stderr, "Login failed (pass)\nResponse: %hd - %s\n", response_code, response);
        return LOGIN_ERROR;
    }

    free(response);
    free(username_cmd);
    free(password_cmd);

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
        return;
    }

    if (DEBUG_MODE) {
        printf("%hd - %s\n", response_code, response);
    }

    if (response_code != QUIT_SUCCESS_CODE) {
        fprintf(stderr, "Quit failed\nResponse: %hd - %s\n", response_code, response);
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
        return SENDING_COMMAND_ERROR;
    }

    if (DEBUG_MODE) {
        printf("->> %s\n", change_dir_command);
    }

    unsigned short response_code;
    char* response = NULL;
    size_t response_size;

    if (read_command_reply(command_socketfd, &response_code, &response, &response_size) != 0) {
        fprintf(stderr, "Failed to read cwd command response\n");
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%hd - %s\n", response_code, response);
    }

    if (response_code != CWD_SUCCESS_CODE) {
        fprintf(stderr, "CWD failed\nResponse: %hd - %s\n", response_code, response);
        return CWD_ERROR;
    }

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
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%hd - %s\n", response_code, response);
    }

    if (response_code != TYPE_SUCCESS_CODE) {
        fprintf(stderr, "Set binary mode failed\nResponse: %hd - %s\n", response_code, response);
        return INVALID_RESPONSE;
    }

    free(response);
    return 0;
}

static int set_passive_mode(int command_socketfd, char** ip, unsigned* port) {
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
        return READING_RESPONSE_ERROR;
    }

    if (DEBUG_MODE) {
        printf("%hd - %s\n", response_code, response);
    }

    if (response_code != PASV_SUCCESS_CODE) {
        fprintf(stderr, "Set passive mode failed\nResponse: %hd - %s\n", response_code, response);
        return INVALID_RESPONSE;
    }

    if (parsePASV(response, ip, port) != 0) {
        fprintf(stderr, "Failed to parse passive mode response\n");
        return PARSE_PASV_FAILED;
    }

    free(response);
    return 0;
}