#include "fg_bg.h"
#include "sys.h"
#include "activities.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void handle_fg_command(int pid) {
    // Check if the process exists
    if (kill(pid, 0) == -1) {
        // Process does not exist
        fprintf(stderr, "No such process found\n");
        return;
    }

    // Find and update the process in the process list
    if (!update_process_state(pid, "Running")) {
        fprintf(stderr, "No such process found\n");
        return;
    }

    // Send SIGCONT to bring the process to the foreground
    if (kill(pid, SIGCONT) == -1) {
        perror("Failed to send SIGCONT");
        return;
    }

    // Wait for the process to finish or stop
    int status;
    waitpid(pid, &status, WUNTRACED);

    // After waiting, update the state
    if (WIFSTOPPED(status)) {
        printf("Process %d stopped\n", pid);
        update_process_state(pid, "Stopped");
    } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
        remove_process(pid);
    }
}


void handle_bg_command(int pid) {
    // Check if the process exists
    if (kill(pid, 0) == -1) {
        // Process does not exist
        fprintf(stderr, "No such process found\n");
        return;
    }

    // Find and update the process in the process list
    if (!update_process_state(pid, "Stopped")) {
        fprintf(stderr, "No such process found\n");
        return;
    }

    // Send SIGCONT to resume the process
    if (kill(pid, SIGCONT) == -1) {
        perror("Failed to send SIGCONT");
        return;
    }

    // Update the state to "Running" in the background
    update_process_state(pid, "Running");
}
