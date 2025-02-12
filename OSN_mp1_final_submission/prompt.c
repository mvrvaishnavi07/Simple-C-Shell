#include "prompt.h"
#include "sys.h" 
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

void display_prompt(const char *home_dir) {
    const char *username_color = "\033[35m"; 
    const char *reset_color = "\033[0m";    

    struct passwd *pw = getpwuid(getuid()); // get the passwd struct using the UID
    if (pw == NULL) {
        perror("getpwuid() error");
        exit(1);
    }
    char *username = pw->pw_name; // pw->pw_name contains username of user char* username points to it
    char sysname[4096];
    if (gethostname(sysname, sizeof(sysname)) == -1) { // gets system’s hostname and stores it in sysname
        perror("gethostname() error");
        exit(1);
    }

    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL) { // gets the current working directory and stores it in cwd
        perror("getcwd() error");
        exit(1);
    }

    if (strstr(cwd, home_dir) == cwd) { // cwd starts with home_dir
        if (last_sleep_duration > 2) {
            printf("%s<%s@%s:~%s sleep : %ds>%s ", username_color, username, sysname, cwd + strlen(home_dir), last_sleep_duration, reset_color);
        } else {
            printf("%s<%s@%s:~%s>%s ", username_color, username, sysname, cwd + strlen(home_dir), reset_color);
        }
    } else {
        if (last_sleep_duration > 2) {
            printf("%s<%s@%s:%s sleep : %ds>%s ", username_color, username, sysname, cwd, last_sleep_duration, reset_color);
        } else {
            printf("%s<%s@%s:%s>%s ", username_color, username, sysname, cwd, reset_color);
        }
    }
}
