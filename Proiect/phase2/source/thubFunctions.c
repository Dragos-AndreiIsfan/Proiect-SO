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


volatile int working = 1; //volatile - spune compilatorului ca e posibil
//ca variabila sa isi schimbe in timpul rularii valoarea
//introducem o variabila care sa spuna daca monitorul ruleaza in fundal sau nu

void handleCertainSignals(int sig){
    /*semnalul SIGTERM va opri monitorul*/
    if(sig == SIGTERM){
        write(1,"!!=Oprire monitor=!!\n",21);
        exit(0);
    }
    //pentru alte semnale vom vedea mai tarziu

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