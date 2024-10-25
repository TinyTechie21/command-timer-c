#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// Global variable to store the child's PID for termination if needed
pid_t child_pid = 0;

// Signal handler for the alarm (time limit exceeded)
void handle_alarm(int sig) {
    if (child_pid > 0) {
        kill(child_pid, SIGTERM);  // Kill the child process
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <time_limit> <command> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Get the time limit from the command line
    int time_limit = atoi(argv[1]);
    if (time_limit <= 0) {
        fprintf(stderr, "Error: Time limit must be a positive integer.\n");
        exit(EXIT_FAILURE);
    }

    // Fork a new process
    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child process: execute the command
        execvp(argv[2], &argv[2]);  // Execute command with arguments
        // If execvp returns, it means an error occurred
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: set up the timer and wait for the child

        // Set up the signal handler for the alarm
        signal(SIGALRM, handle_alarm);

        // Start the timer
        alarm(time_limit);

        // Wait for the child process to finish or be killed
        int status;
        waitpid(child_pid, &status, 0);

        // Check how the child process exited
        if (WIFEXITED(status)) {
            printf("Command exited with status %d.\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Command killed after %d seconds.\n", time_limit);
        }

        // Cancel the alarm if the child process finished before the time limit
        alarm(0);
    }

    return 0;
}