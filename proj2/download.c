#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "connection.h"
#include "commands.h"
#include "parser.h"

int main(int argc, char* argv[]) {
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

    char* ip = NULL;
    if (hostname_to_ip(host, &ip) != 0) {
        exit(HOSTNAME_TRANSLATION_ERROR);
    }

    if (transfer_file(user, password, ip, path, file) != 0) {
        exit(FILE_TRANSFER_ERROR);
    }

    free(user);
    free(password);
    free(host);
    free(ip);
    free(path);
    free(file);

    return 0;
}