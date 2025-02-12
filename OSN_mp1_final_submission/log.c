#include "log.h"
#include "tokenise.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LOG_SIZE 15

static char *log_entries[MAX_LOG_SIZE];
static int log_count = 0;
static char log_file_path[1024];

void init_log(const char *home_dir) {
    snprintf(log_file_path, sizeof(log_file_path), "%s/%s", home_dir, "command_log.txt");
    FILE *file = fopen(log_file_path, "r");
    if (!file) return;

    char line[4096];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // remove newline
        if (log_count < MAX_LOG_SIZE) {
            log_entries[log_count++] = strdup(line);
        }
    }
    fclose(file);
}

void save_log(const char *home_dir) {
    snprintf(log_file_path, sizeof(log_file_path), "%s/%s", home_dir, "command_log.txt");
    FILE *file = fopen(log_file_path, "w");
    if (!file) {
        perror("Failed to open log file");
        return;
    }

    for (int i = 0; i < log_count; i++) {
        fprintf(file, "%s\n", log_entries[i]);
    }

    fclose(file);
}

void add_to_log(const char *command, const char *home_dir) {
    if (log_count > 0 && strcmp(log_entries[log_count - 1], command) == 0) {
        return; // Avoid consecutive duplicates
    }

    if (log_count == MAX_LOG_SIZE) {
        free(log_entries[0]);
        memmove(log_entries, log_entries + 1, sizeof(char*) * (MAX_LOG_SIZE - 1));
        log_count--;
    }

    log_entries[log_count++] = strdup(command);
    save_log(home_dir);
}

void show_log() {
    for (int i = 0; i < log_count; i++) {
        printf("%d: %s\n", i + 1, log_entries[i]);
    }
}

void purge_log() {
    for (int i = 0; i < log_count; i++) {
        free(log_entries[i]);
    }
    log_count = 0;
    save_log(NULL);
}

void execute_log_command(int index) {
    if (index < 1 || index > log_count) {
        printf("Invalid log index\n");
        return;
    }
    int exec_from_log_idx = 1;
    char *command = log_entries[log_count - index]; 
    printf("Executing: %s\n", command);
    process_input(command, NULL);
}
