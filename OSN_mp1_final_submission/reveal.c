#include "reveal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>

#define MAX_PATH_LEN 4096
#define MAX_ENTRIES 1024 

int compare_entries(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void parse_flags_and_path(char *input, int *show_all, int *show_long, char **path) {
    *show_all = 0;
    *show_long = 0;
    *path = NULL;

    char *token = strtok(input, " ");
    while (token != NULL) {
        if (token[0] == '-') {
            for (int i = 1; token[i] != '\0'; ++i) {
                if (token[i] == 'a') {
                    *show_all = 1;
                } else if (token[i] == 'l') {
                    *show_long = 1;
                } else {
                    fprintf(stderr, "Invalid flag -%c\n", token[i]);
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            *path = token;
        }
        token = strtok(NULL, " ");
    }

    if (*path == NULL) {
        *path = "."; // default to the current directory if no path is provided
    }
}

char *resolve_path(const char *path, const char *home_dir) {
    // handle "~" as the home directory
    if (path[0] == '~') {
        const char *home = home_dir;
        char resolved_path[MAX_PATH_LEN];
        snprintf(resolved_path, sizeof(resolved_path), "%s%s", home, path + 1);
        char *final_path = realpath(resolved_path, NULL);
        if (final_path == NULL) {
            perror("realpath");
        }
        return final_path;
    }

    char *resolved_path = realpath(path, NULL);
    if (resolved_path == NULL) {
        perror("realpath");
        return NULL;
    }
    return resolved_path;
}

void print_long_format(const char *path, const char *filename) {
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, filename);

    struct stat statbuf;
    if (stat(full_path, &statbuf) == -1) {
        perror("stat");
        return;
    }

    // determine file type and color
    const char *color;
    if (S_ISDIR(statbuf.st_mode)) {
        color = "\x1b[34m"; // Blue for directories
    } else if (S_ISLNK(statbuf.st_mode)) {
        color = "\x1b[36m"; // Cyan for symlinks
    } else if (statbuf.st_mode & S_IXUSR) {
        color = "\x1b[32m"; // Green for executable files
    } else {
        color = "\x1b[0m";  // Default color for other files
    }

    printf("%s", color);
    printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-");
    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
    printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
    printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
    printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
    printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
    printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
    printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
    printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
    printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");
    printf("\x1b[0m"); // Reset color

    printf(" %ld %s %s %ld %s %s\n",
           statbuf.st_nlink,
           getpwuid(statbuf.st_uid)->pw_name,
           getgrgid(statbuf.st_gid)->gr_name,
           statbuf.st_size,
           ctime(&statbuf.st_mtime),
           filename);
}

int calculate_total_block_size(const char *path, int show_all) {
    int total_blocks = 0;

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        char full_path[MAX_PATH_LEN];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (stat(full_path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        total_blocks += statbuf.st_blocks;
    }

    closedir(dir);
    return total_blocks;
}

void reveal(const char *path, int show_all, int show_long, const char *home_dir) {
    char *resolved_path = resolve_path(path, home_dir);
    if (resolved_path == NULL) {
        return;
    }

    struct stat statbuf;
    if (stat(resolved_path, &statbuf) == -1) {
        perror("stat");
        free(resolved_path);
        return;
    }

    if (S_ISDIR(statbuf.st_mode)) {
        // It's a directory
        DIR *dir = opendir(resolved_path);
        if (dir == NULL) {
            perror("opendir");
            free(resolved_path);
            return;
        }

        char *entries[MAX_ENTRIES];
        int num_entries = 0;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (!show_all && entry->d_name[0] == '.') {
                continue;
            }

            if (num_entries >= MAX_ENTRIES) {
                fprintf(stderr, "Too many entries to handle.\n");
                break;
            }

            entries[num_entries] = strdup(entry->d_name);
            if (entries[num_entries] == NULL) {
                perror("strdup");
                continue;
            }
            num_entries++;
        }

        closedir(dir);

        // Sort entries
        qsort(entries, num_entries, sizeof(char *), compare_entries);

        if (show_long) {
            int total_blocks = calculate_total_block_size(resolved_path, show_all);
            printf("total %d\n", total_blocks);
        }

        for (int i = 0; i < num_entries; i++) {
            char entry_path[MAX_PATH_LEN];
            snprintf(entry_path, sizeof(entry_path), "%s/%s", resolved_path, entries[i]);

            struct stat entry_stat;
            if (stat(entry_path, &entry_stat) == -1) {
                perror("stat");
                continue;
            }

            if (show_long) {
                print_long_format(resolved_path, entries[i]);
            } else {
               
                const char *color;
                if (S_ISDIR(entry_stat.st_mode)) {
                    color = "\x1b[34m"; 
                } else if (S_ISLNK(entry_stat.st_mode)) {
                    color = "\x1b[36m";
                } else if (entry_stat.st_mode & S_IXUSR) {
                    color = "\x1b[32m"; 
                } else {
                    color = "\x1b[0m";
                }
                printf("%s%s\x1b[0m\n", color, entries[i]);
            }

            free(entries[i]);
        }
    } else {
       
        if (show_long) {
            print_long_format("", resolved_path);
        } else {
            
            const char *color;
            if (S_ISDIR(statbuf.st_mode)) {
                color = "\x1b[34m"; 
            } else if (S_ISLNK(statbuf.st_mode)) {
                color = "\x1b[36m"; 
            } else if (statbuf.st_mode & S_IXUSR) {
                color = "\x1b[32m"; 
            } else {
                color = "\x1b[0m"; 
            }
            printf("%s%s\x1b[0m\n", color, resolved_path);
        }
    }

    free(resolved_path);
}
