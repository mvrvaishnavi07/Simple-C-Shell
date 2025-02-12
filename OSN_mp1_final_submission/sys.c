#include "sys.h"
#include "activities.h"
#include "signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

#define MAX_INPUT_SIZE 4096
int last_sleep_duration = 0;

void background_process_handler(int sig) {
    int status;
    pid_t pid;


    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        // printf("Handling PID(background): %d\n", pid);
        if (WIFEXITED(status)) {
            printf("Background process with PID %d exited normally\n", pid);
            update_process_state(pid, "Exited");
        } else if (WIFSIGNALED(status)) {
            printf("Background process with PID %d terminated by signal %d\n", pid, WTERMSIG(status));
            update_process_state(pid, "Exited");
        } else if (WIFSTOPPED(status)) {
            printf("Background process with PID %d is stopped\n", pid);
            update_process_state(pid, "Stopped");
        }
        remove_process(pid);
    }
}

void execute_command(char *command, char *home_dir, int run_in_background) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return;
    } else if (pid == 0) {
        // Child process
        char *args[MAX_INPUT_SIZE];
        int i = 0;
        args[i++] = strtok(command, " ");
        while ((args[i++] = strtok(NULL, " ")) != NULL);

        // Redirect standard input/output/error if running in background
        if (run_in_background) {
            freopen("/dev/null", "r", stdin);
            freopen("/dev/null", "w", stdout);
            freopen("/dev/null", "w", stderr);
        }

        execvp(args[0], args);
        perror("Exec failed"); 
        exit(1);
    } else {
        if (run_in_background) {
            // For background processes
            printf("[%d] %d\n", pid, pid);
            add_process(pid, command); // Add the process to the list
        } else {
            // For foreground processes
            current_foreground_pid = pid;  // Track the foreground process
            // printf("curr pid %d\n",current_foreground_pid);
            time_t start_time = time(NULL);
            int status;
            waitpid(pid, &status, WUNTRACED); 
            current_foreground_pid = -1;       // Reset after the process finishes

            time_t end_time = time(NULL);

            int duration = (int)difftime(end_time, start_time);
            // Check if the command is 'sleep' and took more than 2 seconds
            if (strncmp(command, "sleep", 5) == 0 && duration > 2) {
                last_sleep_duration = duration;
            } else {
                last_sleep_duration = 0;  // Reset if not sleep or duration <= 2s
            }

            if (WIFSTOPPED(status)) {
                // If process was stopped, add it to background list with "Stopped" state
                printf("\nProcess %d stopped\n", pid);
                add_process(pid, command);
                update_process_state(pid, "Stopped");
            }
        }
    }
}

void setup_signal_handler() {
    struct sigaction sa;

    sa.sa_handler = background_process_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(1);
    }

    // SIGINT (Ctrl-C) handling
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT failed");
        exit(1);
    }

    // SIGTSTP (Ctrl-Z) handling
    sa.sa_handler = sigtstp_handler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction SIGTSTP failed");
        exit(1);
    }
}
