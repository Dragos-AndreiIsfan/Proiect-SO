#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "calculate_score.h"


#define ID_LEN 100
#define USERNAME_LEN 120
#define CLUE_LEN 256
#define CHUNK 64
#define SET_CHUNK 32

struct _GPScoordinates{
    double longitude;
    double latitude;
};

//ideea mea este de a implementa o structura de tip multime, in care elementele au un nume, user, si o valoare integer


typedef struct setElement{
    char username[USERNAME_LEN];
    int value;
}element;

typedef struct __set__{
    element *multime;
    int currentSize;
    int maxSize;
}set;




// vom creea operatile necesare multimii

set createSet(){
    set new;
    new.multime = (element*)malloc(sizeof(element)*SET_CHUNK);
    if(new.multime == NULL){
        perror("Multimea nu a fost alocata!\n");
        return new;
    }
    new.currentSize = 0;
    new.maxSize = SET_CHUNK;
    return new;
}

int exists(set *set, element el){
    for(int i=0;i<set->currentSize;i++){ // doua elemente sunt identice daca au acelasi username
        if(strcmp(el.username,set->multime[i].username) == 0){
            return i; //returnam pozitia la care gasim elementul
        }
    }
    return -1; // -1 in caz ca nu este in multime
}

set addToSet(set *sets , element el){
    int index = exists(sets,el);
    if(index != -1){
        sets->multime[index].value += el.value;
        return *sets;
    }
    if(sets->currentSize == sets->maxSize){
        sets->maxSize += SET_CHUNK;
        set tmp;
        tmp.multime = (element*)realloc(sets->multime,sizeof(element)*sets->maxSize);
        if(tmp.multime == NULL){
            perror("Eroare la realocare!\nNicio schimbare facuta!\n");
            return *sets;
        }
        sets->multime = tmp.multime;
    }
    sets->multime[sets->currentSize++] = el;
    return *sets;
}

void printScores(set *sets){
    char value[32] = {'\0'};
    for(int i=0;i<sets->currentSize;i++){
        sprintf(value,"%d",sets->multime[i].value);
        write(1,"\n\t===USERNAME: ",15);
        write(1,sets->multime[i].username,strlen(sets->multime[i].username));
        write(1,"===\n",4);
        write(1,"\n\t===VALUE: ",12);
        write(1,value,strlen(value));
        write(1,"===\n\n",5);
    }
}

void freeSet(set *sets){
    
    free(sets->multime);

}

set readFile(char *path,set *M){
    char ID[ID_LEN] = {'\0'};
    struct _GPScoordinates cords;
    char clue[CLUE_LEN];
    char newlineBuffer[2];
    int fd = open(path,O_RDONLY);
    element el;
    if(fd < 0){
        perror("Eroare la deschidre fisier, nicio modificare facuta!\n");
        return *M;
    }
    int bytes = 0;
    while((bytes = read(fd,ID,sizeof(ID))) > 0){
        read(fd,el.username,sizeof(el.username));
        read(fd,&cords.latitude,sizeof(cords.latitude));
        read(fd,&cords.longitude,sizeof(cords.longitude));
        read(fd,clue,sizeof(clue));
        read(fd,&el.value,sizeof(el.value));
        read(fd,newlineBuffer,sizeof(newlineBuffer));
        *M = addToSet(M,el);
    }
    if(close(fd) != 0){
        perror("Eroare la inchidere fisier!\n");
        exit(EXIT_FAILURE);
    }
    return *M;
}

void calculate_score(char *huntFolder,char *huntID){
    char path[512];
    sprintf(path,"%s/%s",huntFolder,huntID);
    //chdir(path);
    set S = createSet();
    if(S.multime == NULL){
        perror("!!EROARE LA CREEARE MULTIME!!\n");
        return;
    }
    DIR *direct = opendir(path);
    struct dirent *dirEntry = NULL;
    while((dirEntry = readdir(direct)) != NULL){
        if(strcmp(dirEntry->d_name,".") == 0 ||strcmp(dirEntry->d_name,"..") == 0 || strcmp(dirEntry->d_name,"logged_hunt.txt") == 0){
            continue;
        }
        sprintf(path,"%s/%s/%s",huntFolder,huntID,dirEntry->d_name);
        S = readFile(path,&S);
        if(S.multime == NULL){
            perror("Eroare la fisier!\n");
            closedir(direct);
            return;
        }
    }
    closedir(direct);
    printScores(&S);
    freeSet(&S);

}