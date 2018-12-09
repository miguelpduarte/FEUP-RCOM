#ifndef _COMMANDS_H_
#define _COMMANDS_H_

// Commands
#define USER        "USER"
#define PASSWORD    "PASS"
#define CD          "CWD"
#define TYPE_BINARY "TYPE I"
#define PASSIVE     "PASV"
#define RETRIEVE    "RETR"
#define QUIT        "QUIT"

// Replies
#define CWD_SUCCESS_CODE                "250"
#define CWD_SUCCESS                     "250 Directory successfully changed."
#define CWD_FAILURE_CODE                "550"
#define CWD_FAILURE                     "550 Failed to change directory."
#define TYPE_SUCCESS_CODE               "200"
#define TYPE_SUCCESS                    "200 Switching to Binary mode."
#define PASV_SUCCESS_CODE               "227"
#define PASV_SUCCESS                    "227 Entering Passive Mode (%hd,%hd,%hd,%hd,%hd,%hd)."
#define RETR_INITIAL_SUCCESS_CODE       "150"
#define RETR_INITIAL_SUCCESS            "150 Opening BINARY mode data connection for <file> (<size> bytes)."
#define RETR_FINAL_SUCCESS_CODE         "226"
#define RETR_FINAL_SUCCESS              "226 Transfer complete."
#define QUIT_SUCCESS_CODE               "221"
#define QUIT_SUCCESS                    "221 Goodbye."


// Temp: Processo:
/* 
1 - Login
1.1 - USER username
1.2 - PASS password
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

#endif  //_COMMANDS_H_