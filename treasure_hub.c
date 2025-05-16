#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>

/*SIGUSR1 - list_hunts
SIGUSR2 - list_treasures
SIGINT - view_treasure
SIGTERM - stop_monitor
SIGCHLD - closed_monitor
*/

int pfd[2];
pid_t monitor_pid = 1;
int closed = 0;

void output(){
    char aux[512];
    int n = read(pfd[0], aux, sizeof(aux)-1);
    if (n>0){
        aux[n]='\0';
        printf("output: %s\n", aux);
    }
}

void list_hunts(){
    pid_t pid = fork();
    if (pid==0){
        char *arg[] = {"./treasure_manager", "--list_hunts", NULL};
        dup2(pfd[1], 1);
        execvp(arg[0],arg);
        perror("error listing hunts");
        exit(0);
    }
    output();
}

void list_treasures(){
    pid_t pid = fork();
    if (pid==0){
        char aux[256];
        scanf("%255s", aux);
        char *arg[] = {"./treasure_manager", "--list", aux, NULL};
        dup2(pfd[1],1);
        execvp(arg[0],arg);
        perror("error listing treasures");
        exit(0);
    }
    output();
}

void view_treasure(){
    pid_t pid = fork();
    if (pid==0){
        printf("hunt: ");
        char hunt[256];
        scanf("%255s", hunt);
        printf("treasure: ");
        char treasure[256];
        scanf("%255s", treasure);
        char *arg[] = {"./treasure_manager", "view_treasure", hunt, treasure, NULL};
        dup2(pfd[1],1);
        execvp(arg[0],arg);
        perror("error viewing treasure");
        exit(0);
    }
    output();
}

void stop_monitor(){
    sleep(10);
    exit(0);
}

void process(){
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
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

void calculate_score(){
    char* arg[] = {"./calculate_score", "calculate_score", NULL};
    execvp(arg[0], arg);
    perror("exec error");
    exit(-1);
}

void start_monitor(){
    //char string[256];
    if (pipe(pfd)<0){
        perror("error creating pipe");
        exit(-1);
    }
    if ((monitor_pid=fork())<0){
        perror("error creating process");
        exit(-1);
    }
    else if (monitor_pid == 0){
        close(pfd[0]);
        dup2(pfd[1], 1);
        /*execlp("ls", "ls", "-l", NULL);
        perror(NULL);
        exit(-1);*/
    }
    else
        printf("Monitor started with pid: %d\n", monitor_pid);
    close(pfd[1]);
    //FILE *f = fdopen(pfd[0], "r"); - open
    //fscanf(f, "%255s", string); - read
    close(pfd[0]);
}

void closed_monitor(){
    closed=1;
}

void wait_stop(){
   while(!closed)
    sleep(1);
}

int main(){
    char action[256];
    int running = 0, status = 0;

    //printf("actions: start_monitor, list_hunts, list_treasures, view_treasure, stop_monitor\n");

    struct sigaction act;
    memset(&act,0,sizeof(struct sigaction));
    act.sa_handler = closed_monitor;
    if (sigaction(SIGCHLD,&act,NULL)<0){
        perror("error sending SIGCHLD");
        exit(-1);
    }
    while(1){
        closed = 0;
        if (monitor_pid==0)
            process();
        printf("action: ");
        scanf("%255s", action);

        if(strcmp(action, "start_monitor")==0){
            if (!running){
                start_monitor();
                running = 1;
            }
            else
                printf("monitor is running\n");
        }

        else if (strcmp(action, "list_hunts")==0){
            if(kill(monitor_pid, SIGUSR1) < 0){
                perror("error sending SIGUSR1");
                exit(-1);
            }
            wait(&status);
        }

        else if (strcmp(action, "list_treasures")==0){
            if(kill(monitor_pid, SIGUSR2) < 0){
                perror("error sending SIGUSR2");
                exit(-1);
            }
            wait(&status);
        }

        else if(strcmp(action, "view_treasure")==0){
            if(kill(monitor_pid, SIGINT) < 0){
                perror("error sending SIGINT");
                exit(-1);
            }
            wait(&status);
        }

        else if(strcmp(action, "stop_monitor")==0){
            running=0;
            if(kill(monitor_pid, SIGTERM) < 0){
                perror("error sending SIGTERM");
                exit(-1);
            }
            wait_stop();
            wait(&status);
            printf("termination status: %d\n", status);
        }

        else if (strcmp(action, "exit")==0){
            if (running)
                printf("The monitor is running\n");
            else
                break;
        }
        else if (strcmp(action, "calculate_score") == 0){

        }

        else
            printf("actions: start_monitor, list_hunts, list_treasures, view_treasure, stop_monitor\n");
    }
    return 0;
}
