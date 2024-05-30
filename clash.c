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
        // Display the current directory, to be followed by a prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s: ", cwd);
        } else {
            perror("getcwd() error");
            return 1;
        }

        // Read line of input
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            if (feof(stdin)) {
                printf("EOF\n");  // End program on EOF
                break;
            } else if (ferror(stdin)) {
                // Handle read errors
                perror("Error reading input");
                clearerr(stdin);
                break;
            }
        }

        // Remove newline character from input
        cmd[strcspn(cmd, "\n")] = 0;

        // Parse input into command & arguments
        int i = 0;
        token = strtok(cmd, " ");
        if (!token) {
            // Handle missing input
            fprintf(stderr, "No input command.\n");
            continue;
        }

        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        args[i] = NULL;  // Null-terminate the arguments array for execvp

        // Fork new process to execute the command
        pid_t pid = fork();
        if (pid == 0) {
            // Child process: attempt to execute the command
            execvp(args[0], args);
            // Only runs if execvp fails
            perror("execvp");
            return 1;
        } else if (pid > 0) {
            // Parent process: wait for the child to finish
            waitpid(pid, NULL, 0);
        } else {
            // Error handling if fork fails
            perror("fork");
            return 1;
        }
    }

    return 0;
}
