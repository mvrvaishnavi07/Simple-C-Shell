#ifndef NEONATE_H
#define NEONATE_H

#include <signal.h>
#include <unistd.h>
pid_t get_most_recent_pid();
void set_nonblocking_mode();
void restore_blocking_mode();
void run_neonate(int time_interval);

#endif 
