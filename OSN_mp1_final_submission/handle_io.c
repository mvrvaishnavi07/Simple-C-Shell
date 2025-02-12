#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "handle_io.h"
#include "tokenise.h"
#include "log.h"
void redir_pipes(char *cmd,const char *home_dir){
    char *input_file = NULL;
    char *output_file = NULL;
    int append_mode = 0;

    // Check for output redirection (>)
    char *output_redirect = strstr(cmd, ">");
    if (output_redirect) {
        if (*(output_redirect + 1) == '>') {
            append_mode = 1;
            output_redirect += 2;
        } else {
            output_redirect += 1;
        }
        output_file = strtok(output_redirect, " ");
        *strchr(cmd, '>') = '\0';
    }

    // Check for input redirection (<)
    char *input_redirect = strstr(cmd, "<");
    if (input_redirect) {
        input_file = strtok(input_redirect + 1, " ");
        *strchr(cmd, '<') = '\0';
    }

    // Parse the command and arguments
    char *args[256];
    int argc = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL) {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    args[argc] = NULL;
    // if (is_valid_command(args[0])) {
    //     process_input(cmd, home_dir);
    //     return;
    // }
    pid_t pid = fork();

    if (pid == 0) { // Child process
        // Handle output redirection
        if (output_file) {
            int fd;
            if (append_mode) {
                fd = open(output_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            } else {
                fd = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);
            }
            if (fd < 0) {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);  // Redirect stdout
            close(fd);
        }

        // Handle input redirection
        if (input_file) {
            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("No such input file found!");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);  // Redirect stdin
            close(fd);
        }

        // Execute the command
        execvp(args[0], args);

        // If execvp fails
        perror("Exec failed");
        exit(EXIT_FAILURE);

    } else if (pid > 0) { // Parent process
        wait(NULL);  // Wait for child process to finish
    } else {
        perror("Fork failed");
    }
}

// Function to handle commands with pipes and redirection
void handle_pipes(char *cmd,const char *home_dir) {
    int num_pipes = 0;
    char *pipe_pos = cmd;
    while ((pipe_pos = strchr(pipe_pos, '|')) != NULL) {
        num_pipes++;
        pipe_pos++;
    }

    // Create pipes
    int pipefds[2 * num_pipes];
    for (int i = 0; i < num_pipes; i++) {
        pipe(pipefds + i * 2);
    }

    char *commands[num_pipes + 1];
    int i = 0;
    char *token = strtok(cmd, "|");
    while (token) {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }

    for (i = 0; i < num_pipes + 1; i++) {
        pid_t pid = fork();
        if (pid == 0) {  // Child process
            // If not the first command, redirect input from previous pipe
            if (i != 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }

            // If not the last command, redirect output to next pipe
            if (i != num_pipes) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * num_pipes; j++) {
                close(pipefds[j]);
            }

            // Execute the command (with possible redirection)
            redir_pipes(commands[i],home_dir);
            exit(0);
        }
    }

    // Close all pipe file descriptors in the parent process
    for (int i = 0; i < 2 * num_pipes; i++) {
        close(pipefds[i]);
    }

    // Wait for all child processes
    for (int i = 0; i < num_pipes + 1; i++) {
        wait(NULL);
    }
}

void both_helper(char* cmd,const char *home_dir){
    add_to_log(cmd , home_dir);
    if (strchr(cmd, '|')) {
        handle_pipes(cmd,home_dir);
    } else {
        redir_pipes(cmd,home_dir);
    }
}
// int main() {
//     char cmd[256];

//     while (1) {
//         printf("mysh> ");
//         if (!fgets(cmd, sizeof(cmd), stdin)) break;
//         cmd[strcspn(cmd, "\n")] = '\0'; // Remove newline character
//         both_helper(cmd);
        
//     }

//     return 0;
// }
