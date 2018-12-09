#ifndef _CONNECTION_H_
#define _CONNECTION_H_

int hostname_to_ip(const char* hostname, char** ip);

int start_connection(const char* user, const char* password, const char* host, const char* path, const char* file);

#endif  //_CONNECTION_H_