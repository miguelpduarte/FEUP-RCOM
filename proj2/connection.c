#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int hostname_to_ip(const char* hostname, char** ip) {
    struct hostent * h;

    if ((h = gethostbyname(hostname)) == NULL) {
        fprintf(stderr, "Could not translate hostname %s to an IP address", hostname);
        return 1;
    }

    char * temp_ip = inet_ntoa(*((struct in_addr *) h->h_addr_list[0]));
    *ip = strndup(temp_ip, strlen(temp_ip));

    return 0;
}

int start_connection(const char* user, const char* password, const char* host, const char* path, const char* file) {
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

    // server_addr.sin_family = AF_INET;
	// server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
	// server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */

    int command_socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (command_socketfd < 0) {
        fprintf(stderr, "Error creating socket");
        return -1;
    }

    return -1;
}