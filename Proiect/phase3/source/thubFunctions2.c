#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thubFunctions2.h"
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <time.h>


volatile int working = 1;

int *monitor_pfd = NULL;

void treasureInit(totalTreasure *TT){
    TT->totalSize = 0;
    TT->treasuresIndex = 0;
    for(int i=0;i<100;i++){
        TT->treasures[i].lastModification[0] = '\0';
    }
}



void viewT(char *huntID, char *treasureID,Tres *tres){
    struct stat fileInfo;
    char path[256] = {'\0'};
    
    sprintf(path,"../hunts/%s",huntID);
    
    
    if(lstat(path,&fileInfo) == -1){
        perror("Hunt-ul mentionat nu exista!\n");
        return;
    }
    
  
    sprintf(path,"../hunts/%s/%s.bin",huntID,treasureID);
    
    if(lstat(path,&fileInfo) == -1){
        perror("Comoara nu exista!\n");
        return;
    }
    
    int fd = open(path,O_RDONLY);
    if(fd < 0){
        perror("Nu s-a putut deschide comoara!\n");
        return;
    }
    write(1,"Succes opening file!\n",22);
    
    int bytes = 0;
    char newlineBuffer[2];
   
    int i = 0;
    
    

    while((bytes = read(fd,tres->tres[i].ID,sizeof(tres->tres[i].ID))) > 0){
        read(fd,tres->tres[i].userName,sizeof(tres->tres[i].userName));
        read(fd,&tres->tres[i].coordinates.latitude,sizeof(tres->tres[i].coordinates.latitude));
        read(fd,&tres->tres[i].coordinates.longitude,sizeof(tres->tres[i].coordinates.longitude));
        read(fd,tres->tres[i].clue,sizeof(tres->tres[i].clue));
        read(fd,&tres->tres[i].value,sizeof(tres->tres[i].value));
        read(fd,newlineBuffer,sizeof(newlineBuffer));
        i++;
    }
    tres->size = i;

    
    if(close(fd)!= 0){
        perror("O eroare la inchidere a avut loc!\n");
        return;
    }
}



void listT(char *huntID, totalTreasure *TT){
    struct stat fileInfo;
    char path[256] = {'\0'};
    sprintf(path,"../hunts/%s",huntID);
    if(lstat(path,&fileInfo) == -1){
        perror("Hunt does not exist!\n");
        return;
    }
    treasureInit(TT);
    strcpy(TT->huntID,huntID);
    long int totalFileSize = 0;
    int numberOfFiles = 0;
    char filePath[512] = {'\0'};
    DIR *Director = opendir(path);
    struct dirent *dirr = NULL;
    while((dirr=readdir(Director)) != NULL){
        if(strcmp(dirr->d_name,".")!= 0 && strcmp(dirr->d_name,"..") != 0 && strcmp(dirr->d_name,"logged_hunt.txt")!= 0){
            numberOfFiles++;
            sprintf(filePath,"%s/%s",path,dirr->d_name);
            lstat(filePath,&fileInfo);
            strcpy(TT->treasures[TT->treasuresIndex].lastModification , ctime(&fileInfo.st_mtime));
            strcpy(TT->treasures[TT->treasuresIndex++].treasureID,dirr->d_name);
            totalFileSize += fileInfo.st_size;
        }
    }
    closedir(Director);
    TT->totalSize = totalFileSize;
}

void sendSignal(pid_t monitor){
    kill(monitor,SIGUSR1);
}

void countFiles(char *path,HL_t *listH){
    int totalFiles = 0;
    DIR *director = opendir(path);
    if(!director){
        perror("!!=Directorul nu a fost deschis=!!\n\n");
        exit(EXIT_FAILURE);
        //return -1;
    }
    struct dirent *dirEntry = NULL;
    while((dirEntry = readdir(director)) != NULL){
        if(strcmp(dirEntry->d_name,".") != 0 && strcmp(dirEntry->d_name,"..") != 0){
            if(dirEntry->d_type != DT_DIR && strcmp(dirEntry->d_name,"logged_hunt.txt")!= 0){
                totalFiles++;
            }
        }
    }
    //printf("\n==AVEM %d comori in aceasta vanatoare==\n\n",totalFiles);
    listH->list[listH->currSize++].totalFiles = totalFiles;
    if(closedir(director) != 0){
        perror("!!=Directorul nu a fost inchis=!!\n\n");
    }
    
}

void list_hunts(char *huntsFolder,HL_t *list){
    char path[512];
    DIR *dirr = opendir(huntsFolder);
    if(!dirr){
        perror("\n!!=Directorul NU a fost deschis=!!\n\n");
        return;
    }
    struct dirent *dirE = NULL;
    while((dirE = readdir(dirr)) != NULL){
        if(strcmp(dirE->d_name,".")!=0 && strcmp(dirE->d_name,"..")!=0){
            if(dirE->d_type == DT_DIR){
                strcpy(list->list[list->currSize].hunts,dirE->d_name); 
                sprintf(path,"%s/%s",huntsFolder,dirE->d_name);
                countFiles(path,list);

            }
        }
    }
    closedir(dirr);
}

