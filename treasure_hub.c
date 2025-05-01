#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

pid_t monitor_pid = -1;

void start_monitor(){
    if (monitor_run){
        perror("Monitor is running");
        exit(-1);
    }
    if ((monitor_pid=fork())<0){
        perror("error creating process");
        exit(-1);
    }
}

void list_hunts(){
    char **arg = {"./treasure_manager", "list_hunts", NULL};
    if (execvp(arg[0],arg) == -1){
        perror("error listing hunts");
        exit(-1);
    }
}

void list_treasures(){
    char **arg = {"./treasure_manager", "list_treasures", "",  NULL};
    scanf("%255s", args[2]);
    if (execvp(arg[0],arg) == -1){
        perror("error listing treasures");
        exit(-1);
    }
}

void view_treasure(){
    char **arg = {"./treasure_manager", "view_treasure", "",  "", NULL};
    scanf("%255s", args[2]);
    scanf("%255s", args[3]);
    if (execvp(arg[0],arg) == -1){
        perror("error viewing treasure");
        exit(-1);
    }
}

int main(){
    char action[256];
    int running = 0;
    int status = 0;
    while(1){
        scanf("%255s", action);
        if(strcmp(action, "start_monitor")==0){
            start_monitor(running);
            running = 1;
        }
        else if (strcmp(action, "list_hunts")==0){
            list_hunts();
        }
        else if (strcmp(action, "list_treasures")==0){
            list_treasures();
        }
        else if(strcmp(action, "view_treasure")==0){
            view_treasure();
        }
        else if(strcmp(action, "stop_monitor")==0){
            //stop_monitor();
        }
        else if (strcmp(action, "exit")==0){
            break;
        }
    }
    return 0;
}
