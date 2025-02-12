#ifndef REVEAL_H
#define REVEAL_H
void reveal(const char *path, int show_all, int show_long, const char* home_dir);
void parse_flags_and_path(char *input, int *show_all, int *show_long, char **path) ;
#endif
