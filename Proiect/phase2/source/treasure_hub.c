#include <stdio.h>
#include "thubFunctions.h"

typedef enum _options{
    START_MONITOR = 1,
    LIST_HUNTS,
    LIST_TREASURES,
    VIEW_TREASURE,
    STOP_MONITOR,
    EXIT
}opt;

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

void interact(){
    int optiune = -1;
    pid_t idMonitor = -1;
    printf("======VANATOAREA DE COMORI======\n\n");
     printf("=====SUNTETI IN TREASURE HUB=====\n\n");
    do{
        menu();
        scanf("%d",&optiune);
        switch(optiune){
            case START_MONITOR:
                idMonitor = start_monitor();
                break;
            case LIST_HUNTS:
                //list
                break;
            case LIST_TREASURES:
                //list
                break;
            case VIEW_TREASURE:
                //view
                break;
            case STOP_MONITOR:
                //stop_monitor
                stop_monitor(&idMonitor);
                break;
            case EXIT:
                Exit(idMonitor,&optiune);
                printf("Iesire din program, multumim pentru participare, speram ca te-ai distrat!\n");
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