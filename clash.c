#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_INPUT_LENGTH 1337

// Function prototypes
void display_prompt();
int read_command(char *cmd);
void parse_command(char *cmd, char **args, int *background);
int execute_command(char **args, int background);

int main() {
    char cmd[MAX_INPUT_LENGTH];
    char *args[MAX_INPUT_LENGTH / 2];
    int background;

    while (1) {
        display_prompt();
        if (read_command(cmd) == -1) break;
        parse_command(cmd, args, &background);
        if (execute_command(args, background) == -1) break;
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

    // Check if the input was too long & newline character is not the last read character
    if (strchr(cmd, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);

        fprintf(stderr, "Warning: Input exceeds the maximum length of %d bytes and has been discarded.\n", MAX_INPUT_LENGTH - 1);
        return -1;
    }

    // Properly terminate the string by removing the newline character
    cmd[strcspn(cmd, "\n")] = 0;
    return 0;
}


/**
 * Parses the command string into an array of arguments suitable for execvp.
 * Also determines if the command should be run in the background.
 *
 * @param cmd The command string to parse.
 * @param args Array to store the arguments.
 * @param background Pointer to store background execution flag.
 */
void parse_command(char *cmd, char **args, int *background) {
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;  // Null-terminate the arguments array

    // Check if the last argument is an "&", indicating background execution
    *background = (i > 0 && strcmp(args[i - 1], "&") == 0) ? 1 : 0;
    if (*background) {
        args[i - 1] = NULL; // Remove "&" from arguments
    }
}

/**
 * Executes the command using fork and execvp, then prints the exit status.
 * If the command is to be run in the background, it does not wait for the process to finish.
 *
 * @param args Array of arguments for the command to execute.
 * @param background Indicates whether the command should run in the background.
 * @return -1 on fork failure, otherwise 0.
 */
int execute_command(char **args, int background) {
    if (args[0] == NULL) {  // Early exit if no command is given
        fprintf(stderr, "No command entered.\n");
        return 0;
    }

    pid_t pid = fork();
    int status;

    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        if (background) {
            printf("Process [%d] running in background.\n", pid);
        } else {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                char full_command[MAX_INPUT_LENGTH] = "";
                int len = 0;
                for (int i = 0; args[i] != NULL; i++) {
                    len += snprintf(full_command + len, MAX_INPUT_LENGTH - len, "%s%s", (i > 0 ? " " : ""), args[i]);
                    if (len > MAX_INPUT_LENGTH) break; // Prevent buffer overflow. Not expected under current logic, but ensures safety against future modifications.
                }
                printf("Exitstatus [%s] = %d\n", full_command, exit_status);
            }
        }
    } else {
        perror("fork");
        return -1;
    }
    return 0;
}
