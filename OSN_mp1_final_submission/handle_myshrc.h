
#ifndef HANDLE_MYSHRC_H
#define HANDLE_MYSHRC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIAS_FILE ".myshrc"
#define MAX_ALIAS_LEN 256
#define MAX_INPUT_SIZE 4096
typedef struct Alias {
    char key[MAX_ALIAS_LEN];
    char value[MAX_ALIAS_LEN];
} Alias;
extern Alias aliases[];
extern int alias_count;

void load_aliases(Alias aliases[], int *alias_count);
void apply_aliases(char *command);

#endif
