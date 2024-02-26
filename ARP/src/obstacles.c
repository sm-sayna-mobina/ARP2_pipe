#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <errno.h>

#define NUM_OBSTACLES 5

// Pipes
int pipes[NUM_OBSTACLES][2]; // 2 pipes for each obstacle for bidirectional communication

typedef struct {
    int x;
    int y;
} Point;

//Initialize the obstacle's location
void initializeObstacles(Point *obstacles_location){
    for (int i = 0; i < NUM_OBSTACLES; ++i) {
        obstacles_location[i].x = rand() % 30;
        obstacles_location[i].y = rand() % 150;
    }
}

//Update the obstacle's location
void updateObstacles(Point *obstacles_location) {
    // Generate new obstacles
    for (int i = 0; i < NUM_OBSTACLES; ++i) {
        obstacles_location[i].x = rand() % 30;
        obstacles_location[i].y = rand() % 150;
    }
}

void sig_killhandler(int signo);
void logData(char * fileName, char * mode);
void createLogFile(char * fileName, char * mode);

int main(){
    /*Creating a 'Log File' to record the process information like 'PID' and 'Last Activity Time'*/
    createLogFile("./obstacles.txt","w+");
    /*Logging the data*/
    logData("obstacles.txt","w+");
    //a 'SIGINT' signal from the 'watchdog process' to the 'motorx process'.
    signal(SIGINT, sig_killhandler);

    // Initialize pipes
    for (int i = 0; i < NUM_OBSTACLES; ++i) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    srand(time(NULL));
    while (1) {
        Point obstacles[NUM_OBSTACLES];
        initializeObstacles(obstacles);
        for (int i = 0; i < NUM_OBSTACLES; ++i) {
            // Write obstacle information to pipe
            write(pipes[i][1], &obstacles[i], sizeof(Point));
        }
        sleep(1); // Adjust sleep duration for desired update rate (e.g., 1 second)
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
        for (int i = 0; i < NUM_OBSTACLES; ++i) {
            close(pipes[i][0]); // close read end
            close(pipes[i][1]); // close write end
        }

        /*Kill yourself!*/
        exit(EXIT_SUCCESS);
    }
}

/*a function to create a log file for the target process*/
void createLogFile(char * fileName, char * mode){
    /*In a new run of the programe, if the log file exist, remove the old log file and create a new one*/
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
   if (fp<0){
      printf("Could not open the %s file; errno=%d\n", fileName, errno);
      exit(1); 
    }
    /*Writing into the file*/ 
    fprintf(fp, "%d,%d,%d,%d\n", pid, tm.tm_hour, tm.tm_min, tm.tm_sec);
    /*Closing the file*/
    fclose(fp);
}
