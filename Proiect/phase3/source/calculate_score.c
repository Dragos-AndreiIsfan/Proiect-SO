#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "calculate_score.h"
#include <sys/wait.h>





//ideea mea este de a implementa o structura de tip multime, in care elementele au un nume, user, si o valoare integer


// vom creea operatile necesare multimii

set createSet(){
    set new;
    
    new.currentSize = 0;
    new.maxSize = MAX_ELEMENTS;
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
        return *sets;
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



set readFile(char *path,set *M){
    char ID[ID_LEN];
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

set calculate_score(char *huntFolder,set *M){
    char path[256];
    char huntPath[512];
    char filePath[1024];
    pid_t child = -1;
    int pfd[2];
    sprintf(path,"%s",huntFolder);
    //chdir(huntFolder);
    DIR *direct = opendir(path);
    struct dirent *dirEntry = NULL;
    while((dirEntry = readdir(direct)) != NULL){
        if(strcmp(dirEntry->d_name,".") == 0 ||strcmp(dirEntry->d_name,"..") == 0){
            continue;
        }
        sprintf(huntPath,"%s/%s",path,dirEntry->d_name);
        if(pipe(pfd) < 0){
                perror("Pipe error!\n");
                exit(EXIT_FAILURE);
            }
        if((child = fork())<0){
            perror("Eroare la fork!\n");
            exit(EXIT_FAILURE);
        }
        else if(child == 0){
            close(pfd[0]);
            set S = createSet();
            DIR *newDir = opendir(huntPath);
            struct dirent *dirr = NULL;
            while((dirr=readdir(newDir))!= NULL){
                if(strcmp(dirr->d_name,".") == 0 || strcmp(dirr->d_name,"..") == 0 || strcmp(dirr->d_name,"logged_hunt.txt") == 0){
                    continue;
                }   
                sprintf(filePath,"%s/%s",huntPath,dirr->d_name);
                S = readFile(filePath,M);
            }
            closedir(newDir);
            write(pfd[1],&S,sizeof(S));
            close(pfd[1]);
            exit(0);
        }
        waitpid(child,NULL,0);
        read(pfd[0],M,sizeof(*M));
        close(pfd[0]);
    }
    closedir(direct);
    
    return *M;  
}
    
    
    

