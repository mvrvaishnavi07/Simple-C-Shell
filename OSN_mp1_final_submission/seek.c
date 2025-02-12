#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"

void print_entry(char *path, int is_dir) {
    if (is_dir) {
        printf(BLUE "%s/\n" RESET, path);
    } else {
        printf(GREEN "%s\n" RESET, path);
    }
}
void seek(const char *target, const char *dir, int look_for_files, int look_for_dirs, int execute_flag) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(dir);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip the . and .. directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Construct the full path
        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);

        // Check the type of the entry
        if (stat(full_path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        int is_dir = S_ISDIR(statbuf.st_mode);
        int is_file = S_ISREG(statbuf.st_mode);

        // Match the prefix of the name
        if (strncmp(entry->d_name, target, strlen(target)) == 0) {
            if (is_dir && look_for_dirs) {
                print_entry(full_path, 1);
                if (execute_flag) {
                    if (chdir(full_path) == -1) {
                        perror("chdir");
                    } else {
                        printf("Changed directory to: %s\n", full_path);
                        // Stop further recursive processing since we changed directories
                        closedir(dp);  // Close the previous directory
                        return;  // Exit after changing directories
                    }
                }
            } else if (is_file && look_for_files) {
                print_entry(full_path, 0);
                if (execute_flag) {
                    FILE *fp = fopen(full_path, "r");
                    if (fp == NULL) {
                        perror("fopen");
                    } else {
                        char ch;
                        while ((ch = fgetc(fp)) != EOF) {
                            putchar(ch);
                        }
                        fclose(fp);
                    }
                }
            }
        }

        // Recursively search in subdirectories
        if (is_dir) {
            seek(target, full_path, look_for_files, look_for_dirs, execute_flag);
        }
    }

    closedir(dp);
}

int seekhlpr(int argc, char *argv[]){
    int look_for_files = 1, look_for_dirs = 1, execute_flag = 0;
    char *target = NULL;
    char *search_dir = ".";

    // Check if there are enough arguments
    if (argc < 1) {
        printf("Invalid usage! You must provide at least a target name.\n");
        return 1;
    }
    // printf("Number of arguments: %d\n", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("Argument %d: %s\n", i, argv[i]);
    // }


    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            look_for_files = 0;
        } else if (strcmp(argv[i], "-f") == 0) {
            look_for_dirs = 0;
        } else if (strcmp(argv[i], "-e") == 0) {
            execute_flag = 1;
        } else if (argv[i][0] != '-') { // Argument (non-flag) case
            if (target == NULL) {
                target = argv[i];
            } else {
                search_dir = argv[i];
            }
        } else {
            printf("Invalid flags!\n");
            return 1;
        }
    }

    if (look_for_files == 0 && look_for_dirs == 0) {
        printf("Invalid flags! Cannot use both -d and -f together.\n");
        return 1;
    }

    if (target == NULL) {
        printf("Invalid usage! You must provide a target name.\n");
        return 1;
    }

    seek(target, search_dir, look_for_files, look_for_dirs, execute_flag);
    return 0;
}


// int main(int argc, char *argv[]) {

//     seekhlpr(argc,argv);

//     return 0;
// }
