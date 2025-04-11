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
    strcat(path,".txt");
    int fd = open(path,O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
    if(fd < 0 && errno != EEXIST){
        perror("Eroare!\n");
        return;
    }
    /*NU VOR FI PERMISE 2 COMORI DE LA ACELASI USER*/
    char buffer[1024];
    bytes = read(fd,buffer,1024);
    if(bytes < 0){
        perror("Citirea nu a reusit!\n");
        return;
    }
    
    int totalLines = 0;
    for(int i=0;i<strlen(buffer);i++){
        if(buffer[i]=='\n'){
            totalLines++;
        }
    }
    char bufferLines[totalLines][1024];
    char *buffToken = strtok(buffer,"\n");
    if(!buffToken){
        perror("Strtok failed! outside for\n");
        return;
    }
    strcpy(bufferLines[0],buffToken);
    for(int j=1;j<totalLines;j++){
        if(j == totalLines - 1){
            buffToken = strtok(NULL,"\n");
            if(!buffToken){
                perror("Strtok failed!\n");
                return;
            }
            strcpy(bufferLines[j],buffToken);
            continue;
        }
        buffToken = strtok(NULL,"\n");
        if(!buffToken){
                perror("Strtok failed!\n");
                return;
        }
        strcpy(bufferLines[j],buffToken);
    }
    char placeHolder[1024]={'\0'};
    char usernames[totalLines][USERNAME_LEN];
    for(int i=0;i<totalLines;i++){
        strcpy(placeHolder,bufferLines[i]);
        strtok(placeHolder,",");
        buffToken = strtok(NULL,",");
        strcpy(usernames[i],buffToken);
        continue;
    }
    
    //
    IDRead[bytes-1] = '\0';
    char UserRead[USERNAME_LEN];
    char Coordinates[256];
    char Clue[CLUE_LEN];
    char Value[256];

    write(1,"Introduceti username -> ",25);
    bytes = read(0,UserRead,USERNAME_LEN);
    if(bytes < 0){
        perror("Eroare citire!\n");
    }
    UserRead[bytes-1] = '\0';

    for(int i=0;i<totalLines;i++){
        if(strcmp(UserRead,usernames[i]) == 0){
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
    write(fd,tres.ID,strlen(tres.ID));
    write(fd,",",1);
    write(fd,tres.userName,strlen(tres.userName));
    write(fd,",",1);
    write(fd,Coordinates,strlen(Coordinates));
    write(fd,",",1);
    write(fd,tres.clue,strlen(tres.clue));
    write(fd,",",1);
    write(fd,Value,strlen(Value));
    write(fd,"\n",1);
    if(close(fd)!= 0){
        perror("Eroare la inchidere!\n");
        return;
    }
    lstat(path,&fileInfo);
    char logPath[256]={'\0'};
    char *fileModifyTime = ctime(&fileInfo.st_mtime);
    sprintf(logPath,"../hunts/%s/%s-log.txt",huntID,tres.ID);
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
    
    sprintf(targetPath,"hunts/%s/%s-log.txt",huntID,tres.ID);
    sprintf(linkPath,"../symlinklog-%s.txt",tres.ID);
    
    if(symlink(targetPath,linkPath) != 0 && errno != EEXIST){
        perror("Eroare la creearea legaturii simbolice pentru log\n");
        return;
    }
}

void view(char *huntID, char *treasureID){
    struct stat fileInfo;
    char path[256] = {'\0'};
     //path va contine calea spre fisierul cu hunt-uri
    /*paramentrul huntID contine \n la final din cauza ca functia read() citeste
    bytes si nu verifica daca am ajuns la newline. In fisierul treasure_manager se
    va citi de la tastatura ID directorului care contine comoara dorita*/
    huntID[strlen(huntID)-1] = '\0'; 
    sprintf(path,"../hunts/%s",huntID);
    printf("%s\n",path);
    /*La treasureID se intampla acelasi lucru ca la huntID, paramentrul este
    citit cu functia read()*/
    //treasureID[strlen(treasureID)-1] = '\0';
    /*presupunem ca huntID = Hunt1, dupa functia strcat, path va fi
    sirul de caractere ../hunts/Hunt1*/
    //strcat(path,huntID);
    /*Vom utiliza lstat pentru a verifica daca directorul huntID exista
    oferim path ca parametru fiindca path reprezinta calea spre huntID*/
    if(strcmp("../hunts/GOTY001",path)==0){
        printf("CPLM");
    }
    if(lstat(path,&fileInfo) == -1){
        perror("Hunt-ul mentionat nu exista!\n");
        return;
    }
    
    /*Daca huntID exista, schimbam calea catre treasureID*/
    //strcat(path,"/");
    //strcat(path,treasureID);
    //path[strlen(path)-2] = '\0';
    //strcat(path,".txt");
    sprintf(path,"../hunts/%s/%s.txt",huntID,treasureID);
    printf("%s\n",path);
    /*verificam daca comoara treasureID exista sau nu*/
    if(lstat(path,&fileInfo) == -1){
        perror("Comoara nu exista!\n");
        return;
    }
    /*Daca comoara exista vom oferi informatiile care se afla inauntrul acesteia*/
    int fd = open(path,O_RDONLY);
    if(fd < 0){
        perror("Nu s-a putut deschide comoara!\n");
        return;
    }
    write(1,"Succes opening file!\n",22);
    /*Avem un buffer in care vom citi informatia din interioriul comorii
    si alte buffere in care stocam informatii legate de coordonate si value*/
    char buffer[1024] = {'\0'};
    int bytes = 0;
    char coordinatesBuffer[256] = {'\0'};
    char valueBuffer[128] = {'\0'};
    int i=0;
    /*tres este folosit pentru a stoca informatia,avem un tablou in caz ca avem
    mai multe comori inserate la acelasi ID*/
    Treasure tres[CHUNK];
    char *token = NULL;
    bytes = read(fd,buffer,1024);
    if(bytes == -1){
        perror("Eroare la citire din fisier!\n");
        return;
    }
    /*Din moment ce functia read() citeste doar bytes si nu face distinctia
    intre un caracter alfanumeric si newline vom face noi separatia*/
    int totalLines = 0;
    for(int j=0;j<strlen(buffer);j++){
        if(buffer[j] == '\n'){
            totalLines++;
        }
    }
    char bufferLines[totalLines][1024];
    char *bufferToken = strtok(buffer,"\n");
    strcpy(bufferLines[0],bufferToken);
    for(int j=1;j<totalLines;j++){
        if(j == totalLines-1){
            bufferToken = strtok(NULL,"\n");
            strcpy(bufferLines[j],bufferToken);
            break;
        }
        bufferToken = strtok(NULL,"\n");
        strcpy(bufferLines[j],bufferToken);
    }

    for(i=0;i<totalLines;i++){
    token = strtok(bufferLines[i],",");
    if(!token){
        perror("Strtok failed for ID!\n");
        return;
    }
    strcpy(tres[i].ID,token);

    token = strtok(NULL,",");
    if(!token){
        perror("Strtok failed for username!\n");
        return;
    }
    strcpy(tres[i].userName,token);

    token = strtok(NULL,",");
    if(!token){
        perror("Strtok failed for latitude!\n");
        return;
    }
    tres[i].coordinates.latitude = atof(token);

    token = strtok(NULL,",");
    if(!token){
        perror("Strtok failed for coordinates longitude!\n");
        return;
    }
    tres[i].coordinates.longitude = atof(token);

    token = strtok(NULL,",");
    if(!token){
        perror("Strtok failed for clue!\n");
        return;
    }
    strcpy(tres[i].clue,token);

    token = strtok(NULL,"\n");
    if(!token){
        perror("Strtok failed! for value\n");
        return;
    }
    tres[i].value = atoi(token);

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
    //treasureID[strlen(treasureID)-1] = '\0';
    sprintf(path,"../hunts/%s/%s.txt",huntID,treasureID);
    //printf("%s\n",path); //debugging
    //strcat(path,"/");
    //strcat(path,treasureID);
    //path[strlen(path)-1] = '\0';
    //strcat(path,".txt");
    if(lstat(path,&fileInfo) == -1){
        perror("Treasure does not exist!\n");
        return;
    }
    int fd = open(path,O_RDWR);
    if(fd < 0){
        perror("Eroare la deschidere fisier!\n");
        return;
    }
    char buffer[1024];
    int bytes = read(fd,buffer,1024);
    if(bytes < 0){
        perror("Citirea nu a reusit!\n");
        return;
    }
    close(fd);
    int totalLines = 0;
    for(int i=0;i<strlen(buffer);i++){
        if(buffer[i]=='\n'){
            totalLines++;
        }
    }
    char bufferLines[totalLines][1024];
    char *buffToken = strtok(buffer,"\n");
    if(!buffToken){
        perror("Strtok failed! outside for\n");
        return;
    }
    strcpy(bufferLines[0],buffToken);
    for(int j=1;j<totalLines;j++){
        if(j == totalLines - 1){
            buffToken = strtok(NULL,"\n");
            if(!buffToken){
                perror("Strtok failed!\n");
                return;
            }
            strcpy(bufferLines[j],buffToken);
            continue;
        }
        buffToken = strtok(NULL,"\n");
        if(!buffToken){
                perror("Strtok failed!\n");
                return;
        }
        strcpy(bufferLines[j],buffToken);
    }
    char placeHolder[1024]={'\0'};
    char usernames[totalLines][USERNAME_LEN];
    for(int i=0;i<totalLines;i++){
        strcpy(placeHolder,bufferLines[i]);
        strtok(placeHolder,",");
        buffToken = strtok(NULL,",");
        strcpy(usernames[i],buffToken);
        continue;
    }
    write(1,"Username-uri prezente in comoara selectata:\n",44);
    for(int i=0;i<totalLines;i++){
        write(1,usernames[i],strlen(usernames[i]));
        write(1,"\n",1);
    }
    write(1,"Introduceti username pe care doriti sa il stergeti -> ",54);
    char choice[USERNAME_LEN] = {'\0'};
    read(0,choice,USERNAME_LEN);
    choice[strlen(choice)-1] = '\0';
    printf("Choice:%s\n",choice);
    fd = open(path,O_TRUNC|O_WRONLY|O_APPEND);
    for(int k=0;k<totalLines;k++){
        if(strcmp(choice,usernames[k]) == 0){
            continue;
        }
        if(k == totalLines-1){
            write(fd,bufferLines[k],strlen(bufferLines[k]));
            write(fd,"\n",1);
            break;
        }
        write(fd,bufferLines[k],strlen(bufferLines[k]));
        write(fd,"\n",1);
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
    char linkLogPath[256] = {'\0'};
    sprintf(logPath,"../hunts/%s/%s-log.txt",huntID,treasureID);
    sprintf(linkLogPath,"../symlinklog-%s.txt",treasureID);
    printf("%s\n",logPath);
    if(fileInfo.st_size == 0){
        if(unlink(logPath) == -1){
            perror("Eroare la stergere log!\n");
            return;
        }
        if(unlink(linkLogPath)==-1){
            perror("Eroare la stergere legatura simbolica!\n");
            return;
        }
        return;
    }
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
    char path[256] = "../hunts/";
    strcat(path,huntID);
    path[strlen(path)-1] = '\0';
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
    int isLog = 0;
    size_t dirNameLen = 0 ;
    struct dirent *dirr = NULL;
    char filePathInsideDirr[1024] = {'\0'};
    while((dirr = readdir(director)) != NULL){
        if(strcmp(dirr->d_name,".")!= 0 && strcmp(dirr->d_name,"..")!=0){
            sprintf(filePathInsideDirr,"%s/%s",path,dirr->d_name);
            dirNameLen = strlen(dirr->d_name);
            if(dirr->d_name[dirNameLen-5]=='g' && dirr->d_name[dirNameLen-6] == 'o' && dirr->d_name[dirNameLen-7] == 'l' && dirr->d_name[dirNameLen-8] == '-'){
                isLog = 1;
            }
            if(isLog == 0){
                sprintf(linkedPath,"../symlinklog-%s",dirr->d_name);
                printf("%s\n",linkedPath);
                if(unlink(linkedPath)!= 0){
                    perror("Eroare la stergere legatura simbolica cu fisierul!\n");
                    closedir(director);
                    return;
                }
            }
            if(unlink(filePathInsideDirr)!= 0){
                perror("Eroare la stergere fisier!\n");
                closedir(director);
                return;
            }
            
        }
        isLog = 0;
    }
    closedir(director);

    if(rmdir(path)==-1){
        perror("File exists but has not been removed!\n");
        return;
    }
    write(1,"Directory removed succesfully!\n",32);
    

}