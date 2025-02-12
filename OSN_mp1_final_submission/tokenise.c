#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "sys.h"
#include "seek.h"
#include "proclore.h"
// #include "handle_myshrc.h"
#include "handle_io.h"
#include "fg_bg.h"
#include "activities.h"
#include "signal.h" 
#include "neonate.h"
#include "iman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_INPUT_SIZE 4096
const char *custom_cmds[] = {//list of custom commands
    "hop",
    "reveal",
    "log",
    "proclore",
    "seek",
    "activities",
    "ping",
    "fg",
    "bg",
    "neonate",
    "iMan"
};

int n = sizeof(custom_cmds) / sizeof(custom_cmds[0]);
int is_custom_command(char *command);

void process_input(char *input, char *home_dir) {
    char *dup = strdup(input);
    int should_log = 1;
    int exec_from_log_idx;
    if(home_dir == NULL){
        exec_from_log_idx = 1;
    }
    char *command_segment = strtok(input, ";");
    while (command_segment != NULL) {
        while (isspace(*command_segment)) { 
            command_segment++;
        }
        char *end = command_segment + strlen(command_segment) - 1;
        while (end > command_segment && isspace(*end)) { 
            end--;
        }
        *(end + 1) = '\0';
        char *command_start = command_segment;
        while (command_start != NULL) {
            char *ampersand_pos = strchr(command_start, '&');
            if (ampersand_pos != NULL) {
                *ampersand_pos = '\0'; 
                command_start = ampersand_pos + 1; 
            } else {
                command_start = NULL; 
            }

            while (isspace(*command_segment)) { 
                command_segment++;
            }
            end = command_segment + strlen(command_segment) - 1;
            while (end > command_segment && isspace(*end)) { 
                end--;
            }
            *(end + 1) = '\0';
            int run_in_background = (ampersand_pos != NULL);
            if (is_custom_command(command_segment)) {
                if (strncmp(command_segment, "hop", 3) == 0 && (command_segment[3] == '\0' || isspace(command_segment[3]))) {
                    char *args = command_segment + 3;
                    while (isspace(*args)) {
                        args++;
                    }
                    hop(args, home_dir);
                } else if (strncmp(command_segment, "reveal", 6) == 0 && (command_segment[6] == '\0' || isspace(command_segment[6]))) {
                    int show_all, show_long;
                    char *path;
                    parse_flags_and_path(command_segment + 7, &show_all, &show_long, &path);
                    reveal(path, show_all, show_long, home_dir);
                }
                else if (strncmp(command_segment, "log", 3) == 0 && (command_segment[3] == '\0' || isspace(command_segment[3]))) {
                    should_log = 0;
                    char *args = command_segment + 3;
                    while (isspace(*args)) {
                        args++;
                    }
                    if (strcmp(args, "purge") == 0) {
                        purge_log();
                    } else if (strncmp(args, "execute", 7) == 0 && isspace(args[7])) {
                        int index = atoi(args + 8);
                        execute_log_command(index);
                    } else {
                        show_log();
                    }
                    } else if (strncmp(command_segment, "proclore", 8) == 0 && (command_segment[8] == '\0' || isspace(command_segment[8]))) {
                        char *args = command_segment + 8;
                        while (isspace(*args)) {
                            args++;
                        }
                        handle_proclore_command(args);
                    }

                    else if (strncmp(command_segment, "seek", 4) == 0 && (command_segment[4] == '\0' || isspace(command_segment[4]))) {
                        char *args = command_segment + 4;
                        while (isspace(*args)) {
                            args++;
                        }
                        char *seek_args[10];
                        int seek_arg_count = 0;
                        char *token = strtok(args, " ");
                        while (token != NULL && seek_arg_count < 10) {
                            seek_args[seek_arg_count++] = token;
                            token = strtok(NULL, " ");
                        }
                        seek_args[seek_arg_count] = NULL; 
                        seekhlpr(seek_arg_count, seek_args);
                    }
                    
                    else if (strncmp(command_segment, "activities", 10) == 0 && (command_segment[10] == '\0' || isspace(command_segment[10]))) {
                        // printf("act\n");
                        display_activities();  
                    }
                    else if (strncmp(command_segment, "ping", 4) == 0 && (command_segment[4] == '\0' || isspace(command_segment[4]))) {
                        // printf("Debug: ping command received: '%s'\n", command_segment);
                        int pid, signal_number;
                        if (sscanf(command_segment + 5, "%d %d", &pid, &signal_number) == 2) {
                            // printf("Debug: Parsed PID = %d, Signal Number = %d\n", pid, signal_number);
                            send_signal_to_process(pid, signal_number);
                        } else {
                            fprintf(stderr, "Invalid ping command format\n");
                        }
                    }
                    else if (strncmp(command_segment, "fg", 2) == 0 && (command_segment[2] == '\0' || isspace(command_segment[2]))) {
                        int pid;
                        if (sscanf(command_segment + 3, "%d", &pid) == 1) {
                            handle_fg_command(pid);
                        } else {
                            fprintf(stderr, "Invalid fg command format\n");
                        }
                    } else if (strncmp(command_segment, "bg", 2) == 0 && (command_segment[2] == '\0' || isspace(command_segment[2]))) {
                        int pid;
                        if (sscanf(command_segment + 3, "%d", &pid) == 1) {
                            handle_bg_command(pid);
                        } else {
                            fprintf(stderr, "Invalid bg command format\n");
                        }
                    }
                    else if (strncmp(command_segment, "neonate", 7) == 0 && (command_segment[7] == '\0' || isspace(command_segment[7]))) {
                        int time_interval = 1; // Default to 1 second if not provided
                        char *args = command_segment + 7;
                        while (isspace(*args)) {
                            args++;
                        }
                        if (strncmp(args, "-n", 2) == 0) {
                            sscanf(args + 3, "%d", &time_interval);
                        }
                        run_neonate(time_interval);
                    }
                    else if (strncmp(command_segment, "iMan", 4) == 0 && (command_segment[4] == '\0' || isspace(command_segment[4]))) {
                        char *args = command_segment + 4;
                        while (isspace(*args)) {
                            args++;
                        }
                        fetch_man_page(args);
                    }
                    
            } else {
                execute_command(command_segment, home_dir, run_in_background);
            }
            
            command_segment = command_start;
        }
        command_segment = strtok(NULL, ";");
    }
    if (should_log == 1 && exec_from_log_idx != 1) {
        add_to_log(dup , home_dir);
    }
    free(dup);
}

int is_custom_command(char *command) {//check if the command is in the list of custom commands
    for (int i = 0; i < n; i++) {
        if (strncmp(command, custom_cmds[i], strlen(custom_cmds[i])) == 0 &&
            (command[strlen(custom_cmds[i])] == '\0' || isspace(command[strlen(custom_cmds[i])])))
        {
            return 1;
        }
    }
    return 0;
}
