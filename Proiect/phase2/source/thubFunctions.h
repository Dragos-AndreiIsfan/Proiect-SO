#ifndef _THUBFUNCTIONS__H_
#define _THUBFUNCTIONS__H_

#include <sys/types.h>
#include "../../phase1/src/treasureHunting.h"

pid_t start_monitor();
void list_hunts();
void list_treasures();
void view_treasure();
void stop_monitor(pid_t *monitorID);
void Exit(pid_t monitorID,int *optiune);


#endif