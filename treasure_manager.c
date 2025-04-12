#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define USER_LENGTH 30
#define CLUE_LENGTH 200

#define TREASURE_FILE "treasure"
#define LOG_FILE "logged_hunt"

typedef struct{
    int id;
    char username[USER_LENGTH];
    float latitude;
    float longitude;
    char clue[CLUE_LENGTH];
    int value;
}Treasure;


void logging(const char *hunt_id, const char *message){
    char s[256];
    sprintf(s, "%s-%s", hunt_id, LOG_FILE);
    int fd = open(s, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd == -1){
        perror("error opening file");
        exit(-1);
    }
    char buffer[512];
    int len = sprintf(buffer, "[%s] %s\n", hunt_id, message);
    write(fd, buffer, len);
    close(fd);
}


void add(const char *hunt_id){
    struct stat st;
    if (stat(hunt_id, &st) == -1){
    	if (mkdir(hunt_id, 0777)==-1){
        	perror("error creating directory");
        	exit(-1);
    	}
    }

    char s[256];
    sprintf(s, "%s - %s", hunt_id, TREASURE_FILE);
    int fd = open (s, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd == -1){
        perror("error opening file");
        exit(-1);
    }

    Treasure t;
    printf("Treasure ID: ");
    if(scanf("%d", &t.id)!=1){
    perror("scan error");
        exit(-1);
    }

    printf("Username: ");
    if (scanf("%29s", t.username)==0){
        perror("scan error");
        exit(-1);
    }

    printf("Latitude: ");
    if(scanf("%f", &t.latitude)!=1){
    perror("scan error");
        exit(-1);
    }

    printf("Longitude: ");
    if(scanf("%f", &t.longitude)!=1){
    perror("scan error");
        exit(-1);
    }

    printf("Clue: ");
    if(scanf("%199s", t.clue)!=1){
    perror("scan error");
        exit(-1);
    }

    printf("Value: ");
    if(scanf("%d", &t.value)!=1){
    perror("scan error");
        exit(-1);
    }

    write(fd, &t, sizeof(Treasure));
    close(fd);
    logging(hunt_id, "Added treasure");
    char s1[256], s2[256];
    sprintf(s1, "%s - %s", hunt_id, LOG_FILE);
    sprintf(s2, "logged - %s", hunt_id);
    unlink(s2);

    if (symlink(s1, s2) == -1){
        perror("symlink error");
        exit(-1);
    }
}


void list(const char *hunt_id){
    char s[256];
    sprintf(s, "%s - %s", hunt_id, TREASURE_FILE);
    int fd = open(s, O_RDONLY);

    if (fd == -1){
        perror("error opening file");
        exit(-1);
    }

    struct stat st;
    if (stat(s, &st) == -1){
        perror("stat error");
        close(fd);
        exit(-1);
    }

    printf("Hunt: %s\n", hunt_id);
    printf("File size: %ld bytes\n", st.st_size);
    printf("Last modified: %s\n", ctime(&st.st_mtime));

    Treasure t;
    printf("Treasures:\n");
    while(read(fd, &t, sizeof(Treasure)) == sizeof(Treasure))
        printf("ID: %d, Username: %s, Latitude: %f, Longitude: %f, Clue: %s, Value: %d\n", t.id, t.username, t.latitude, t.longitude, t.clue, t.value);
    close(fd);
    logging(hunt_id, "Listed treasures");
}


void view(const char *hunt_id, int treasure_id){
    char s[256];
    sprintf(s, "%s - %s", hunt_id, TREASURE_FILE);
    int fd = open(s, O_RDONLY);

    if (fd == -1){
        perror("error opening file");
        exit(-1);
    }

    Treasure t;
    while(read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)){
        if (t.id == treasure_id){
            printf("Treasure: \n");
            printf("ID: %d, User: %s, Latitude: %f, Longitude: %f, Clue: %s, Value: %d\n", t.id, t.username, t.latitude, t.longitude, t.clue, t.value);
            close(fd);
            logging(hunt_id, "Viewed treasure");
            return;
        }
    }

    printf("Treasure %d not found.\n", treasure_id);
    close(fd);
}


void remove_treasure(const char *hunt_id, int treasure_id){
    char s[256];
    sprintf(s, "%s - %s", hunt_id, TREASURE_FILE);
    int fd = open(s, O_RDWR);

    if (fd == -1){
        perror("error opening file");
        exit(-1);
    }

    Treasure t;
    off_t offset = 0;
    int found = 0;
    while(read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)){
        if (t.id == treasure_id){
            found=1;
            break;
        }
        offset += sizeof(Treasure);
    }

    if (!found){
        printf("Treasure not found.\n");
        close(fd);
        return;
    }

    Treasure t2;
    while (read(fd, &t2, sizeof(Treasure)) == sizeof(Treasure)){
        lseek(fd, -2 * sizeof(Treasure), SEEK_CUR);
        write(fd, &t2, sizeof(Treasure));
        lseek(fd, sizeof(Treasure), SEEK_CUR);
    }

    if (ftruncate(fd, offset + (lseek(fd, 0, SEEK_CUR) - offset - sizeof(Treasure))) == -1){
        perror("truncating error");
        exit(-1);
    }

    close(fd);
    logging(hunt_id, "Removed treasure");
}


void remove_hunt(const char *hunt_id){
    char s[256];
    sprintf(s, "%s - %s", hunt_id, TREASURE_FILE);
    unlink(s);
    sprintf(s, "%s - %s", hunt_id, LOG_FILE);
    unlink(s);
    rmdir(hunt_id);
    char s2[256];
    sprintf(s2, "logged - %s", hunt_id);
    unlink(s2);
    printf("Hunt removed.\n");
}


void usage(){
    printf("Usage:\n");
    printf("--add <hunt_id>\n");
    printf("--list <hunt_id>\n");
    printf("--view <hunt_id> <treasure_id>\n");
    printf("--remove_treasure <hunt_id> <treasure_id>\n");
    printf("--remove_hunt <hunt_id>\n");
}


int main(int argc, char **argv){
    if (argc < 3){
        usage();
        exit(-1);
    }

    if (strcmp(argv[1], "--add") == 0)
        add(argv[2]);
    else if (strcmp(argv[1], "--list") == 0)
        list(argv[2]);
    else if (strcmp(argv[1], "--view") == 0){
        if (argc!=4){
            usage();
            exit(-1);
        }
        view(argv[2], atoi(argv[3]));
    }

    else if (strcmp(argv[1], "--remove_treasure") == 0){
        if (argc != 4){
            usage();
            exit(-1);
        }
        remove_treasure(argv[2], atoi(argv[3]));
    }

    else if (strcmp(argv[1], "--remove_hunt") == 0)
        remove_hunt(argv[2]);
    else{
        usage();
        exit(-1);
    }

    return 0;
}
