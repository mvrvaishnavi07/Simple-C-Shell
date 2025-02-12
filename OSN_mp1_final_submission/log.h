#ifndef LOG_H
#define LOG_H

void init_log(const char *home_dir);
void save_log(const char *home_dir);
void add_to_log(const char *command, const char *home_dir);
void show_log();
void purge_log();
void execute_log_command(int index);

#endif
