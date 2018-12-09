#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <unistd.h>

#define DEBUG_MODE  1

// Commands
#define USER        "USER"
#define PASS        "PASS"
#define CWD         "CWD"
#define TYPE_BINARY "TYPE I"
#define PASV        "PASV"
#define RETR        "RETR"
#define QUIT        "QUIT"

// Replies
#define INITIAL_CONNECTION_CODE         220
#define USER_SUCCESS_CODE               331
#define USER_SUCCESS                    "331 Please specify the password."
#define PASS_SUCCESS_CODE               230
#define PASS_SUCCESS                    "230 Login successful."
#define CWD_SUCCESS_CODE                250
#define CWD_SUCCESS                     "250 Directory successfully changed."
#define CWD_FAILURE_CODE                550
#define CWD_FAILURE                     "550 Failed to change directory."
#define TYPE_SUCCESS_CODE               200
#define TYPE_SUCCESS                    "200 Switching to Binary mode."
#define PASV_SUCCESS_CODE               227
#define PASV_SUCCESS                    "227 Entering Passive Mode (%hd,%hd,%hd,%hd,%hd,%hd)."
#define PASV_STRING                     " Entering Passive Mode (%hd,%hd,%hd,%hd,%hd,%hd)."
#define RETR_INITIAL_SUCCESS_CODE       150
#define RETR_INITIAL_SUCCESS            "150 Opening BINARY mode data connection for <file> (<size> bytes)."
#define RETR_FINAL_SUCCESS_CODE         226
#define RETR_FINAL_SUCCESS              "226 Transfer complete."
#define QUIT_SUCCESS_CODE               221
#define QUIT_SUCCESS                    "221 Goodbye."

#define CODE_SIZE                       3
#define RESPONSE_MAX_SIZE               1025

#define READ_CMD_ERROR                  1
#define MALLOC_ERROR                    2
#define ERROR_READING_EXTRA_RESPONSE    3
#define SENDING_COMMAND_ERROR           4
#define READING_RESPONSE_ERROR          5
#define ERROR_READING_INITIAL_RESPONSE  6
#define INVALID_RESPONSE                7

// Temp: Processo:
/* 
1 - Login
1.1 - Send username
1.1.1 - USER username
1.1.2 - Handle response: 331 Please specify the password.
1.2 - Send password
1.2.1 - PASS password
1.2.2 - Handle response: 230 Login successful.
2 - Change directory to the desired one
2.1.1 - CWD dir
2.1.2 - Handle response: 250 Directory successfully changed.
2.1.2 - Handle error: 550 Failed to change directory.
3 - Initiate transfer
3.1 - Setting mode as binary
3.1.1 - TYPE I
3.1.2 - Handle response: 200 Switching to Binary mode.
3.2 - Enabling passive mode
3.2.1 - PASV
3.2.2 - Handle response: 227 Entering Passive Mode (193,137,29,15,213,242).
3.3 - Request file
3.3.1 - RETR filename
3.3.2 - Handle starting response: 150 Opening BINARY mode data connection for README (1184 bytes).
3.3.3 - Receive file
3.3.4 - Handle final response: 226 Transfer complete.
4 - Close connection
4.1 - QUIT
4.2 - Handle response: 221 Goodbye.
*/

int read_command_reply(int socketfd, unsigned short* response_code, char** response_str, size_t * response_str_size);

int send_command(int socketfd, const char* command);

#endif  //_COMMANDS_H_