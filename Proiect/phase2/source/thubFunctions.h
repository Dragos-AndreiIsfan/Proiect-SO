#ifndef _THUBFUNCTIONS__H_
#define _THUBFUNCTIONS__H_

#include <sys/types.h>
#include "../../phase1/src/treasureHunting.h"

typedef enum _options{
    START_MONITOR = 1,
    LIST_HUNTS,
    LIST_TREASURES,
    VIEW_TREASURE,
    STOP_MONITOR,
    EXIT
}opt;



pid_t start_monitor();
void list_hunts(char *huntsFolder);
void sendSignal(pid_t monitorID);
void list_treasures(int option);
void view_treasure(int option);
void stop_monitor(pid_t *monitorID);
void Exit(pid_t monitorID,int *optiune);

#endif