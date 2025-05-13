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

int main(int argc, char* argv[]){
    if (argc != 2){
        perror(NULL);
        exit(-1);
    }
    char text[256];
    sprintf(text, "%s/%s", argv[1], TREASURE_FILE);
    int fd = open(text, O_RDONLY);
    if (fd==-1){
        perror("error opening file");
        exit(-1);
    }

    close(fd);
    return 0;
}
