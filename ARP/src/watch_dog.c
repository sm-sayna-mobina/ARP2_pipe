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

#define NUMBER_OF_INACTIVE_PROCESS  4
#define INACTIVE_TIME_LIMIT   60
#define CHECK_TIME   30

void sig_killhandler(int signo);
bool checkTime(int hStart, int mStart, int sStart, int durationSeconds);

/*
 Declaring the Signals Handler Function
*/
void sig_killhandler(int signo);

int main(int argc, char const *argv[])
{
    int pipe_fd[2];
    char buffer[100];
    int processPID;
    int lastActivityHour;
    int lastActivityMin;
    int lastActivitySec;
    bool processActivityStatus;
    int inactiveProcessCounter = 0;
    char processLogAddress[6][20] = {"./drone.txt", "./server.txt","./obstacles.txt", "./targets.txt","./window.txt", "./master.txt"};
    char processPIDList[6][20] = {"", "", "","", "", ""};
    
    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    while(1){
        sleep(CHECK_TIME); // Wait for 30 seconds then check the activity status of the process
        while (inactiveProcessCounter < NUMBER_OF_INACTIVE_PROCESS){
            /*Opening the logfile of the target process to read its PID */
            FILE *fp = fopen(processLogAddress[inactiveProcessCounter], "r");
            /*Error Checking*/ 
            if (fp == NULL){
                printf("Could not open the log file '%s'; errno=%d\n", processLogAddress[inactiveProcessCounter], errno);
                exit(EXIT_FAILURE); 
            } 

            /* Reading the first 50 bytes of the file */
            fread(buffer, 50, 1, fp);
            fclose(fp);

            /* Extracting PID of the target process */
            char *token = strtok(buffer, ",");
            strcpy(processPIDList[inactiveProcessCounter], token);
            processPID = atoi(token);

            /* Extracting the last active time of the target process */
            token = strtok(NULL, ",");
            lastActivityHour = atoi(token);

            token = strtok(NULL, ",");
            lastActivityMin = atoi(token);

            token = strtok(NULL, ",");
            lastActivitySec = atoi(token);
            
            /* Print the last active time of the target process */
            printf("The process %d last activity time is(H,M,S):%d:%d:%d \n", processPID, lastActivityHour, lastActivityMin, lastActivitySec);

            /* Check the activity status of the target process in the last 60 seconds */
            printf("Check Activity Status in last 60 seconds...\n");
            processActivityStatus = checkTime(lastActivityHour, lastActivityMin, lastActivitySec, INACTIVE_TIME_LIMIT);

            if(processActivityStatus){ /* If the process was inactive in the last 60 seconds */
                /* Print information of the inactive process */
                printf("The process %d was inactive for more than 60 seconds... :(\n\n", processPID);
                /* Add 1 to the inactive process counter */
                inactiveProcessCounter = inactiveProcessCounter + 1;
                if(inactiveProcessCounter == NUMBER_OF_INACTIVE_PROCESS){  /* If all the processes were inactive in the last 60 seconds */
                    printf("Killing all the processes :|\n");
                    for(int i = 0; i < NUMBER_OF_INACTIVE_PROCESS; i++){
                        /* Send the 'kill signal' to all the target processes */
                        printf("Killing process %s\n", processPIDList[i]);
                        kill(atoi(processPIDList[i]), SIGINT);
                    }
                }
            } else {
                /* If at least one process was active in the last 60 seconds, ignore the checking process */
                printf("Good! The process %d is active :)\n\n", processPID);
                inactiveProcessCounter = 0;
                break;
            }
        }
    }
    return 0; 
}

/*
 Defining the signal handlers function
*/
void sig_killhandler(int signo){
    if (signo == SIGINT){
        printf("I Received SIGINT Signal!\n");
        kill(getpid(), SIGKILL);
    }
}

/* A function to check the activity status of a target process based on the last active time of the process and the given time limitation. */
bool checkTime(int hStart, int mStart, int sStart, int durationSeconds){
    time_t t = time(NULL);  
    struct tm tm = *localtime(&t);
    /* Comparing the last activity time of the process with a given time limitation */   
    bool status = (tm.tm_hour - hStart) * 3600 + (tm.tm_min - mStart) * 60 + (tm.tm_sec - sStart) > durationSeconds;
    if(status){
        printf("Time Over\n");
        return true;
    }
    return false;      
}
