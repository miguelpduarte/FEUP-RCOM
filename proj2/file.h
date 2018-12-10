#ifndef _FILE_H_
#define _FILE_H_

#define BUF_SIZE                256
#define FILE_TRANSFER_FAILED    1
#define FILE_PERMISSIONS        0644

int copy_file(int fd, const char* file_name);

#endif  // _FILE_H_