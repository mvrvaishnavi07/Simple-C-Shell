#include "signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

pid_t current_foreground_pid = -1; // Track the foreground process

void sigint_handler(int sig) {
    if (current_foreground_pid > 0) {
        // Send SIGINT to the foreground process
        kill(current_foreground_pid, SIGINT);
    }
}

void sigtstp_handler(int sig) {
    if (current_foreground_pid > 0) {
        // Send SIGTSTP to the foreground process
        kill(current_foreground_pid, SIGTSTP);
    }
}

void setup_signal_handlerr() {
    struct sigaction sa;
    // printf("%d\n",current_foreground_pid);
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

void send_signal_to_process(pid_t pid, int signal_number) {
    int signal = signal_number % 32; // Handle out-of-range signals

    if (kill(pid, 0) == -1) {
        fprintf(stderr, "No such process found\n");
        return;
    }

    if (signal <= 0 || signal >= NSIG) {
        fprintf(stderr, "Invalid signal number\n");
        return;
    }

    if (kill(pid, signal) == -1) {
        perror("Failed to send signal");
    } else {
        printf("Sent signal %d to process with PID %d\n", signal, pid);
    }
}
