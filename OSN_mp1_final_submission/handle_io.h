#ifndef HANDLE_IO_H
#define HANDLE_IO_H

#include <stddef.h>

void redir_pipes(char *cmd,const char *home_dir);
void handle_pipes(char *cmd,const char *home_dir);
void both_helper(char* cmd,const char *home_dir);

#endif
