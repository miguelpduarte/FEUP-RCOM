#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "download.h"

static void validate_url(const char* url, char** user, char** password, char** host, char** path);
static bool url_has_user(const char* url);
static bool host_is_specified(const char* url);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s ftp://[<user>:<password>@]<host>/<url-path>.\n", argv[0]);
        exit(INVALID_ARGS);
    }

    char* user = NULL;
    char* password = NULL;
    char* host = NULL;
    char* path = NULL;
    validate_url(argv[1], &user, &password, &host, &path);

    printf("User:     %s\n", user);
    printf("Password: %s\n", password);
    printf("Host:     %s\n", host); 
    printf("URL:      %s\n", path);

    free(user);
    free(password);
    free(host);
    free(path);

    return 0;
}

static void validate_url(const char* url, char** user, char** password, char** host, char** path) {
    if (strncmp(url, URL_START, strlen(URL_START)) != 0) {
        fprintf(stderr, "Invalid URL. URL should start with '" URL_START "'.\n");
        exit(INVALID_URL);
    }

    size_t index = strlen(URL_START);
    
    if (url_has_user(url)) {
        size_t user_len = 0, password_len = 0;

        for (; url[index] != ':'; ++index) {
            user_len++;
        }
        if (user_len == 0) {
            fprintf(stderr, "Invalid URL. User can not be empty.\n");
            exit(INVALID_USERNAME);
        }

        *user = strndup(url + index - user_len, user_len);

        index++;    //ignore ':' char
        for (; url[index] != '@'; ++index) {
            password_len++;
        }
        if (password_len == 0) {
            fprintf(stderr, "Invalid URL. Password can not be empty.\n");
            exit(INVALID_USERNAME);
        }

        *password = strndup(url + index - password_len, password_len);
        index++;    //ignore '@' char
    } 

    if (!host_is_specified(url + index)) {
        fprintf(stderr, "Invalid URL. URL must include host and path.\n");
        exit(HOST_UNSPECIFIED);
    }
    
    size_t host_len = 0;
    for (; url[index] != '/'; ++index) {
        host_len++;
    }
    if (host_len == 0) {
        fprintf(stderr, "Invalid URL. Host can not be empty.\n");
        exit(INVALID_HOST);
    }

    *host = strndup(url + index - host_len, host_len);
    index++;
    
    size_t path_len = strlen(url + index);
    if (path_len == 0) {
        fprintf(stderr, "Invalid URL. Path can not be empty.\n");
        exit(INVALID_PATH);
    }
    
    *path = strndup(url + index, path_len);
}

static bool url_has_user(const char* url) {
    size_t len = strlen(url);
    bool colon_found = false;
    bool at_sign_found = false;

    size_t i;
    for (i = 0; i < len; ++i) {
        // Check for : separator, separating the user and the password
        if (url[i] == ':') {
            colon_found = true;
        } 
        // Check for @ separator, separating a user:password block from the url
        else if (url[i] == '@' && colon_found) {
            at_sign_found = true;
        }
    }

    return colon_found && at_sign_found;
}

static bool host_is_specified(const char* url) {
    size_t len = strlen(url);

    size_t i;
    for (i = 0; i < len; ++i) {
        if (url[i] == '/') {
            return true;
        }
    }
    
    return false;
}