#include "jowessendorf_dll.h"
#include "makeargv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <errno.h>

typedef struct _proc_node {
    struct list_head head;
    int id;
    int pid;
    int finished;
    int status;
    char *cmd;
} proc_node;

const char *PROMPT = "tsh> ";

const char *DELIMITERS = "\n\t ";

void run(char **args, int tokens) {
    char *command = args[0];

    if (strcmp(command, "cd") == 0) {
        if (tokens == 1) {
            int status = chdir(getenv("HOME"));
            printf("[status=%d]\n", status);
        } else if (tokens == 2) {
            int status = chdir(args[1]);
            printf("[status=%d]\n", status);
        } else {
            printf("[cd: too many arguments]\n");
        }
        return;
    } 

    pid_t pid = fork();
    if (pid == 0) {
        // this will be executed from the child process
        execvp(args[0], args);

        // if the above command executed successfully the child process will not return
        printf("[invalid command]\n");
        exit(0);
    } else if (pid == -1) {
        // this will be executed from the parent process if the fork failed
        printf("[fork failed - could not run command]");
    } else {
        // this will be executed from the parent process
        int status;
        pid_t ret_val = waitpid(pid, &status, 0);
        if (ret_val != -1) {
            printf("[status=%d]\n", status);
        } else if (errno == ECHILD) {
            // process with pid not found
            // this error can be ignored
        }
    }
}

void start_job(char **args, proc_node *head, int tokens) {
    // ignore the job which is the first argument
    args++;
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        printf("\n[could not start job]\n");
        exit(-1);
    } else if (pid == -1) {
        printf("[fork failed - could not start job]");
    } else {
        proc_node *new_elem = malloc(sizeof(proc_node));
        list_add_tail(&new_elem->head, &head->head);
        new_elem->id = ((proc_node*) new_elem->head.prev)->id + 1;
        new_elem->pid = pid;
        new_elem->finished = 0;
        new_elem->status = 0;

        // concatenate the command with the arguments to a single string
        char *cmd = malloc(sizeof(char) * strlen(args[0]));
        strcpy(cmd, args[0]);
        for (int i = 1; i < tokens; i++) {
            cmd = realloc(cmd, sizeof(char) * (strlen(cmd) + strlen(args[i])));
            strcat(cmd, " ");
            strcat(cmd, args[i]);
        }
        new_elem->cmd = cmd;
    }
}

proc_node* get_job_by_id(int id, proc_node *head) {
    proc_node *tmp = (proc_node*) head->head.next;
    do {
        if (tmp->id != id || tmp->id == -1) {
            tmp = (proc_node*) tmp->head.next;
            continue;
        }
        return tmp;
    } while (tmp != head);

    return NULL;
}

void update_proc_list(proc_node *head) {
    // ignore the anker node
    proc_node *tmp = (proc_node*) head->head.next;

    do {
        if (tmp->id == -1) continue;
        if (tmp->finished == 1 || tmp->id == -1) {
            tmp = (proc_node*) tmp->head.next;
            continue;
        };

        int status;
        pid_t ret_val = waitpid(tmp->pid, &status, WNOHANG);

        if (ret_val == -1 && errno == ECHILD) {
            // process does not exist or is not a child of the shell process therefore it is set to finished
            tmp->finished = 1;
        } else if (ret_val != 0) {
            tmp->finished = 1;
            tmp->status = status;
        }
        
        tmp = (proc_node*) tmp->head.next;
    } while (tmp != head);
}

void print_proc_list(proc_node *head) {
    update_proc_list(head);

    // ignore the anker node
    proc_node *tmp = (proc_node*) head->head.next;
    do {
        if (tmp->id == -1) continue;
        printf("  %d (pid\t%d %s\tstatus=%d): %s\n", tmp->id, tmp->pid, tmp->finished == 1 ? "finished" : "running", tmp->status, tmp->cmd);
        tmp = (proc_node*) tmp->head.next;
    } while (tmp != head);
}

void print_proc_info(proc_node *head, int id) {
    update_proc_list(head);

    proc_node *node = get_job_by_id(id, head);
    if (node != NULL) {
        printf("  %d (pid\t%d %s\tstatus=%d): %s\n", node->id, node->pid, node->finished == 1 ? "finished" : "running", node->status, node->cmd);
    } else {
        printf("There is no job with the id %d\n", id);
    }
}

void kill_job(int id, proc_node *head) {
    proc_node *node = get_job_by_id(id, head);
    if (node != NULL && node->finished != 1) {
        kill(node->pid, SIGKILL);
    } else {
        printf("There is no running job with the id %d\n", id);
    }
}


void wait_job(int id, proc_node *head) {
    proc_node *node = get_job_by_id(id, head);
    if (node != NULL && node->finished != 1) {
        int status;
        pid_t ret_val = waitpid(node->pid, &status, 0);
        node->status = status;
        node->finished = 1;
        printf("[status=%d]\n", status);
    } else {
        printf("There is no running job with the id %d\n", id);
    }
}


int main() {
    proc_node *anker = malloc(sizeof(proc_node));
    anker->id = -1;

    list_init(&anker->head);

    while (1) {
        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        printf("%s %s", cwd, PROMPT);

        char *input = NULL;
        size_t input_size = 0;

        getline(&input, &input_size, stdin);

        char **args = NULL;

        int tokens = makeargv(input, DELIMITERS, &args);

        if (tokens == 0) {
            continue;
        } else if (tokens == 1 && strcmp(args[0], "quit") == 0) {
            exit(0);
        } else if (tokens == 1 && strcmp(args[0], "list") == 0) {
            print_proc_list(anker);
        } else if (tokens == 2 && strcmp(args[0], "info") == 0) {
            print_proc_info(anker, atoi(args[1]));
        } else if (tokens == 2 && strcmp(args[0], "kill") == 0) {
            kill_job(atoi(args[1]), anker);
        } else if (tokens == 2 && strcmp(args[0], "wait") == 0) {
            wait_job(atoi(args[1]), anker);
        } else if (strcmp(args[0], "job") == 0) {
            start_job(args, anker, tokens - 1);
        } else {
            run(args, tokens);
        }

        freemakeargv(args);
    };

    printf("\n");
    return 0;
}
