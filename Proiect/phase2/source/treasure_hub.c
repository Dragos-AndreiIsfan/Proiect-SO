#include <stdio.h>
#include "thubFunctions.h"
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

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
void menu(){
    printf("===Sunt valabile urmatoarele optiuni===\n");
    printf("\t1.Porneste monitorul\n");
    printf("\t2.Listeaza vanatoarea\n");
    printf("\t3.Listeaza comori\n");
    printf("\t4.Vizualieaza o comoara\n");
    printf("\t5.Opreste monitor\n");
    printf("\t6.Iesire\n");
    printf("=======================================\n");
    printf("Introduceti optiune ->");
}

void readFromInputWriteInOutput(int output,int input){
    char buff[256];
    int bytes = read(input,buff,256);
    if(bytes < 0){
        perror("eroare la citire din fisier1\n");
        return;
    }
    //buff[bytes-1] = '\0';
    write(output,buff,bytes);

}

void interact(){
    int optiune = -1;
    int fd = 0;
    pid_t idMonitor = -1;
    printf("======VANATOAREA DE COMORI======\n\n");
    printf("=====SUNTETI IN TREASURE HUB=====\n\n");
    parentSigHandler();
    do{
        menu();
        scanf("%d",&optiune);
        switch(optiune){
            case START_MONITOR:
                if(idMonitor != -1){break;}
                fd = open("command.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
                if(fd < 0){
                    perror("==!!Eroare la deschidere fisier!!==\n");
                    break;
                }
                idMonitor = start_monitor();
                if(close(fd) != 0){
                    perror("==!!Eroare la inchidere fisier!!==\n");
                }
                break;
            case LIST_HUNTS:
                //list
                parentReady = -1;
                if(idMonitor == -1){
                    write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
                    break;
                }   
                fd = open("command.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
                if(fd < 0){
                    perror("==!!Eroare la deschidere fisier!!==\n");
                    break;
                }
                write(fd,"LIST_HUNTS",11);
                if(close(fd) != 0){
                    perror("==!!Eroare la inchidere fisier!!==\n");
                }
                sendSignal(idMonitor);
                while(parentReady == -1){
                    pause();
                }
                break;
            case LIST_TREASURES:
                //list
                parentReady = -1;
                if(idMonitor == -1){
                    write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
                    break;
                }   
                fd = open("command.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
                if(fd < 0){
                    perror("==!!Eroare la deschidere fisier!!==\n");
                    break;
                }
                write(fd,"LIST_TREASURES",15);
                if(close(fd) != 0){
                    perror("==!!Eroare la inchidere fisier!!==\n");
                }
                int fd1 = open("huntID.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
                if(fd1 < 0){
                    perror("Eroare la creeare fisier cu huntID!\n");
                    break;
                }
                write(1,"Introduceti huntID:",19);
                readFromInputWriteInOutput(fd1,0);
                if(close(fd1) < 0){
                    perror("Eroare la inchidere fisier!\n");
                }
                sendSignal(idMonitor);
                while(parentReady == -1){
                    pause();
                }
                break;
            case VIEW_TREASURE:
                //view
                parentReady = -1;
                if(idMonitor == -1){
                    write(1,"\n!!=Monitorul nu este deschis=!!\n\n",34);
                    break;
                }   
                fd = open("command.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
                if(fd < 0){
                    perror("==!!Eroare la deschidere fisier!!==\n");
                    break;
                }
                write(fd,"VIEW_TREASURE",14);
                if(close(fd) != 0){
                    perror("==!!Eroare la inchidere fisier!!==\n");
                }
                fd1 = open("huntID.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
                if(fd1 < 0){
                    perror("Eroare la creeare fisier cu huntID!\n");
                    break;
                }
                write(1,"Introduceti huntID:",19);
                readFromInputWriteInOutput(fd1,0);
                if(close(fd1) < 0){
                    perror("Eroare la inchidere fisier!\n");
                }
                int fdTreasure = open("treasure.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
                if(fdTreasure < 0){
                    perror("Eroare la creearea fisierului text cu id treasure!\n");
                    return;
                }
                write(1,"Introduceti treasureID:",23);
                readFromInputWriteInOutput(fdTreasure,0);
                if(close(fdTreasure) != 0){
                    perror("Eroare la inchidere fisier!\n");
                    break;
                }
                sendSignal(idMonitor);
                while(parentReady == -1){
                    pause();
                }
                break;
            case STOP_MONITOR:
                //stop_monitor
                stop_monitor(&idMonitor);
                break;
            case EXIT:
                Exit(idMonitor,&optiune);
                if(optiune != -1){
                    printf("Iesire din program, multumim pentru participare, speram ca te-ai distrat!\n");
                }
                break;
            default:
                printf("Optiune invalida\n\n");
        }
    }while(optiune != EXIT);
}

int main(void){
    
    interact();

    return 0;
}