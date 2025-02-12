#include "hop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#define PATH_MAX 4096
static char prev_dir[PATH_MAX] = "";//global var to store the previous directory

void hop(char *argss, char *home_dir){
    char* args = strdup(argss);
    char cwd[PATH_MAX];
    char new_dir[PATH_MAX];//buffer to store the new directory path to which the program will change
    if(getcwd(cwd, sizeof(cwd)) == NULL){//get cwd
        perror("getcwd");
        return;
    }
    if(args == NULL || *args == '\0'){//if no arguments are provided, hop to the home directory
        strncpy(new_dir, home_dir, PATH_MAX);
    }else {//tokenize the arguments to handle multiple hops
        char *token = strtok((char *)args, " ");//split args into token seperated by spaces
        while(token != NULL){
            if(strcmp(token, "~") == 0){//if token is ~ set new_dir as home_dir
                strncpy(new_dir, home_dir, PATH_MAX);
            }
            else if(strcmp(token, "-") == 0){//if token is -
                if(prev_dir[0] != '\0'){//if prev_dir is not null , set new_dir as prev_dir
                    strncpy(new_dir, prev_dir, PATH_MAX);
                }
                else{//error
                    printf("OLDPWD not set , no previous directory\n");
                    // exit(1);
                    return;
                }
            } 
            else if(strcmp(token, ".") == 0){//if token is . set new_dir as cwd
                strncpy(new_dir, cwd, PATH_MAX);
            } 
            else if(strcmp(token, "..") == 0){//if token is ..
                if(snprintf(new_dir, sizeof(new_dir), "%s/..", cwd) >= sizeof(new_dir)) {//format string
                    fprintf(stderr, "Path too long\n");
                    return;
                }
            } 
            else{
                if(token[0] == '/'){//if token starts with / , absolute path
                    strncpy(new_dir, token, PATH_MAX);//copies token to new_dir
                } 
                else if(token[0] == '~'){//if token starts vth ~ , path rltv to home dir
                    if(snprintf(new_dir, sizeof(new_dir), "%s%s", home_dir, token + 1) >= sizeof(new_dir)){//formats new_dir by concatenating home_dir with token(excluding ~)
                        fprintf(stderr, "Path too long\n");
                        return;
                    }
                } 
                else{//tokens that don’t start with / or ~, it’s assumed to be a rltv
                    if (snprintf(new_dir, sizeof(new_dir), "%s/%s", cwd, token) >= sizeof(new_dir)){//formats new_dir by appending the token to cwd
                        fprintf(stderr, "Path too long\n");
                        return;
                    }
                }
            }
            if(chdir(new_dir) == 0){//successfully changed directory
                strncpy(prev_dir, cwd, PATH_MAX);//update prev_dir before changing cwd
                if(getcwd(cwd, sizeof(cwd)) != NULL){//get new cwd 
                    printf("%s\n", cwd);
                }
            }
            else{
                perror(token);
                return;
            }
            token = strtok(NULL, " ");//to get the next token from the input string
        }
    }
}
