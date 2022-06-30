#include "jowessendorf_dll.h"
//#include "makeargv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include <errno.h>
int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
      return -1;
   }
   *argvp = NULL;                           
   snew = s + strspn(s, delimiters);         /* snew is real start of string */
   if ((t = malloc(strlen(snew) + 1)) == NULL) 
      return -1; 
   strcpy(t, snew);               
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)     /* count the number of tokens in s */
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ; 

                             /* create argument array for ptrs to the tokens */
   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1; 
   } 
                        /* insert pointers to tokens into the argument array */
   if (numtokens == 0) 
      free(t);
   else {
      strcpy(t, snew);
      **argvp = strtok(t, delimiters);
      for (i = 1; i < numtokens; i++)
          *((*argvp) + i) = strtok(NULL, delimiters);
    } 
    *((*argvp) + numtokens) = NULL;             /* put in final NULL pointer */
    return numtokens;
}     

void freemakeargv(char **argv) {
   if (argv == NULL)
      return;
   if (*argv != NULL)
      free(*argv);
   free(argv);
}

const char *PROMPT = "tsh> ";

const char *DELIMITERS = "\n\t ";


void run(char ***args, int tokens) {
    char *command = (*args)[0];

    if (strcmp(command, "cd") == 0) {
        if (tokens == 1) {
            int status = chdir(getenv("HOME"));
            printf("[status=%d]\n", status);
        } else if (tokens == 2) {
            int status = chdir((*args)[1]);
            printf("[status=%d]\n", status);
        } else {
            printf("[cd: too many arguments]\n");
        }
        return;
    } 

    pid_t pid = fork();
    if (pid == 0) {
        // this will be executed from the child process
        execvp((*args)[0], *args);

        // if the above command executed successfully the child process will not return
        printf("\n[invalid command]");
        printf("\n[status=1]\n");
        return;
    } else {
        // this will be executed from the parent process
        int status;
        pid_t ret_val = waitpid(pid, &status, 0);
        printf("[status=%d]\n", status);
    }
}

int main() {
    while (1) {
        //char *path = NULL;
        //getcwd(path, 0);
        //printf("%s:%s", path, PROMPT);
        printf("%s", PROMPT);

        char *input = NULL;
        size_t input_size = 0;

        getline(&input, &input_size, stdin);

        char **args = NULL;

        int tokens = makeargv(input, DELIMITERS, &args);

        if (tokens == 1 && strcmp(args[0], "quit") == 0) {
            exit(0);
        } else if (tokens == 1 && strcmp(args[0], "list") == 0) {
            printf("listing all processes\n");
        } else if (tokens == 2 && strcmp(args[0], "info") == 0) {
            printf("info about a process\n");
        } else if (tokens == 2 && strcmp(args[0], "kill") == 0) {
            printf("killing a process\n");
        } else if (tokens == 2 && strcmp(args[0], "wait") == 0) {
            printf("waitings for process to end\n");
        } else if (tokens == 2 && strcmp(args[0], "job") == 0) {
            printf("adding a process to the bg task list\n");
        } else {
            run(&args, tokens);
        }

        freemakeargv(args);

    };

    printf("\n");
    return 0;
}
