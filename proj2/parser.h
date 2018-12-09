#ifndef _PARSER_H_
#define _PARSER_H_

#define NUM_PASV_FIELDS 6
#define IP_STRING_SIZE 16

#define INVALID_ARGS        -1
#define INVALID_URL         -2
#define INVALID_USERNAME    -3
#define INVALID_PASSWORD    -4
#define HOST_UNSPECIFIED    -5
#define INVALID_HOST        -6
#define INVALID_PATH        -7
#define URL_START       "ftp://"

int parsePASV(const char* pasv, char** ip, unsigned* port);

void validate_url(const char* url, char** user, char** password, char** host, char** path, char** file);

#endif //_PARSER_H_