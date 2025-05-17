#ifndef _CALCULATE_SCORE_H
#define _CALCULATE_SCORE_H

#define ID_LEN 100
#define USERNAME_LEN 120
#define CLUE_LEN 256
#define CHUNK 64
#define SET_CHUNK 32
#define MAX_ELEMENTS 100

struct _GPScoordinates{
    double longitude;
    double latitude;
};

typedef struct setElement{
    char username[USERNAME_LEN];
    int value;
}element;

typedef struct __set__{
    element multime[MAX_ELEMENTS];
    int currentSize;
    int maxSize;
}set;

set createSet();
set calculate_score(char *huntsFolder,set *S);
void printScores(set *S);

#endif