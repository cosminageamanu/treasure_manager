#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char action[256];
    while(1){
        scanf("%255s", action);
        if(strcmp(action, "start_monitor")==0){
            int pid, status;
            if ((pid=fork())<0){
                perror("error creating process");
                exit(-1);
            }
            if (!pid){
                execvp("ls", "ls", "-l", NULL);
            }
        }
        else if (strcmp(action, "list_hunts")==0){

        }
        else if (strcmp(action, "list_treasures")==0){

        }
        else if(strcmp(action, "view_treasure")==0){

        }
        else if(strcmp(action, "stop_monitor")==0){

        }
        else if (strcmp(action, "exit")==0){
            break;
        }
    }

}
