#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_INPUT_LENGTH 1024

int main() {
    char cmd[MAX_INPUT_LENGTH];
    char *args[MAX_INPUT_LENGTH / 2];
    char *token;
    char cwd[PATH_MAX];

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s: ", cwd);
        } else {
            perror("getcwd() error");
            return 1;
        }

        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            printf("EOF\n");
            break;
        }

        // Remove newline character from fgets input
        cmd[strcspn(cmd, "\n")] = 0;

        // Parse the command and arguments
        int i = 0;
        token = strtok(cmd, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the arguments array

        // Execute the command
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execvp(args[0], args);
            // If execvp returns, it must have failed
            perror("execvp");
            return 1;
        } else if (pid > 0) {
            // Parent process
            waitpid(pid, NULL, 0);
        } else {
            perror("fork");
            return 1;
        }
    }

    return 0;
}
