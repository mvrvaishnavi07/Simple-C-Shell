#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>
#include <sys/types.h>
// Declare global variable to track the foreground process PID
extern pid_t current_foreground_pid;

void setup_signal_handlerr(); 
void send_signal_to_process(pid_t pid, int signal_number);

void sigint_handler(int sig);    
void sigtstp_handler(int sig);

#endif
