
#include "handle_myshrc.h"
#include <ctype.h>
Alias aliases[1000]; 
int alias_count = 0; 

void load_aliases(Alias aliases[], int *alias_count) {
    FILE *file = fopen(ALIAS_FILE, "r");
    if (file == NULL) {
        perror("Failed to open alias file");
        return;
    }

    char line[MAX_ALIAS_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        line[strcspn(line, "\n")] = 0;
        char *equals_pos = strchr(line, '=');
        if (equals_pos == NULL) continue;

        *equals_pos = '\0';
        char *key = line;
        char *value = equals_pos + 1;

        while (isspace((unsigned char)*key)) key++;
        char *end = key + strlen(key) - 1;
        while (end > key && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        while (isspace((unsigned char)*value)) value++;
        end = value + strlen(value) - 1;
        while (end > value && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        strncpy(aliases[*alias_count].key, key, MAX_ALIAS_LEN);
        strncpy(aliases[*alias_count].value, value, MAX_ALIAS_LEN);
        (*alias_count)++;
    }

    fclose(file);
}

void apply_aliases(char *command) {
    for (int i = 0; i < alias_count; i++) {
        char *pos = strstr(command, aliases[i].key);
        if (pos != NULL) {
            char buffer[MAX_INPUT_SIZE];
            size_t len = pos - command;
            strncpy(buffer, command, len);
            buffer[len] = '\0';
            strcat(buffer, aliases[i].value);
            strcat(buffer, pos + strlen(aliases[i].key));
            strcpy(command, buffer);
        }
    }
}
