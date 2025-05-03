#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thubFunctions.h"
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

char huntsPath[256];

volatile int working = 1; //volatile - spune compilatorului ca e posibil
//ca variabila sa isi schimbe valoarea in timpul rularii programului
//introducem o variabila care sa spuna daca monitorul ruleaza in fundal sau nu



int countFiles(char *path){
    int totalFiles = 0;
    DIR *director = opendir(path);
    if(!director){
        perror("!!=Directorul nu a fost deschis=!!\n\n");
        return -1;
    }
    struct dirent *dirEntry = NULL;
    while((dirEntry = readdir(director)) != NULL){
        if(strcmp(dirEntry->d_name,".") != 0 && strcmp(dirEntry->d_name,"..") != 0){
            if(dirEntry->d_type != DT_DIR && strcmp(dirEntry->d_name,"logged_hunt.txt")!= 0){
                totalFiles++;
            }
        }
    }
    printf("\n==AVEM %d comori in aceasta vanatoare==\n\n",totalFiles);
    if(closedir(director) != 0){
        perror("!!=Directorul nu a fost inchis=!!\n\n");
    }
    return totalFiles;
}


void sendSignal(pid_t monitor){
    kill(monitor,SIGUSR1);
}


void list_hunts(char *huntsFolder){
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
                write(1,"\n!!==AVEM VANATOAREA ",21);
                write(1,dirE->d_name,strlen(dirE->d_name));
                write(1,"==!!\n\n",6);
                sprintf(path,"%s/%s",huntsFolder,dirE->d_name);
                if(countFiles(path) == -1){
                    return;
                }
            }
        }
    }
    closedir(dirr);
}

int readCommand(){
    int fd = open("command.txt",O_RDONLY);
    char commandRead[256] = {'\0'};
    int bytes = read(fd,commandRead,256);
    if(bytes < 0){
        perror("Eroare la citire comanda!\n");
        return -1;
    }
    //commandRead[bytes] = '\0';
    if(strcmp(commandRead,"LIST_HUNTS") == 0){
        return 2;
    }
    else if(strcmp(commandRead,"LIST_TREASURES") == 0){
        return 3;
    }
    else if(strcmp(commandRead,"VIEW_TREASURE") == 0){
        return 4;
    }
    return -1;
}

void readHuntID(char *stringToStoreID,int stringSize,char *fileName){
    int fd = open(fileName,O_RDONLY);
    if(fd < 0){
        perror("Citire esuata!\n");
        return;
    }
    int bytes = read(fd,stringToStoreID,stringSize);
    if(bytes < 0){
        perror("Citire huntID esuata\n");
        return;
    }
    if(close(fd) != 0){
        perror("Eroare la inchidere fisier!\n");
        return;
    }
    //stringToStoreID[bytes-1] = '\0';
}

void readTreasureID(char *destination,int destSize, char *fileName){
    int fd = open(fileName,O_RDONLY);
    if(fd < 0){
        perror("Citire esuata!\n");
        return;
    }
    int bytes = read(fd,destination,destSize);
    if(bytes < 0){
        perror("Citire huntID esuata\n");
        return;
    }
    destination[bytes - 1] = '\0';
    if(close(fd) != 0){
        perror("Eroare la inchidere fisier!\n");
        return;
    }
}

void processListHuntsCommand(){
    //int fd = 0;
    //char toHunts[256];
    char hunts[] = "../../phase1/hunts";
    //int bytes = 0;
    /*fd = open("command.txt",O_RDONLY);
    if(fd < 0){
        perror("FIle not opened!\n");
        return;
    }*/
    //bytes = read(fd,toHunts,256);
    //if(bytes < 0){
    //    perror("Eroare la citire din fisier!\n");
   // }
    //toHunts[bytes-1] = '\0';
    //sprintf(huntsPath,"../../")
    list_hunts(hunts);
}


void stop_monitor(pid_t *monitorID){

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
}

void Exit(pid_t monitorID,int *optiune){
    if(monitorID != -1){
        *optiune = -1;
        write(1,"\n!!=Monitorul inca nu este inchis=!!\n!!=Va rugam sa il inchideti=!!\n",68);
        return;
    }

    return;
}



void handleCertainSignals(int sig){
    /*semnalul SIGTERM va opri monitorul*/
    if(sig == SIGTERM){
        working = 0;
        write(1,"!!=Oprire monitor=!!\n",21);
        exit(0);
    }
    if(sig == SIGUSR1){
        int command = readCommand();
        if(command < 0){
            perror("Command not read!\n");
            return;
        }
        if(command == LIST_HUNTS){
            processListHuntsCommand();
            fflush(stdout);
            kill(getppid(),SIGUSR2);
        }
        else if(command == LIST_TREASURES){
            char huntID[256] = {'\0'};
            readHuntID(huntID,256,"huntID.txt");
            if(strcmp(huntID,"\0") == 0){
                return;
            }
            if(chdir("../../phase1/src") == -1){
                perror("Eroare la schimbare director spre fisierul cu codul din phase1\n");
            }
            list(huntID);
            //write(1,"Commence operation list treasures\n",34);
            fflush(stdout);
            chdir("../../phase2/source");
            kill(getppid(),SIGUSR2);
        }
        else if(command == VIEW_TREASURE){
            char huntID[256] = {'\0'};
            char treasureID[256] = {'\0'};
            readHuntID(huntID,256,"huntID.txt");
            readTreasureID(treasureID,256,"treasure.txt");
            if(strcmp(huntID,"\0") == 0){
                return;
            }
            if(strcmp(treasureID,"\0") == 0){
                return;
            }
            if(chdir("../../phase1/src") == -1){
                perror("Eroare la schimbare director spre fisierul cu codul din phase1\n");
            }
            view(huntID,treasureID);
            //write(1,"Commence operation list treasures\n",34);
            fflush(stdout);
            chdir("../../phase2/source");
            kill(getppid(),SIGUSR2);
        }
    }

}

void monitorSignalHandling(){
    /*in aceasta functie vom implementa toate actiunile
    pentru a putea lucra cu semnale
    Vom lucra cu sigaction, conform cerintei proiectului*/
    struct sigaction action; //creem structura cu care vom lucra pentru action handling
    action.sa_handler = handleCertainSignals; /*atribuim campului sa_handler functia handleExit care se va ocupa de proces in cazul in care se receptioneaza un semnal*/
    sigemptyset(&action.sa_mask); //seteaza multimea semnalelor de blocat din sa_mask la multimea vida
    action.sa_flags = 0; //nu ne intereseaza flag-uri specifice
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

void monitor(){ //in aceasta functie vom implementa comportamentul monitorului
//cu alte cuvinte, daca primeste un semnal stie ce sa faca, altfel asteapta unul

    monitorSignalHandling(); //apelam functia care prelucreaza semnalele;
    
    while(working){
        pause(); //se asteapta un semnal de la parinte;
   
    }
}


pid_t start_monitor(){
    pid_t monitorID = fork();
    if(monitorID < 0){
        perror("\n!!=Procesul pentru monitor nu a fost creat=!!\n");
        return 0;
    }
    if(monitorID == 0){
        //tratam cazul de executie al codului copilului
        //trebuie sa creem posibilitatea de a accpeta semnale
        monitor();

    }
    write(1,"\n✓✓=Monitorul a fost deschis=✓✓\n\n",41);
    return monitorID; //returnam ID-ul procesului descris de variabila 
    //pentru a putea fi folosita la oprirea monitorului mai tarziu
}