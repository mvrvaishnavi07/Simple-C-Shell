#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#define MAX_ARG_SIZE 100

void handle_io_redirection(char *cmd) {
    char *input_file = NULL;
    char *output_file = NULL;
    char *append_file = NULL;

    // Check for redirection symbols
    if (strstr(cmd, ">")) {
        char *token = strtok(cmd, ">");
        cmd = strtok(NULL, ">");
        if (cmd) {
            if (strstr(cmd, ">>")) {
                // Handle >>
                token = strtok(cmd, ">>");
                cmd = strtok(NULL, ">>");
                if (cmd) {
                    append_file = strtok(cmd, " \t");
                    if (append_file) {
                        int fd = open(append_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if (fd < 0) {
                            perror("open");
                            exit(1);
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                }
            } else {
                // Handle >
                output_file = strtok(cmd, " \t");
                if (output_file) {
                    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
            }
        }
    }
    
    if (strstr(cmd, "<")) {
        char *token = strtok(cmd, "<");
        cmd = strtok(NULL, "<");
        if (cmd) {
            input_file = strtok(cmd, " \t");
            if (input_file) {
                int fd = open(input_file, O_RDONLY);
                if (fd < 0) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
        }
    }
}

void execute_command(char *cmd) {
    char *args[MAX_ARG_SIZE];
    int i = 0;
    
    char *token = strtok(cmd, " \t");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
    
    execvp(args[0], args);
    perror("exec failed");
    exit(1);
}

void handle_io_redirection_and_pipes(char *command) {
    int pipefds[2];
    int in_fd = -1;
    char *cmd = strtok(command, "|");
    
    while (cmd != NULL) {
        if (strtok(NULL, "|") != NULL) {
            if (pipe(pipefds) == -1) {
                perror("pipe failed");
                exit(1);
            }
        }
        
        if (fork() == 0) {
            if (in_fd != -1) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (strtok(NULL, "|") != NULL) {
                dup2(pipefds[1], STDOUT_FILENO);
            }
            close(pipefds[0]);
            handle_io_redirection(cmd);
            execute_command(cmd);
        } else {
            wait(NULL);
            if (strtok(NULL, "|") != NULL) {
                close(pipefds[1]);
            }
            if (in_fd != -1) {
                close(in_fd);
            }
            in_fd = pipefds[0];
            cmd = strtok(NULL, "|");
        }
    }
}

// int main(int argc, char *argv[]) {
//     if (argc < 2) {
//         fprintf(stderr, "Usage: %s <command>\n", argv[0]);
//         exit(1);
//     }

//     char command[1024];
//     strcpy(command, argv[1]);

//     handle_io_redirection_and_pipes(command);

//     return 0;
// }
