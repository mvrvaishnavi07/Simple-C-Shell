#include "activities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PROCESSES 4096

static ProcessInfo processes[MAX_PROCESSES];
static int process_count = 0;
void add_process(int pid, const char *command) {
    if (process_count >= MAX_PROCESSES) {
        fprintf(stderr, "Max process limit reached\n");
        return;
    }
    processes[process_count].pid = pid;
    processes[process_count].command = strdup(command);
    processes[process_count].state = strdup("Running");
    process_count++;
    
}


int update_process_state(int pid, const char *state) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            free(processes[i].state);
            processes[i].state = strdup(state);
            return 1; 
        }
    }
    return 0;
}

void remove_process(int pid) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            free(processes[i].command);
            free(processes[i].state);
            for (int j = i; j < process_count - 1; j++) {
                processes[j] = processes[j + 1];
            }
            process_count--;
            return;
        }
    }
}

int compare_processes(const void *a, const void *b) {
    ProcessInfo *procA = (ProcessInfo *)a;
    ProcessInfo *procB = (ProcessInfo *)b;
    return strcmp(procA->command, procB->command);
}

void display_activities() {
    qsort(processes, process_count, sizeof(ProcessInfo), compare_processes);
    for (int i = 0; i < process_count; i++) {
        printf("[%d] : %s - %s\n", processes[i].pid, processes[i].command, processes[i].state);
    }
}

void monitor_background_processes() {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
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

// pid_t get_most_recent_pid() {
//     return most_recent_pid;
// }