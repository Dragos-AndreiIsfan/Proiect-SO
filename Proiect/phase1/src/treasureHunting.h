#ifndef _TREASURE_HUNTING__H_
#define _TREASURE_HUNTING__H_

typedef struct _GPScoordinates GPSCoord;
typedef struct _treasure Treasure;


void add(char *huntID);
void list(char *huntID);
void view(char *huntID,char *treasureID);
void remove_treasure(char *huntID,char *treasureID);
void remove_hunt(char *huntID);




#endif
