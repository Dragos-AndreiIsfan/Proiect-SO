#include <stdio.h>
#include <stdlib.h>
#include "treasureHunting.h"
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>

#define ID_LEN 100
#define USERNAME_LEN 120
#define CLUE_LEN 256
#define CHUNK 64

struct _GPScoordinates{
    double longitude;
    double latitude;
};

struct _treasure{
    char ID[ID_LEN];
    char userName[USERNAME_LEN];
    GPSCoord coordinates;
    char clue[CLUE_LEN];
    int value;

};

void add(char *huntID){
    struct stat fileInfo;
    char path[256]={'\0'};
    //strcpy(path,"../hunts/");
    //huntID[strlen(huntID)-1]='\0';
    //strcat(path,huntID); 
    //path[strlen(path)] = '\0';
    sprintf(path,"../hunts/%s",huntID);
    if(lstat(path,&fileInfo) == -1){
        write(1,"Directorul nu exista,acesta va fi creat!\n",41);
        int dirCheck = mkdir(path,0777);
        if(dirCheck == -1){
            perror("Directorul nu a putut fi creat!\nNu se fac schimbari\n");
            return;
        }
    }
    //path[strlen(path)-1]='\0';

    GPSCoord coordinates;
    Treasure tres;
    int bytes = 0;
    write(1,"Introduceti ID->",17);
    char IDRead[ID_LEN] = {'\0'};
    bytes=read(0,IDRead,ID_LEN);
    if(bytes == -1){
        perror("Eroare la citire!\n");
        return;
    }
    IDRead[bytes-1] = '\0'; 
    strcat(path,"/");
    strcat(path,IDRead);
    //path[strlen(path)-1]='\0';
    strcat(path,".bin");
    int fd = open(path,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
    if(fd < 0 && errno != EEXIST){
        perror("Eroare!\n");
        return;
    }
    /*NU VOR FI PERMISE 2 COMORI DE LA ACELASI USER*/
    Treasure treasArray[CHUNK];
    int arrayIndex = 0;
    int arraySize = 0;
    char newlineBuffer[2];
    

    while((bytes = read(fd,treasArray[arrayIndex].ID,sizeof(treasArray[arrayIndex].ID))) > 0){
        read(fd,treasArray[arrayIndex].userName,sizeof(treasArray[arrayIndex].userName));
        read(fd,&treasArray[arrayIndex].coordinates.latitude,sizeof(treasArray[arrayIndex].coordinates.latitude));
        read(fd,&treasArray[arrayIndex].coordinates.longitude,sizeof(treasArray[arrayIndex].coordinates.longitude));
        read(fd,treasArray[arrayIndex].clue,sizeof(treasArray[arrayIndex].clue));
        read(fd,&treasArray[arrayIndex].value,sizeof(treasArray[arrayIndex].value));
        read(fd,newlineBuffer,sizeof(newlineBuffer));
        arrayIndex++;
        
    }
    arraySize = arrayIndex;
    arrayIndex = 0;
    
    
    //
    IDRead[bytes-1] = '\0';
    char UserRead[USERNAME_LEN] = {'\0'};
    char Coordinates[256];
    char Clue[CLUE_LEN];
    char Value[256];

    write(1,"Introduceti username -> ",25);
    bytes = read(0,UserRead,sizeof(UserRead));
    if(bytes < 0){
        perror("Eroare citire!\n");
    }
    UserRead[bytes-1] = '\0';

    for(arrayIndex=0;arrayIndex<arraySize;arrayIndex++){
        if(strcmp(UserRead,treasArray[arrayIndex].userName) == 0){
            perror("User-ul are deja o comoara la ID-ul acesta\n");
            return;
        }
    }
    write(1,"Introduceti coordonate(in forma x,y) -> ",41);
    bytes = read(0,Coordinates,256);
    if(bytes < 0){
        perror("Eroare citire!\n");
    }
    Coordinates[bytes-1]='\0';
    if(sscanf(Coordinates,"%lf,%lf",&coordinates.latitude,&coordinates.longitude) != 2){
        perror("Eroare la citire!\n");
        return;
    }
    
    write(1,"Introduceti un clue -> ",24);
    bytes = read(0,Clue,CLUE_LEN);
    if(bytes < 0){
        perror("Eroare citire!\n");
    }
    Clue[bytes-1]='\0';

    write(1,"Introduceti un value -> ",25);
    bytes = read(0,Value,256);
    if(bytes < 0){
        perror("Eroare citire!\n");
    }
    Value[bytes-1] = '\0';
    if(sscanf(Value,"%d",&tres.value) != 1){
        perror("Eroare la citire!\n");
        return;
    }
    strcpy(tres.ID,IDRead);
    strcpy(tres.userName,UserRead);
    tres.coordinates.latitude = coordinates.latitude ; tres.coordinates.longitude = coordinates.longitude;
    strcpy(tres.clue,Clue);
    //printf("%s|%s|%lf|%lf|%s|%d\n",tres.ID,tres.userName,tres.coordinates.latitude,tres.coordinates.longitude,tres.clue,tres.value);
    write(fd,tres.ID,sizeof(tres.ID));
    //write(fd,",",1);
    write(fd,tres.userName,sizeof(tres.userName));
    //write(fd,",",1);
    write(fd,&tres.coordinates.latitude,sizeof(tres.coordinates.latitude));
    write(fd,&tres.coordinates.longitude,sizeof(tres.coordinates.longitude));
    //write(fd,",",1);
    write(fd,tres.clue,sizeof(tres.clue));
    //write(fd,",",1);
    write(fd,&tres.value,sizeof(tres.value));
    write(fd,"\n",2);
    if(close(fd)!= 0){
        perror("Eroare la inchidere!\n");
        return;
    }
    lstat(path,&fileInfo);
    char logPath[256]={'\0'};
    char *fileModifyTime = ctime(&fileInfo.st_mtime);
    sprintf(logPath,"../hunts/%s/logged_hunt.txt",huntID);
    printf("%s\n",logPath);
    int logFD = open(logPath,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
    if(logFD < 0 && errno != EEXIST){
        perror("Fisierul log nu a putut fi creat\n");
        return;
    }
    write(logFD,"A fost adaugata o comoara a useru-ului ",39);
    write(logFD,tres.userName,strlen(tres.userName));
    write(logFD," in cufarul cu comori ",22);
    write(logFD,tres.ID,strlen(tres.ID));
    write(logFD," la data ",9);
    write(logFD,fileModifyTime,strlen(fileModifyTime));
    //write(logFD,"\n",1);
    close(logFD);
    char linkPath[512];
    char targetPath[512];
    
    sprintf(targetPath,"hunts/%s/logged_hunt.txt",huntID);
    sprintf(linkPath,"../symlinklog-%s.txt",huntID);
    
    if(symlink(targetPath,linkPath) != 0 && errno != EEXIST){
        perror("Eroare la creearea legaturii simbolice pentru log\n");
        return;
    }
}

void view(char *huntID, char *treasureID){
    struct stat fileInfo;
    char path[256] = {'\0'};
    
    huntID[strlen(huntID)-1] = '\0'; 
    sprintf(path,"../hunts/%s",huntID);
    printf("%s\n",path);
    
    
    if(lstat(path,&fileInfo) == -1){
        perror("Hunt-ul mentionat nu exista!\n");
        return;
    }
    
  
    sprintf(path,"../hunts/%s/%s.bin",huntID,treasureID);
    printf("%s\n",path);
    
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
    char coordinatesBuffer[256] = {'\0'};
    char valueBuffer[128] = {'\0'};
    char newlineBuffer[2];
   
    Treasure tres[CHUNK];
    int i = 0;
    int size = 0;
    
    if(bytes == -1){
        perror("Eroare la citire din fisier!\n");
        return;
    }
    while((bytes = read(fd,tres[i].ID,sizeof(tres[i].ID))) > 0){
        read(fd,tres[i].userName,sizeof(tres[i].userName));
        read(fd,&tres[i].coordinates.latitude,sizeof(tres[i].coordinates.latitude));
        read(fd,&tres[i].coordinates.longitude,sizeof(tres[i].coordinates.longitude));
        read(fd,tres[i].clue,sizeof(tres[i].clue));
        read(fd,&tres[i].value,sizeof(tres[i].value));
        read(fd,newlineBuffer,sizeof(newlineBuffer));
        i++;
        size = i;
    }
    i=0;

    while(i<size){
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
    }
    if(close(fd)!= 0){
        perror("O eroare la inchidere a avut loc!\n");
        return;
    }
}

void list(char *huntID){
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
}

void remove_treasure(char *huntID,char *treasureID){
    struct stat fileInfo;
    char path[256] = {'\0'};
    //huntID[strlen(huntID)-1] = '\0';
    sprintf(path,"../hunts/%s",huntID);
    //strcat(path,huntID);
    if(lstat(path,&fileInfo) == -1){
        perror("The respective hunt does not exist, no changes made\n");
        return;
    }
    
    sprintf(path,"../hunts/%s/%s.bin",huntID,treasureID);
    if(lstat(path,&fileInfo) == -1){
        perror("Treasure does not exist!\n");
        return;
    }
    int fd = open(path,O_RDONLY);
    if(fd < 0){
        perror("Eroare la deschidere fisier!\n");
        return;
    }
    /*char ID[ID_LEN] = {'\0'};
    char username[USERNAME_LEN] ={'\0'};
    GPSCoord coordinates;
    char clue[CLUE_LEN] = {'\0'};
    int value = 0;*/
    Treasure treasure[CHUNK];
    int i=0;
    int size = 0;
    char newlineBuffer[2] = {'\0'};
    char choice[USERNAME_LEN] = {'\0'};

    int bytes = 0;
    while((bytes = read(fd,treasure[i].ID,sizeof(treasure[i].ID)) > 0)){
        read(fd,treasure[i].userName,sizeof(treasure[i].userName));
        read(fd,&treasure[i].coordinates.latitude,sizeof(treasure[i].coordinates.latitude));
        read(fd,&treasure[i].coordinates.longitude,sizeof(treasure[i].coordinates.longitude));
        read(fd,treasure[i].clue,sizeof(treasure[i].clue));
        read(fd,&treasure[i].value,sizeof(treasure[i].value));
        read(fd,newlineBuffer,sizeof(newlineBuffer));
        i++;
    }
    size = i;
    i = 0;
    write(1,"Username-uri prezente in comoara selectata:\n",44);
    for(i=0;i<size;i++){
        write(1,treasure[i].userName,strlen(treasure[i].userName));
        write(1,"\n",1);
    }
    write(1,"Introduceti username pe care doriti sa il stergeti -> ",54);
    close(fd);

    read(0,choice,USERNAME_LEN);
    choice[strlen(choice)-1] = '\0';
    fd = open(path,O_TRUNC|O_WRONLY|O_APPEND);
    for(i=0;i<size;i++){
        if(strcmp(choice,treasure[i].userName) == 0){
            continue;
        }
        write(fd,treasure[i].ID,sizeof(treasure[i].ID));
        write(fd,treasure[i].userName,sizeof(treasure[i].userName));
        write(fd,&treasure[i].coordinates.latitude,sizeof(treasure[i].coordinates.latitude));
        write(fd,&treasure[i].coordinates.longitude,sizeof(treasure[i].coordinates.longitude));
        write(fd,treasure[i].clue,sizeof(treasure[i].clue));
        write(fd,&treasure[i].value,sizeof(treasure[i].value));
        write(fd,"\n",2);
    }
    lstat(path,&fileInfo);
    if(fileInfo.st_size == 0){
        write(1,"Comoara actuala este goala. Cufarul va fi sters!\n",50);
        if(unlink(path) != 0){
            perror("Eroare la stergere fisier!\n");
            return;
        }
    }
    close(fd);
    char logPath[256] = {'\0'};
    char *fileModifyTime = ctime(&fileInfo.st_mtime);
    //char linkLogPath[256] = {'\0'};
    sprintf(logPath,"../hunts/%s/logged_hunt.txt",huntID);
    //sprintf(linkLogPath,"../symlinklog-%s.txt",treasureID);
    printf("%s\n",logPath);
    int logFD = open(logPath,O_RDWR|O_APPEND,S_IRUSR,S_IWUSR);
    if(logFD < 0){
        perror("Eroare la deschidere log\n");
        return;
    }
    write(logFD,"A fost stearsa comoara userului ",32);
    write(logFD,choice,strlen(choice));
    write(logFD," din cufarul cu ID-ul ",22);
    write(logFD,treasureID,strlen(treasureID));
    write(logFD," la data ",9);
    write(logFD,fileModifyTime,strlen(fileModifyTime));
    close(logFD);
   
}

void remove_hunt(char *huntID){
    struct stat fileInfo;
    char path[256] = {'\0'};
    //strcat(path,huntID);
    //path[strlen(path)-1] = '\0';
    sprintf(path,"../hunts/%s",huntID);
    if(lstat(path,&fileInfo) == -1){
        perror("The respective hunt does not exist, no changes made\n");
        return;
    }
    char linkedPath[512] = {'\0'};
    DIR *director = opendir(path);
    if(!director){
        perror("Eroare la deschidere director!\n");
        return;
    }
    //int isLog = 0;
    //size_t dirNameLen = 0 ;
    struct dirent *dirr = NULL;
    char filePathInsideDirr[1024] = {'\0'};
    while((dirr = readdir(director)) != NULL){
        if(strcmp(dirr->d_name,".")!= 0 && strcmp(dirr->d_name,"..")!=0){
            sprintf(filePathInsideDirr,"%s/%s",path,dirr->d_name);
            //dirNameLen = strlen(dirr->d_name);
            /*if(dirr->d_name[dirNameLen-5]=='g' && dirr->d_name[dirNameLen-6] == 'o' && dirr->d_name[dirNameLen-7] == 'l' && dirr->d_name[dirNameLen-8] == '-'){
                isLog = 1;
            }
            if(isLog == 0){
                
                printf("%s\n",linkedPath);
                if(unlink(linkedPath)!= 0){
                    perror("Eroare la stergere legatura simbolica cu fisierul!\n");
                    closedir(director);
                    return;
                }
            }*/
            if(unlink(filePathInsideDirr)!= 0){
                perror("Eroare la stergere fisier!\n");
                closedir(director);
                return;
            }
            
        }
        //isLog = 0;
    }
    closedir(director);
    sprintf(linkedPath,"../symlinklog-%s.txt",huntID);
    printf("%s\n",linkedPath);
    if(rmdir(path)==-1){
        perror("File exists but has not been removed!\n");
        return;
    }
    write(1,"Directory removed succesfully!\n",32);
    if(unlink(linkedPath) == -1){
        perror("Eroare la stergere legatura simbolica\n");
        return;
    }

}