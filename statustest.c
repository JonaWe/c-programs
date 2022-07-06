#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
    pid_t cpid, tpid;
    int status;

    printf("Before the fork\n");
    if ((cpid = fork()) == 0) {
        printf("I’m the child (sleeping for 3 seconds)\n");
        sleep(3);
        printf("child done\n");
        exit(100);
    }
    else if (cpid == -1) {
        printf("Fork failed\n");
    } else {
        printf("I’m the parent (I’ve got a child with id %d)\n", cpid);
        tpid = wait(&status);
        printf("parent: wait returned pid %d\n", tpid);
        if (WIFEXITED(status) == 1) {
            int s = WEXITSTATUS(status);
            printf("parent: child returned with status %d\n", s);
        }
    }
    exit(0);
}
