#ifndef ACTIVITIES_H
#define ACTIVITIES_H

typedef struct {
    int pid;
    char *command;
    char *state;
} ProcessInfo;

void add_process(int pid, const char *command);
int update_process_state(int pid, const char *state);
void remove_process(int pid);
void display_activities();
void monitor_background_processes();

#endif
