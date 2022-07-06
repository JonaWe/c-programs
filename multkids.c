#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage multkids <number>\n");
        exit(-1);
    }

    int childs = atoi(argv[1]);

    printf("Creating %d child processes\n", childs);

    for (int i = 0; i < childs; i++) {
        pid_t pid;

        printf("Before the fork\n");
        pid = fork();

        if (pid == 0) {
            printf("I'm a child\n");
            break;
        } else if(pid == -1) {
            printf("Fork failed! Could not create child process\n");
        } else {
            printf("I'm the parent and I created a child with pid = %d\n", pid);
        }
    }


    exit(0);
}
