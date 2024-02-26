#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <signal.h>
#include <time.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <errno.h>
#include "blackboard.h"

int pipe_fd[2]; // Pipe file descriptors

// Define the border dimensions
#define BORDER_WIDTH 140
#define BORDER_HEIGHT 32

void sig_killhandler(int signo);
void logData(char *fileName, char *mode);
void createLogFile(char *fileName, char *mode);

// Draw border to show the working area of the drone
void drawBorder() {
    // Draw horizontal borders
    attron(COLOR_PAIR(1)); // Use blue color for the border
    for (int i = 0; i < BORDER_WIDTH; ++i) {
        mvaddch(0, i, '-');                           // Top border
        mvaddch(BORDER_HEIGHT - 1, i, '-');           // Bottom border
    }
    attroff(COLOR_PAIR(1));

    // Draw vertical borders
    attron(COLOR_PAIR(1)); // Use blue color for the border
    for (int i = 0; i < BORDER_HEIGHT; ++i) {
        mvaddch(i, 0, '|');                          // Left border
        mvaddch(i, BORDER_WIDTH - 1, '|');          // Right border
    }
    attroff(COLOR_PAIR(1));
}

// Function to initialize colors in NCurses
void initialize_colors() {
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    }
}

// Function to draw a character on the screen based on the Character structure
void draw_character(Character *character) {
    wattron(stdscr, COLOR_PAIR(character->color_pair));
    mvaddch(character->row, character->col, character->symbol);
    wattroff(stdscr, COLOR_PAIR(character->color_pair));
}

// Function to update targets and obstacles based on input from the main program
void updateTargetsObstacles(Character *targets, Character *obstacles) {
    // Read data from the pipe
    read(pipe_fd[0], targets, sizeof(Character) * MAX_TARGETS);
    read(pipe_fd[0], obstacles, sizeof(Character) * MAX_OBSTACLES);
}

int main() {
    // Creating a 'Log File' to record the process information like 'PID' and 'Last Activity Time'
    createLogFile("./window.txt", "w+");
    // Logging the data
    logData("window.txt", "w+");
    // Handling SIGINT signal
    signal(SIGINT, sig_killhandler);

    // Initialize NCurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide cursor
    timeout(200); // Set a timeout for getch() to simulate real-time behavior

    initialize_colors();

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    while (1) {
        clear(); // Clear the screen

        // Draw border
        drawBorder();

        // Array to hold targets and obstacles
        Character targets[MAX_TARGETS];
        Character obstacles[MAX_OBSTACLES];

        // Update targets and obstacles based on input from the main program
        updateTargetsObstacles(targets, obstacles);

        // Draw targets
        for (int i = 0; i < MAX_TARGETS; ++i) {
            if (targets[i].row != -1 && targets[i].col != -1) {
                draw_character(&targets[i]);
            }
        }

        // Draw obstacles
        for (int i = 0; i < MAX_OBSTACLES; ++i) {
            if (obstacles[i].row != -1 && obstacles[i].col != -1) {
                draw_character(&obstacles[i]);
            }
        }

        // Refresh the screen
        refresh();
    }

    // Close the pipe
    close(pipe_fd[0]);

    // End NCurses
    endwin();
    return 0;
}

// Signal handler for SIGINT
void sig_killhandler(int signo) {
    if (signo == SIGINT) {
        printf("Received SIGINT Signal!\n");
        close(pipe_fd[0]); // Close the read end of the pipe
        exit(EXIT_SUCCESS);
    }
}

// Function to create a log file for the target process
void createLogFile(char *fileName, char *mode) {
    // Remove the old log file if it exists
    if (!remove(fileName)) {
        printf("The old log file is deleted successfully!\n");
    }
    FILE *fp;
    // Create a new log file
    if (fp = fopen(fileName, mode)) {
        printf("The new log file is created successfully!\n");
        logData(fileName, mode);
    } else {
        printf("Could not create a log file!\n");
    }
}

// Function to record the essential information of a process into a logfile
void logData(char *fileName, char *mode) {
    FILE *fp;
    int pid = getpid();
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
   
    // Opening the logfile and record the current activity time of the process
    fp = fopen(fileName, mode);
    if (fp < 0) {
        printf("Could not open the %s file; errno=%d\n", fileName, errno);
        exit(1); 
    }
    // Writing into the file
    fprintf(fp, "%d,%d,%d,%d\n", pid, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fclose(fp); // Closing the file
}
