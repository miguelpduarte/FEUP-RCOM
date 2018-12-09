#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "connection.h"
#include "commands.h"
#include "parser.h"

int main(int argc, char* argv[]) {

    char * my_ip = NULL;

    hostname_to_ip("google.com", &my_ip);

    printf("ip from host: %s\n", my_ip);

    free(my_ip);


    if (argc != 2) {
        fprintf(stderr, "usage: %s ftp://[<user>:<password>@]<host>/<url-path>.\n", argv[0]);
        exit(INVALID_ARGS);
    }

    char* user = NULL;
    char* password = NULL;
    char* host = NULL;
    char* path = NULL;
    char* file = NULL;

    validate_url(argv[1], &user, &password, &host, &path, &file);

    start_connection(user, password, host, path, file);

    printf("User:     %s\n", user);
    printf("Password: %s\n", password);
    printf("Host:     %s\n", host); 
    printf("URL:      %s\n", path);
    printf("File:     %s\n", file);

    free(user);
    free(password);
    free(host);
    free(path);
    free(file);

    return 0;
}