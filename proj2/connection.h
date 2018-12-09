#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#define HOSTNAME_TRANSLATION_ERROR  1
#define SOCKET_ERROR                2
#define CONNECTION_ERROR            3
#define FILE_TRANSFER_ERROR         4
#define LOGIN_ERROR                 5

#define FTP_CONTROL_PORT 21

int hostname_to_ip(const char* hostname, char** ip);

int transfer_file(const char* user, const char* password, const char* host, const char* path, const char* file);

#endif  //_CONNECTION_H_