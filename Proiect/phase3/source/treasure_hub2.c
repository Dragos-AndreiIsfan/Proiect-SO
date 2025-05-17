#include <stdio.h>
#include "thubFunctions2.h"
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "calculate_score.h"
#include <dirent.h>
#include <sys/wait.h>

volatile int parentReady = -1;
void parentSignalHandle(int sig){
    if(sig == SIGUSR2){
        parentReady = 1;
    }
    
}
void parentSigHandler(){
    struct sigaction action; 
    action.sa_handler = parentSignalHandle; 
    sigemptyset(&action.sa_mask); 
    action.sa_flags = 0; 
    if(sigaction(SIGUSR2,&action,NULL) == -1){
        perror("\n✘✘=Ceva nu a mers bine la sigaction() cu semnalul SIGUSR2=✘✘\n");
        exit(EXIT_FAILURE);
    }
}







    




void printViewTreasure(Tres *T){
    char coordinatesBuffer[256] = {'\0'};
    char valueBuffer[128] = {'\0'};
    for(int i=0;i<T->size;i++){
        sprintf(coordinatesBuffer,"%lf,%lf",T->tres[i].coordinates.latitude,T->tres[i].coordinates.longitude);
        sprintf(valueBuffer,"%d",T->tres[i].value);

        write(1,"\nID:",4);
        write(1,T->tres[i].ID,strlen(T->tres[i].ID));
        write(1,"\n------\n",8);
        write(1,"Username:",10);
        write(1,T->tres[i].userName,strlen(T->tres[i].userName));
        write(1,"\n------\n",8);
        write(1,"Coordinates(Latitude,Longitude):",33);
        write(1,coordinatesBuffer,strlen(coordinatesBuffer));
        write(1,"\n------\n",8);
        write(1,"Clue:",6);
        write(1,T->tres[i].clue,strlen(T->tres[i].clue));
        write(1,"\n------\n",8);
        write(1,"Value:",7);
        write(1,valueBuffer,strlen(valueBuffer));
        write(1,"\n------\n\n\n",10);
    }
}

void printHuntList(HL_t *list){
    for(int i=0;i<list->currSize;i++){
        printf("\n==AVEM HUNT-UL %s==\n",list->list[i].hunts);
        printf("=AVEM %d COMORI IN ACEST HUNT=\n",list->list[i].totalFiles);
    }
    printf("\n\n");
}

void printListTreasures(totalTreasure *TT){
    printf("\n==Nume hunt: %s==\n",TT->huntID);
    printf("==Total File Size : %ld==\n",TT->totalSize);
    printf("==Ultimele Modificari==\n");
    for(int i=0;i<TT->treasuresIndex;i++){
        printf("\t%s->%s\t\n",TT->treasures[i].treasureID,TT->treasures[i].lastModification);
    }
    printf("==Comori==\n");
    for(int i=0;i<TT->treasuresIndex;i++){
        printf("\t%s\t\n",TT->treasures[i].treasureID);
    }
    printf("\n");
}

void menu(){
    printf("===Sunt valabile urmatoarele optiuni===\n");
    printf("\t1.Porneste monitorul\n");
    printf("\t2.Listeaza vanatoarea\n");
    printf("\t3.Listeaza comori\n");
    printf("\t4.Vizualieaza o comoara\n");
    printf("\t5.Calculate Score\n");
    printf("\t6.Opreste monitor\n");
    printf("\t7.Iesire\n");
    printf("=======================================\n");
    printf("Introduceti optiune ->");
}

void interact(){
    pid_t idMonitor = -1;
    int optiune = 0;
    int pfd[2];
    char huntID[64];
    char tid[ID_LEN];
    int bytes = 0;
    printf("======VANATOAREA DE COMORI======\n\n");
    printf("=====SUNTETI IN TREASURE HUB=====\n\n");
    parentSigHandler();
    do{
        menu();
        scanf("%d",&optiune);
        switch(optiune){
            case START_MONITOR:
                if(idMonitor != -1){break;}
                idMonitor = start_monitor(pfd);
                break;
            case LIST_HUNTS:
                if(idMonitor == -1){
                    write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
                    break;
                }
                parentReady = -1;
                write(pfd[1],&optiune,sizeof(optiune));
                sendSignal(idMonitor);
                //sleep(2);
                while(parentReady == -1){
                    pause();
                }
                HL_t list;
                read(pfd[0],&list,sizeof(list));
                printHuntList(&list);
                break;
            case LIST_TREASURES:
                if(idMonitor == -1){
                    write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
                    break;
                }   
                parentReady = -1;
                write(pfd[1],&optiune,sizeof(optiune));
                sendSignal(idMonitor);
                
                write(1,"Introduceti id-ul hunt-ului: ",29);
                bytes = read(0,huntID,64);
                if(bytes < 0){
                    perror("Citire esuata!\n");
                    exit(EXIT_FAILURE);
                }
                huntID[bytes-1] = '\0';
                write(pfd[1],huntID,strlen(huntID)+1);
                while(parentReady == -1){
                    pause();
                }
                totalTreasure TT;
                read(pfd[0],&TT,sizeof(TT));
                printListTreasures(&TT);
                break;
            case VIEW_TREASURE:
                if(idMonitor == -1){
                    write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
                    break;
                }
                parentReady = -1;
                write(pfd[1],&optiune,sizeof(optiune));
                sendSignal(idMonitor);
                write(1,"Introduceti id-ul hunt-ului: ",29);
                bytes = read(0,huntID,64);
                if(bytes < 0){
                    perror("Citire esuata!\n");
                    exit(EXIT_FAILURE);
                }
                huntID[bytes-1] = '\0';
                write(1,"Introduceti id-ul treasure-ului: ",33);
                bytes = read(0,tid,ID_LEN);
                tid[bytes-1] = '\0';
                write(pfd[1],huntID,strlen(huntID)+1);
                sleep(1);
                write(pfd[1],tid,strlen(tid)+1);
                while(parentReady == -1){
                    pause();
                }
                Tres T;
                read(pfd[0],&T,sizeof(T));
                printViewTreasure(&T);
                break;
            case CALCULATE_SCORE:
                if(idMonitor == -1){
                    write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
                    break;
                }
                set S = createSet();
                S = calculate_score("../../phase1/hunts",&S);
                write(1,"\n==SCORURI IN HUNTS==\n",22);
                printScores(&S);
                write(1,"\n==============\n\n",17);
                break;
            case STOP_MONITOR:
                //stop
                stop_monitor(&idMonitor,pfd);
                break;
            case EXIT:
                Exit(idMonitor,&optiune);
                if(optiune != -1){
                    printf("Iesire din program, multumim pentru participare, speram ca te-ai distrat!\n");
                }
                break;
            default:
                printf("Optiune invalida\n\n");
                break;
        }
    }while(optiune != EXIT);
}

int main(void){
    
    interact();

    return 0;
}