void processListHuntsCommand(HL_t *list){
    char hunts[] = "../../phase1/hunts";
    list_hunts(hunts,list);
}

void handleCertainSignals(int sig){
    if(sig == SIGTERM){
        working = 0;
        write(1,"!!=Oprire monitor=!!\n",21);
        close(monitor_pfd[0]);
        close(monitor_pfd[1]);
        
        exit(0);
    }
    if(sig == SIGUSR1){
        int cmd = 0;
        read(monitor_pfd[0],&cmd,sizeof(cmd));
        if(cmd < 0){
            perror("Command not read!\n");
            return;
        }
        if(cmd == LIST_HUNTS){
            //procesam list hunts
            HL_t huntsList;
            huntsList.currSize = 0;
            processListHuntsCommand(&huntsList);
            write(monitor_pfd[1],&huntsList,sizeof(huntsList));
            kill(getppid(),SIGUSR2);
            return;
        }
        if(cmd == LIST_TREASURES){
            sleep(2);
            char huntID[64];
            read(monitor_pfd[0],huntID,64);
            if(chdir("../../phase1/src") == -1){
                perror("!!Eroare la schimbare director spre fisierul cu codul din phase1!!\n");
            }
            totalTreasure TT;
            listT(huntID,&TT);
            chdir("../../phase3/source");
            write(monitor_pfd[1],&TT,sizeof(TT));
            kill(getppid(),SIGUSR2);
            return;
        }
        if(cmd == VIEW_TREASURE){
            sleep(2);
            char huntID[64];
            char treasureID[ID_LEN];
            read(monitor_pfd[0],huntID,64);
            sleep(1);
            read(monitor_pfd[0],treasureID,ID_LEN);
            if(chdir("../../phase1/src") == -1){
                perror("!!Eroare la schimbare director spre fisierul cu codul din phase1!!\n");
            }
            Tres T;
            T.size = 0;
            viewT(huntID,treasureID,&T);
            chdir("../../phase3/source");
            write(monitor_pfd[1],&T,sizeof(T));
            kill(getppid(),SIGUSR2);
            return;
        }
    }
}

void monitorSignalHandling(){
    
    struct sigaction action; 
    action.sa_handler = handleCertainSignals; 
    sigemptyset(&action.sa_mask); 
    action.sa_flags = 0; 
    if(sigaction(SIGTERM,&action,NULL) == -1){
        perror("\n✘✘=Ceva nu a mers bine la sigaction() cu semnalul SIGTERM=✘✘\n");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGUSR1,&action,NULL) == -1){
        perror("\n✘✘=Ceva nu a mers bine la sigaction() cu semnalul SIGUSR1=✘✘\n");
        exit(EXIT_FAILURE);
    }
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGUSR2,&action,NULL);
}

void stop_monitor(pid_t *monitorID,int *pfd){

    if(*monitorID == -1){
        write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
        return;
    }
    if(kill(*monitorID,SIGTERM)  == -1){
        perror("\n✘✘=Functia kill nu a trimis semnalul de terminare=✘✘\n");
        exit(EXIT_FAILURE);
    }
    write(1,"\n↺↺=Monitorul se inchide...=↺↺\n",39);
    int status = 0;
    waitpid(*monitorID,&status,0);
    if(WIFEXITED(status)){
        write(1,"✓✓=Monitorul a fost inchis normal=✓✓\n\n",46);
    }
    else{
        perror("!!=Monitorul NU a fost inchis normal=!!\n\n");
    }
    *monitorID = -1;
    close(pfd[0]);
    close(pfd[1]);
}

void monitor(){ //in aceasta functie vom implementa comportamentul monitorului
//cu alte cuvinte, daca primeste un semnal stie ce sa faca, altfel asteapta unul

    monitorSignalHandling(); //apelam functia care prelucreaza semnalele;
    
    while(working){
        pause(); //se asteapta un semnal de la parinte;
   
    }
}


pid_t start_monitor(int *pfd){
    pid_t child = 0;
    if(pipe(pfd)< 0){
        perror("\n!!==Eroare la functia pipe()==!!\n\n");
        return 0;
    }
    if((child = fork())<0){
        perror("\n!!=Procesul pentru monitor nu a fost creat=!!\n");
        return -1;
    }
    if(child == 0){
        //codul copilului
        monitor_pfd = pfd;
        monitor();
    }
    write(1,"\n✓✓=Monitorul a fost deschis=✓✓\n\n",41);
    return child;
}

void Exit(pid_t monitorID,int *optiune){
    if(monitorID != -1){
        *optiune = -1;
        write(1,"\n!!=Monitorul inca nu este inchis=!!\n!!=Va rugam sa il inchideti=!!\n",68);
        return;
    }
    return;
}