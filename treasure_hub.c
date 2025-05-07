#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

/*SIGUSR1 - list_hunts
SIGUSR2 - list_treasures
SIGINT - view_treasure
SIGTERM - stop_monitor
*/
pid_t monitor_pid = -1;
int monitor_run = 0;

void list_hunts(){
    char *arg[] = {"./treasure_manager", "list_hunts", NULL};
    if (execvp(arg[0],arg) == -1){
        perror("error listing hunts");
        exit(-1);
    }
}

void list_treasures(){
    char aux[256];
    scanf("%255s", aux);
    char *arg[] = {"./treasure_manager", "list", aux, NULL};
    if (execvp(arg[0],arg) == -1){
        perror("error listing treasures");
        exit(-1);
    }
}

void view_treasure(){
    printf("hunt: ");
    char aux[256];
    scanf("%255s", aux);
    printf("treasure: ");
    char aux2[256];
    scanf("%255s", aux2);
    char *arg[] = {"./treasure_manager", "view_treasure", aux, aux2, NULL};
    if (execvp(arg[0],arg) == -1){
        perror("error viewing treasure");
        exit(-1);
    }
}

void stop_monitor(){
    usleep(10);
    exit(0);
}

void process(){
    struct sigaction act;
    memset(&act, 0x00, sizeof(struct sigaction));
    act.sa_handler = list_hunts;
    if (sigaction(SIGUSR1, &act, NULL) == -1){
        perror("error sending SIGUSR1");
        exit(-1);
    }
    act.sa_handler = list_treasures;
    if (sigaction(SIGUSR2, &act, NULL) == -1){
        perror("error sending SIGUSR2");
        exit(-1);
    }
    act.sa_handler = view_treasure;
    if (sigaction(SIGINT, &act, NULL) == -1){
        perror("error sending SIGINT");
        exit(-1);
    }
    act.sa_handler = stop_monitor;
    if (sigaction(SIGTERM, &act, NULL) == -1){
        perror("error sending SIGTERM");
        exit(-1);
    }
    while(1);
}

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

void wait_stop(){
    
}

int main(){
    char action[256];
    int running = 0;
    int status;
    while(1){
        monitor_run = 0;
        if (!monitor_pid)
            process();
        printf("action: ");
        scanf("%255s", action);
        if(strcmp(action, "start_monitor")==0){
            if (!monitor_run){
                start_monitor();
                running = 1;
                printf("monitor has started\n");
            }
        }
        else if (strcmp(action, "list_hunts")==0){
            if(kill(monitor_pid, SIGUSR1) < 0){
                perror("error sending SIGUSR1");
                exit(-1);
            }
            wait(&status);
            start_monitor();
        }
        else if (strcmp(action, "list_treasures")==0){
            if(kill(monitor_pid, SIGUSR2) < 0){
                perror("error sending SIGUSR2");
                exit(-1);
            }
            wait(&status);
            start_monitor();
        }
        else if(strcmp(action, "view_treasure")==0){
            if(kill(monitor_pid, SIGINT) < 0){
                perror("error sending SIGINT");
                exit(-1);
            }
            wait(&status);
            start_monitor();
        }
        else if(strcmp(action, "stop_monitor")==0){
            running=0;
            if(kill(monitor_pid, SIGTERM) < 0){
                perror("error sending SIGTERM");
                exit(-1);
            }
            wait(&status);
        }
        else if (strcmp(action, "exit")==0){
            if (running)
                printf("The monitor is running\n");
            else
                break;
        }
        else
            printf("actions: start_monitor, list_hunts, list_treasures, view_treasure, stop_monitor\n");
    }
    return 0;
}
