/*
techshell.c

Author: Edward Ware

This supports:
 - Command execution via fork + execvp
 - Built-in cd command
 - Input/output redirection (<, >)
 - Pipes (|)
    - Arbitrary number of commands in a pipeline
 - Background execution (&)
 - SIGINT handling
 - Command history

 The code is written to compile cleanly with:
  gcc -std=c11 -Wall -Wextra -Werror -pedantic
*/
#define _POSIX_C_SOURCE 200809L // POSIX compliance for getline, etc.

#include <stdio.h>      // printf, fgets, perror
#include <unistd.h>     // fork, execvp, chdir, dup2, getcwd
#include <string.h>     // strtok, strcmp, strerror
#include <fcntl.h>      // open, O_* flags
#include <sys/wait.h>   // wait, waitpid
#include <errno.h>      // errno
#include <signal.h>     // signal, SIGINT


// Configuration constants

#define MAX_LINE     1024
#define MAX_ARGS     128
#define MAX_HISTORY  100

/*
 Debug macro
 Compile with -DDEBUG to enable
 */
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) ((void)0)
#endif


// History storage

static char history[MAX_HISTORY][MAX_LINE];
static int history_count = 0;


// Function prototypes

static void print_prompt(void);
static int parse_line(char *line, char **args, int *background);
static void execute_pipeline(char **args, int background);
static void handle_redirection(char **args);
static int count_pipes(char **args);
static void sigint_handler(int signo);
static void add_history(const char *line);
static void print_history(void);


// Global variables
static volatile sig_atomic_t sigint_received = 0;

// Main shell loop

int main(void) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    int background = 0;

    // Ignore SIGINT in the shell itself
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);


    while (1) {
        print_prompt();

        sigint_received = 0;

        if (fgets(line, sizeof(line), stdin) == NULL) {

        if (sigint_received) {
            write(STDOUT_FILENO, "\n", 1);
            continue;   // back to prompt
        }

        // EOF (Ctrl+D)
        printf("\n");
        break;
        }


        // Strip trailing newline
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        add_history(line);

        background = 0;
        int argc = parse_line(line, args, &background);
        if (argc == 0) {
            continue;
        }

        // Built-in exit
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // Built-in cd
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL || chdir(args[1]) != 0) {
                fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));
            }
            continue;
        }

        // Built-in history
        if (strcmp(args[0], "history") == 0) {
            print_history();
            continue;
        }

        execute_pipeline(args, background);
    }

    return 0;
}


// Prompt display

static void print_prompt(void) {
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s$ ", cwd);
    } else {
        perror("getcwd");
    }

    fflush(stdout);
}


// Input parsing

static int parse_line(char *line, char **args, int *background) {
    int argc = 0;
    char *token = strtok(line, " ");

    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            *background = 1;
        } else {
            args[argc++] = token;
        }
        token = strtok(NULL, " ");
    }

    args[argc] = NULL;
    DEBUG_PRINT("Parsed %d arguments", argc);
    return argc;
}


// Pipeline execution

static void execute_pipeline(char **args, int background) {
    int pipe_count = count_pipes(args);
    int fd[2];
    int in_fd = STDIN_FILENO;
    pid_t pid;

    for (int i = 0; i <= pipe_count; i++) {

        if (i < pipe_count && pipe(fd) < 0) {
            perror("pipe");
            return;
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        }

        if (pid == 0) {
            // Child restores default SIGINT behavior
            signal(SIGINT, SIG_DFL);

            if (in_fd != STDIN_FILENO) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            if (i < pipe_count) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }

            for (int j = 0; args[j] != NULL; j++) {
                if (strcmp(args[j], "|") == 0) {
                    args[j] = NULL;
                    break;
                }
            }

            handle_redirection(args);
            execvp(args[0], args);

            perror("execvp");
            _exit(1);
        }


        if (in_fd != STDIN_FILENO) {
            close(in_fd);
        }

        if (i < pipe_count) {
            close(fd[1]);
            in_fd = fd[0];
        }

        // Advance argument pointer past current command
        while (*args != NULL && strcmp(*args, "|") != 0) {
            args++;
        }
        args++;
    }

    if (!background) {
        while (wait(NULL) > 0) {
            // Wait for all pipeline processes
        }
    }
}


// I/O redirection

static void handle_redirection(char **args) {
    for (int i = 0; args[i] != NULL; i++) {

        if (strcmp(args[i], "<") == 0) {
            int fd = open(args[i + 1], O_RDONLY);
            if (fd < 0) {
                perror("input");
                _exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
        }

        if (strcmp(args[i], ">") == 0) {
            int fd = open(args[i + 1],
                          O_WRONLY | O_CREAT | O_TRUNC,
                          0644);
            if (fd < 0) {
                perror("output");
                _exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        }
    }
}


// Pipe counting helper

static int count_pipes(char **args) {
    int count = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            count++;
        }
    }
    return count;
}


// SIGINT handler

static void sigint_handler(int signo) {
    (void)signo;
    sigint_received = 1;
}


// History management

static void add_history(const char *line) {
    if (history_count < MAX_HISTORY) {
        strncpy(history[history_count], line, MAX_LINE - 1);
        history[history_count][MAX_LINE - 1] = '\0';
        history_count++;
    }
}

static void print_history(void) {
    for (int i = 0; i < history_count; i++) {
        printf("%d  %s\n", i + 1, history[i]);
    }
}
