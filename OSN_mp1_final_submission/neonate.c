#include "neonate.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#define PROC_DIR "/proc"

pid_t get_most_recent_pid() {
    int highest_pid = 0;
    struct dirent *entry;
    DIR *proc_dir = opendir(PROC_DIR);
    if (!proc_dir) {
        perror("Failed to open /proc directory");
        exit(EXIT_FAILURE);
    }

   
    while ((entry = readdir(proc_dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            if (pid > highest_pid) {
                highest_pid = pid;
            }
        }
    }

    closedir(proc_dir);
    return highest_pid;
}

void set_nonblocking_mode() {
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void restore_blocking_mode() {
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);
}

void run_neonate(int time_interval) {
    // Set terminal to non-blocking mode
    set_nonblocking_mode();

    while (1) {
        // Check if 'x' was pressed
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0 && c == 'x') {
            printf("Stopping neonate.\n");
            break;
        }

        // Get the most recently created process' PID
        pid_t most_recent_pid = get_most_recent_pid();
        printf("Most recent PID: %d\n", most_recent_pid);

        // Sleep for the specified time interval
        sleep(time_interval);
    }

    // Restore terminal to blocking mode
    restore_blocking_mode();
}
