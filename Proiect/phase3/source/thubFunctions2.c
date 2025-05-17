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

/*char huntsPath[256];

 //volatile - spune compilatorului ca e posibil
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
    fd = open("command.txt",O_RDONLY);
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
    /*list_hunts(hunts);
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
    semnalul SIGTERM va opri monitorul*/
    /*if(sig == SIGTERM){
        working = 0;
        write(1,"!!=Oprire monitor=!!\n",21);
        exit(0);
        return;
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
                perror("!!Eroare la schimbare director spre fisierul cu codul din phase1!!\n");
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
    in aceasta functie vom implementa toate actiunile
    pentru a putea lucra cu semnale
    Vom lucra cu sigaction, conform cerintei proiectului*/
    /*struct sigaction action; //creem structura cu care vom lucra pentru action handling
    action.sa_handler = handleCertainSignals; atribuim campului sa_handler functia handleExit care se va ocupa de proces in cazul in care se receptioneaza un semnal*/
    /*sigemptyset(&action.sa_mask); //seteaza multimea semnalelor de blocat din sa_mask la multimea vida
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
}*/

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
    /*char coordinatesBuffer[256] = {'\0'};
    char valueBuffer[128] = {'\0'};*/
    char newlineBuffer[2];
   
    int i = 0;
    
    
    /*if(bytes == -1){
        perror("Eroare la citire din fisier!\n");
        return;
    }*/
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

    /*while(i<size){
    sprintf(coordinatesBuffer,"%lf,%lf",tres[i].coordinates.latitude,tres[i].coordinates.longitude);
    sprintf(valueBuffer,"%d",tres[i].value);

    write(1,"\nID:",4);
    write(1,tres[i].ID,strlen(tres[i].ID));
    write(1,"\n------\n",8);
    write(1,"Username:",10);
    write(1,tres[i].userName,strlen(tres[i].userName));
    write(1,"\n------\n",8);
    write(1,"Coordinates(Latitude,Longitude):",33);
    write(1,coordinatesBuffer,strlen(coordinatesBuffer));
    write(1,"\n------\n",8);
    write(1,"Clue:",6);
    write(1,tres[i].clue,strlen(tres[i].clue));
    write(1,"\n------\n",8);
    write(1,"Value:",7);
    write(1,valueBuffer,strlen(valueBuffer));
    write(1,"\n------\n\n",8);
    i++;
    }*/
    if(close(fd)!= 0){
        perror("O eroare la inchidere a avut loc!\n");
        return;
    }
}

/*void list(char *huntID){
    struct stat fileInfo;
    char path[256] = {'\0'};
    huntID[strlen(huntID)-1] = '\0'; 
    sprintf(path,"../hunts/%s",huntID);
    if(lstat(path,&fileInfo) == -1){
        perror("Hunt does not exist!\n");
        return;
    }
    write(1,"Nume Director: ",15);
    write(1,huntID,strlen(huntID));
    write(1,"\n",1);
    long int totalFileSize = 0;
    int numberOfFiles = 0;
    char filePath[512] = {'\0'};
    char *fileModification = NULL;
    write(1,"\nUltimele Modificari: \n",23);
    DIR *Director = opendir(path);
    struct dirent *dirr = NULL;
    while((dirr=readdir(Director)) != NULL){
        if(strcmp(dirr->d_name,".")!= 0 && strcmp(dirr->d_name,"..") != 0){
            numberOfFiles++;
            sprintf(filePath,"%s/%s",path,dirr->d_name);
            lstat(filePath,&fileInfo);
            fileModification = ctime(&fileInfo.st_mtime);
            write(1,"Fisierul ",10);
            write(1,dirr->d_name,strlen(dirr->d_name));
            write(1,"->",2);
            write(1,fileModification,strlen(fileModification));
            write(1,"\n",1);
            totalFileSize += fileInfo.st_size;
        }
    }
    closedir(Director);
    Director = opendir(path);
    if(Director ==NULL){
        perror("Problem opening director!\n");
        return;
    }
    char IDs[numberOfFiles][ID_LEN];
    int i=0;
    while((dirr=readdir(Director))!= NULL){
         if(strcmp(dirr->d_name,".")!= 0 && strcmp(dirr->d_name,"..") != 0){
            sprintf(filePath,"%s/%s",path,dirr->d_name);
            strcpy(IDs[i],dirr->d_name);
            i++;
         }
    }
    closedir(Director);
    char number[256];
    sprintf(number,"%ld",totalFileSize);
    write(1,"Total size of Director: ",25);
    write(1,number,strlen(number));
    write(1,"\nTreasure IDs:",15);
    for(i = 0;i<numberOfFiles;i++){
        write(1,IDs[i],strlen(IDs[i]));
        write(1,"\n",1);
    }
}*/

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