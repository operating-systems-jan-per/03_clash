#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_INPUT_LENGTH 1024

// Function prototypes
void display_prompt();
int read_command(char *cmd);
void parse_command(char *cmd, char **args);
int execute_command(char **args);

int main() {
    char cmd[MAX_INPUT_LENGTH];
    char *args[MAX_INPUT_LENGTH / 2];

    while (1) {
        display_prompt();
        if (read_command(cmd) == -1) break;
        parse_command(cmd, args);
        if (execute_command(args) == -1) break;
    }

    return 0;
}

/**
 * Displays the current working directory, to be followed by a prompt.
 */
void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s: ", cwd);
    } else {
        perror("getcwd() error");
        exit(1);
    }
}

/**
 * Reads a command from standard input.
 *
 * @param cmd Buffer to store the read command.
 * @return -1 on failure or EOF, otherwise 0.
 */
int read_command(char *cmd) {
    if (fgets(cmd, MAX_INPUT_LENGTH, stdin) == NULL) {
        if (feof(stdin)) {
            printf("EOF\n");
            return -1;
        } else {
            perror("Error reading input");
            clearerr(stdin);
            return -1;
        }
    }
    cmd[strcspn(cmd, "\n")] = 0;  // Remove newline character
    return 0;
}

/**
 * Parses the command string into an array of arguments suitable for execvp.
 *
 * @param cmd The command string to parse.
 * @param args Array to store the arguments.
 */
void parse_command(char *cmd, char **args) {
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;  // Null-terminate the arguments array
}

/**
 * Executes the command using fork and execvp.
 *
 * @param args Array of arguments for the command to execute.
 * @return -1 on fork failure, otherwise 0.
 */
int execute_command(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        waitpid(pid, NULL, 0);
    } else {
        perror("fork");
        return -1;
    }
    return 0;
}
