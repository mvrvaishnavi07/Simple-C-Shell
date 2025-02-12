#ifndef SYS_H
#define SYS_H

void execute_command(char *command, char *home_dir, int run_in_background); 
void setup_signal_handler();  
void background_process_handler(int sig);  
void monitor_background_processes(); 

extern int last_sleep_duration;  

#endif
