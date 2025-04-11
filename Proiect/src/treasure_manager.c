#include <stdio.h>
#include "treasureHunting.h"
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

typedef enum _options{
    CREATE_HUNTS_FOLDER = 1,
    ADD = 2,
    LIST_ALL = 3,
    VIEW = 4,
    REMOVE_TREASURE = 5,
    REMOVE_HUNT = 6,
    EXIT = 7
}options;


void showMenu(){
    
    printf("Here are your options:\n\n");
    printf("1. Create The Folder where your hunts will be stored(if does not exist)\n");
    printf("2. ADD a treasure to a specific hunt(if the hunt does not exist, it will be created)\n");
    printf("3. LIST all of the treasures in a specified hunt\n");
    printf("4. View a specific treasure from a hunt\n");
    printf("5. REMOVE a treasure from a specific hunt\n");
    printf("6. REMOVE a specified hunt\n");
    printf("7. EXIT the Treasure Hunt Manager\n");
    printf("Introduce option ->");
    
}

int verifyHuntsFileExistence(){
    struct stat tempStat;
    if(lstat("../hunts",&tempStat) == -1){
        perror("Directorul \"hunts\" NU A FOST CREAT!\n");
        return -1;
    }
    return 1;
}


void addMenu(){
    char buff[100];
    int bytes = read(0,buff,100);
    if(bytes == -1){
        perror("Something went wrong when reading!\n");
        return;
    }
    buff[bytes-1] = '\0';
    add(buff);
}

void viewMenu(){
    write(1,"Introduce the ID of the hunt you wanna view into -> ",53);
    char huntID[100];
    char treasureID[100];
    int bytes = read(0,huntID,100);
    if(bytes == -1){
        perror("Something went wrong when reading from STDIN!\n");
        return;
    }
    treasureID[bytes-1] = '\0';
    write(1,"Introduce the ID of the treasure you want to see -> ",53);
    bytes = read(0,treasureID,100);
    if(bytes == -1){
        perror("Something went wrong when reading\n");
        return;
    }
    treasureID[bytes-1] = '\0';
    view(huntID,treasureID);
}

void listMenu(){
    write(1,"Introduce the ID of the treasure you want to list -> ",54);
    char huntID[120] = {'\0'};
    int bytes = read(0,huntID,120);
    if(bytes == -1){
        perror("Something went wrong when reading!\n");
        return;
    }
    list(huntID);
}

void removeTreasureMenu(){
    write(1,"Introduce the ID of the hunt you wanna view into -> ",53);
    char huntID[100];
    char treasureID[100];
    int bytes = read(0,huntID,100);
    if(bytes == -1){
        perror("Something went wrong when reading from STDIN!\n");
        return;
    }
    huntID[bytes-1]='\0';
    write(1,"Introduce the ID of the treasure you want to remove from -> ",61);
    bytes = read(0,treasureID,100);
    if(bytes == -1){
        perror("Something went wrong when reading\n");
        return;
    }
    treasureID[bytes-1] = '\0';
    remove_treasure(huntID,treasureID);
}


void removeMenu(){
    write(1,"Introduce ID of the hunt to remove ->",38);
    char buff[100];
    int bytes = read(0,buff,100);
    if(bytes == -1){
        perror("Something went wrong when reading!\n");
        return;
    }
    buff[bytes-1] = '\0';
    remove_hunt(buff);
}

void controlMenu(){
    int options= -1;
    printf("Welcome to the Treasure Hunt manager!\n");
    do{
        showMenu();
        scanf("%d",&options);
        getchar();
        printf("\n\n");
        switch(options){
            case CREATE_HUNTS_FOLDER:
                //create the folder
                if(mkdir("../hunts",0777)== -1){
                    if(errno == EEXIST){
                        perror("Hunts directory already created!\n");
                        break;
                    }
                    else{
                        perror("Directorul de stocare a vanatorilor de comori NU sa creat cu succes!\nIesire!");
                        return ;
                    }
                }
                break;
            case ADD:
                //handle add
                
                if(verifyHuntsFileExistence() == -1){
                    break;
                }
                write(1,"Introduce ID of the hunt->",27);
                addMenu();
                break;
            case LIST_ALL:
                //handle listing
                if(verifyHuntsFileExistence() == -1){
                    break;
                }
                listMenu();
                break;
            case VIEW:
                //handle viewing
                if(verifyHuntsFileExistence() == -1){
                    break;
                }
                viewMenu();
                break;
            case REMOVE_TREASURE:
                //handle this
                if(verifyHuntsFileExistence() == -1){
                    break;
                }
                removeTreasureMenu();
                break;
            case REMOVE_HUNT:
                //handle this
                if(verifyHuntsFileExistence() == -1){
                    break;
                }
                removeMenu();
                break;
            case EXIT:
                printf("Exiting program. Thank you for playing!\n");
                break;
            default:
                printf("Option unrecognized, please introduce another one!\n\n");
                break;
        }
    }while(options != EXIT);
}

int main(void){
    
    controlMenu();

    
    return 0;
}