#include "prompt.h"
#include "tokenise.h"
#include "sys.h"
#include "log.h"
#include "activities.h"
#include "handle_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifndef MAX_INPUT_SIZE
#define MAX_INPUT_SIZE 4096
#endif

int main(){
    char home_dir[PATH_MAX];//for storing home_dir
    char input[MAX_INPUT_SIZE];
    if (getcwd(home_dir, sizeof(home_dir)) == NULL) {//gets the current working directory and stores it in home_dir
        perror("getcwd");
        exit(1);
    }
    init_log(home_dir);
    setup_signal_handler(); 
    
    while (1) {
        display_prompt(home_dir);
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            if (feof(stdin)) {  // If Ctrl-D (EOF) is pressed
                printf("\nExiting shell\n");
                break;  // Exit the shell gracefully
            }
            perror("fgets");
            continue;
        }
        int len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {//replace '\n' with '\0'
            input[len - 1] = '\0';
        }
        if (strcmp(input, "exit") == 0) {//exit
            break;
        }
        // process_input(input,home_dir);
        if (strchr(input, '|') || strchr(input, '>') || strchr(input, '<')) {
            both_helper(input, home_dir); // New handler for I/O and pipes
        } else {
            process_input(input, home_dir); // Standard command processing
        }
        monitor_background_processes();
    }

    return 0;
}
