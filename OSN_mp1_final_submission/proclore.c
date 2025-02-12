#include "proclore.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#define MAX_PATH 4096

void print_process_info(pid_t pid){
    char path[MAX_PATH];
    char buf[MAX_PATH];
    int len;
    FILE *fp;
    
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);//format the path for the /proc filesystem

    fp = fopen(path, "r");//open the /proc/[pid]/stat file
    if (fp == NULL) {
        perror("fopen");
        return;
    }
    if (fgets(buf, sizeof(buf), fp) == NULL) {//reads a line from the file into buf. If reading fails, it prints an error message.
        perror("fgets");
        fclose(fp);
        return;
    }
    fclose(fp);

    //parse the stat file
    char comm[256];//buffer to hold the command name of the process
    char state;//var to hold the process state character.
    unsigned long vsize;//var to hold the virtual memory size
    sscanf(buf, "%*d %s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu",
           comm, &state, &vsize);//parses the process information from the stat file line.

    //print process information
    printf("pid : %d\n", pid);
    printf("process status : %c\n", state);
    printf("Process Group : %d\n", getpgid(pid));
    printf("Virtual memory : %lu\n", vsize);

    // Get executable path
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);//formats the path to the symbolic link of the executable.
    len = readlink(path, buf, sizeof(buf) - 1);//reads the symbolic link to get the path of the executable.
    if(len != -1){
        buf[len] = '\0';
        printf("executable path : %s\n", buf);
    } 
    else{
        perror("readlink");
    }
}

void handle_proclore_command(char *args) {
    pid_t pid;
    if (args == NULL || strlen(args) == 0){//checks if no arguments are provided.
        pid = getpid(); //default to current shell process
    } 
    else{
        pid = atoi(args);
        if (pid <= 0){//converts the args string to an integer PID
            fprintf(stderr, "Invalid PID: %s\n", args);
            return;
        }
    }

    print_process_info(pid);
}
