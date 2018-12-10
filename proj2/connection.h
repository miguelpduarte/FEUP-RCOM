#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#define HOSTNAME_TRANSLATION_ERROR          1
#define SOCKET_ERROR                        2
#define CONNECTION_ERROR                    3
#define FILE_TRANSFER_ERROR                 4
#define LOGIN_ERROR                         5
#define CHANGE_DIR_ERROR                    6
#define CWD_ERROR                           7
#define SET_BINARY_MODE_ERROR               8
#define SET_PASISVE_MODE_ERROR              9
#define PARSE_PASV_FAILED                   10
#define REQUEST_FILE_FAILED                 11
#define DOWNLOAD_FILE_FAILED                12
#define RETRIEVE_FINAL_RESPONSE_FAILED      13
#define FAILED_FILE_TRANSFER                14
#define RETR_ERROR                          15
#define RETR_FINAL_ERROR                    16
#define READ_INITIAL_RESPONSE_ERROR         17

#define FTP_CONTROL_PORT 21

int hostname_to_ip(const char* hostname, char** ip);

int transfer_file(const char* user, const char* password, const char* host, const char* path, const char* file);

int connect_to_ip(const char * ip, unsigned );

#endif  //_CONNECTION_H_