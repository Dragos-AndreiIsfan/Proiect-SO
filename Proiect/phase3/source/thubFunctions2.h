#ifndef _THUBFUNCTIONS2__H_
#define _THUBFUNCTIONS2__H_

#include <sys/types.h>
#include "../../phase1/src/treasureHunting.h"
#include "calculate_score.h"

#define HUNT_LEN 256
#define MAX_HUNTS 50

#define ID_LEN 100
#define USERNAME_LEN 120
#define CLUE_LEN 256
#define CHUNK 64

/*typedef struct _GPScoordinates{
    double longitude;
    double latitude;
}GPSCoord;*/

typedef struct _treasure{
    char ID[ID_LEN];
    char userName[USERNAME_LEN];
    GPSCoord coordinates;
    char clue[CLUE_LEN];
    int value;
}Treasure;

typedef struct _treasuress{
    Treasure tres[100];
    int size;
}Tres;

typedef struct _Treasures{
    char treasureID[ID_LEN];
    char lastModification[256];
}T;

typedef struct huntTotal{
    char huntID[64];
    T treasures[100];
    long int totalSize;
    int treasuresIndex;
}totalTreasure;

typedef struct huntsList{
    char hunts[HUNT_LEN];
    int totalFiles;
}elem;

typedef struct HLE{
    elem list[MAX_HUNTS];
    int currSize;
}HL_t;

typedef enum _options{
    START_MONITOR = 1,
    LIST_HUNTS = 2,
    LIST_TREASURES = 3,
    VIEW_TREASURE = 4,
    CALCULATE_SCORE = 5,
    STOP_MONITOR = 6,
    EXIT = 7
}opt;



pid_t start_monitor(int *pfd);
void list_hunts(char *huntsFolder,HL_t *list);
void sendSignal(pid_t monitorID);
void list_treasures(int option);
void view_treasure(int option);
void stop_monitor(pid_t *monitorID,int *pfd);
void Exit(pid_t monitorID,int *optiune);

#endif