#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define USER_LENGTH 30
#define CLUE_LENGTH 200

#define TREASURE_FILE "treasure"

typedef struct{
    int id;
    char username[USER_LENGTH];
    float latitude;
    float longitude;
    char clue[CLUE_LENGTH];
    int value;
}Treasure_t;

typedef struct{
    char username[USER_LENGTH];
    int score;
}Score_t;

void calculate_score(){
    int value = 0, score = 0;
    char path[100];
    char s[256];
    int fd = open(path, O_RDONLY);
    if (fd == -1){
        perror("error opening file");
        exit(-1);
    }
    while(read(fd, path, sizeof(path)-1)){
        value = atoi(path+96);
        score+=value;
    }
    printf("")

}

int main(int argc, char* argv[]){
    if (argc != 2){
        perror(NULL);
        exit(-1);
    }
    DIR *dir = opendir(".");
    if (!dir){
        perror("error opening directory");
        exit(-1);
    }
    struct dirent *i;
    while ((i=readdir(dir))!=NULL){
        if(i->d_type == DT_DIR && i->d_name[0] != '.'){
            char aux[512];
            snprintf(aux, sizeof(aux), "%s/%s", i->d_name, TREASURE_FILE);
            int fd = open(aux, O_RDONLY);
            if (fd == -1)
                continue;
            struct stat s;
            if (fstat(fd, &s) == -1){
                close(fd);
                continue;
            }
           /* int c = s.st_size / sizeof(Treasure);
            printf("%s: %d treasures\n", i->d_name, c);
            close(fd);
        }*/
    }
    closedir(dir);
    char text[512];
    sprintf(text, "%s/%s", argv[1], TREASURE_FILE);
    /*int fd = open(text, O_RDONLY);
    if (fd==-1){
        perror("error opening file");
        exit(-1);
    }

    close(fd);*/
    return 0;
}
