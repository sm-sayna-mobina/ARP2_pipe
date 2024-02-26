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
#include <sys/wait.h>
#include <stdbool.h>
#include <errno.h>
#include "blackboard.h"

#define NUM_TARGETS 5

// Pipes
int pipes[NUM_TARGETS][2][2]; // 2 pipes for each target for bidirectional communication

//Initialize the target's location
void initializeTargets(Point *targets_location){
    for (int i = 0; i < NUM_TARGETS; ++i) {
        targets_location[i].x = rand() % 30;
        targets_location[i].y = rand() % 150;
    }
}

//Update the target's location
void updateTargets(Point *targets_location) {
    // Update the status of targets based on the drone's movement
    for (int i = 0; i < NUM_TARGETS; ++i) {
        // Read from the read end of the pipe
        char buffer[256];
        ssize_t bytes_read = read(pipes[i][0][0], buffer, sizeof(buffer));
        if (bytes_read > 0) {
            // Assuming the message received is of the form "x,y"
            buffer[bytes_read] = '\0';
            char *token = strtok(buffer, ",");
            int new_x = atoi(token);
            token = strtok(NULL, ",");
            int new_y = atoi(token);

            // Update the target's location
            targets_location[i].x = new_x;
            targets_location[i].y = new_y;

            printf("Target %d updated to (%d, %d)\n", i+1, new_x, new_y);
        }
    }
}

void sig_killhandler(int signo);
void logData(char * fileName, char * mode);
void createLogFile(char * fileName, char * mode);

int main(){
    /*Creating a 'Log File' to record the process information like 'PID' and 'Last Activity Time'*/
    createLogFile("./targets.txt","w+");
    /*Logging the data*/
    logData("targets.txt","w+");
    //a 'SIGINT' signal from the 'watchdog process' to the 'motorx process'.
    signal(SIGINT, sig_killhandler);

    // Initialize pipes
    for (int i = 0; i < NUM_TARGETS; ++i) {
        if (pipe(pipes[i][0]) == -1 || pipe(pipes[i][1]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Random seed
    srand(time(NULL));

    // Initialize shared memory
    Blackboard *shared_memory = initialize_shared_memory();

    initializeTargets(shared_memory->targets);

    while (1) {
        updateTargets(shared_memory->targets);
        sleep(1); // Adjust this based on how frequently you want to update targets
    }

    return 0;
}

/*
Defining the signal handlers function
*/
void sig_killhandler(int signo){
    if (signo == SIGINT){
        printf("I Received SIGINT Signal!\n");
        /*Close the resource*/
        for (int i = 0; i < NUM_TARGETS; ++i) {
            close(pipes[i][0][0]); // close read end of the first pipe
            close(pipes[i][0][1]); // close write end of the first pipe
            close(pipes[i][1][0]); // close read end of the second pipe
            close(pipes[i][1][1]); // close write end of the second pipe
        }

        /*Kill yourself!*/
        exit(EXIT_SUCCESS);
    }
}

/*a function to create a log file for the target process*/
void createLogFile(char * fileName, char * mode){
    /*In a new run of the program, if the log file exists, remove the old log file and create a new one*/
    FILE *fp;
    if(!remove(fileName)){
        printf("The old log file is deleted successfully!\n");
    }
    if(fp = fopen(fileName, mode)){
        printf("The new log file is created successfully!\n");
        logData(fileName, mode);
    }else{
        printf("Could not create a log file!\n");
    }
}

/*a function to record the essential information of a process into a logfile.*/
void logData(char * fileName, char * mode){
   FILE *fp;
   int pid = getpid();

   time_t t = time(NULL);
   struct tm tm = *localtime(&t);
   
   /*Opening the logfile and record the current activity time of the process*/
   fp = fopen(fileName, mode);
   /*Error Checking*/ 
   if (fp == NULL){
      printf("Could not open the %s file; errno=%d\n", fileName, errno);
      exit(1); 
    }
    /*Writing into the file*/ 
    fprintf(fp, "%d,%d,%d,%d\n", pid, tm.tm_hour, tm.tm_min, tm.tm_sec);
    /*Closing the file*/
    fclose(fp);
}
