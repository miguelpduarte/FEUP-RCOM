#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "commands.h"
#include "parser.h"

static size_t last_index_of(const char * str, const char to_find);
static bool url_has_password(const char* url);
static bool url_has_user(const char* url);
static bool host_is_specified(const char* url);

int parsePASV(const char* pasv, char** ip, unsigned* port) {
    unsigned short h1, h2, h3, h4, p1, p2;
    
    if (sscanf(pasv, PASV_SUCCESS, &h1, &h2, &h3, &h4, &p1, &p2) != NUM_PASV_FIELDS) {
        return -1;
    } else {
        *port = (p1<<8) | p2;
        *ip = malloc(IP_STRING_SIZE * sizeof(**ip));
        snprintf(*ip, IP_STRING_SIZE, "%d.%d.%d.%d", h1, h2, h3, h4);
        return 0;
    }
}

void validate_url(const char* url, char** user, char** password, char** host, char** path, char** file) {
    if (strncmp(url, URL_START, strlen(URL_START)) != 0) {
        fprintf(stderr, "Invalid URL. URL should start with '" URL_START "'.\n");
        exit(INVALID_URL);
    }

    size_t index = strlen(URL_START);
    
    if (url_has_user(url)) {
        bool has_password = url_has_password(url);

        size_t user_len = 0, password_len = 0;

        for (; url[index] != (has_password ? ':' : '@') ; ++index) {
            user_len++;
        }
        if (user_len == 0) {
            fprintf(stderr, "Invalid URL. User can not be empty.\n");
            exit(INVALID_USERNAME);
        }

        *user = strndup(url + index - user_len, user_len);

        if (has_password) {
            index++;    //ignore ':' char
            for (; url[index] != '@'; ++index) {
                password_len++;
            }
        
            *password = strndup(url + index - password_len, password_len);
        } else {
            *password = strndup("", 0);
        }

        index++;    //ignore '@' char
    } else {
        *user = strndup("", 0);
        *password = strndup("", 0);        
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

    // Actually the index of the '/'
    size_t file_name_index = last_index_of(url + index, '/');
    if (file_name_index == 0) {
        // There is no path specified, file is in root
        *path = strndup(".", 1);
        *file = strndup(url + index, path_len);
    } else {
        // There is a path AND a file
        *path = strndup(url + index, file_name_index);
        *file = strndup(url + index + file_name_index + 1, path_len - file_name_index - 1);
    }
    
}

static size_t last_index_of(const char * str, const char to_find) {
    const size_t str_size = strlen(str);
    size_t i = str_size - 1;
    for (; i > 0; --i) {
        if (str[i] == to_find) {
            return i;
        }
    }

    return 0;
}

static bool url_has_user(const char* url) {
    size_t len = strlen(url);

    size_t i;
    for (i = strlen(URL_START); i < len; ++i) {
        if (url[i] == '@') {
            return true;
        }
    }

    return false;
}

static bool url_has_password(const char* url) {
    size_t len = strlen(url);
    bool colon_found = false;
    bool at_sign_found = false;

    size_t i;
    for (i = strlen(URL_START); i < len; ++i) {
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
    for (i = strlen(URL_START); i < len; ++i) {
        if (url[i] == '/') {
            return true;
        }
    }
    
    return false;
}

