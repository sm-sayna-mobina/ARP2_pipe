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
#include <termios.h>
#include "blackboard.h"
#include <math.h>

// Drone working area
#define X_LIMIT_MIN    0
#define X_LIMIT_MAX    32
#define Y_LIMIT_MIN    0
#define Y_LIMIT_MAX    140

// Given parameters
double M = 1.0;           // Mass of the drone
double K = 0.1;            // Air friction coefficient
double deltaTime = 0.1;    // Time step 
double Fx = 0;           // Force in x direction
double Fy = 0;           // Force in y direction

// Initial conditions
double x = 1.0;   // Initial position in x direction
double y = 1.0;   // Initial position in y direction
double vx = 0.0;  // Initial velocity in x direction
double vy = 0.001;  // Initial velocity in y direction

void sig_killhandler(int signo);
void logData(char * fileName, char * mode);
void createLogFile(char * fileName, char * mode);
void calculateDroneMotion();
void moveDrone(char direction);

int main(){
  /*Creating a 'Log File' to record the process information like 'PID' and 'Last Activity Time'*/
  createLogFile("./drone.txt","w+");
  /*Logging the data*/
  logData("drone.txt","w+");
  //a 'SIGINT' signal from the 'watchdog process' to the 'motorx process'.
  signal(SIGINT, sig_killhandler);
  
  int pipe_fd[2]; // File descriptors for the pipe
  char buffer[1]; // Buffer for reading one character command

  // Create pipe
  if (pipe(pipe_fd) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
  }

  while (1) {
      // Read from the pipe for movement command
      read(pipe_fd[0], buffer, sizeof(buffer));
      moveDrone(buffer[0]);
      usleep(100000);  // Adjust usleep duration for desired update rate (e.g., 10 milliseconds)
  }

  close(pipe_fd[0]);
  close(pipe_fd[1]);

  return 0;
}

/*
Defining the signal handlers function
*/
void sig_killhandler(int signo){
    if (signo == SIGINT){
        printf("I Received SIGINT Signal!\n");
        exit(EXIT_SUCCESS);
    }
}

void calculateDroneMotion() {
    // Calculate velocities
    vx += (Fx - K * (vx)) / M * deltaTime;
    vy += (Fy - K * (vy)) / M * deltaTime;

    // Update positions
    x += vx * deltaTime + 0.5 * ((vx - K * vx) / M) * deltaTime * deltaTime;
    y += vy * deltaTime + 0.5 * ((vy - K * vy) / M) * deltaTime * deltaTime;

    // Check boundary conditions and adjust velocities
    if (x < X_LIMIT_MIN || x > X_LIMIT_MAX) {
        vx = -(vx); // Reverse the velocity upon hitting X boundary
    }
    if (y < Y_LIMIT_MIN || y > Y_LIMIT_MAX) {
        vy = -(vy); // Reverse the velocity upon hitting Y boundary
    }
}

void moveDrone(char direction) {
    // Compute the Force in x and y directions
    switch (direction) {
        case 'e':
           Fx += -1;
           logData("drone.txt","w+");
            break;
        case 'c':
           Fx += 1;
           logData("drone.txt","w+");
            break;
        case 's':
            Fy += -1;
            logData("drone.txt","w+");
            break;
        case 'f':
            Fy += 1;
            logData("drone.txt","w+");
            break;
        case 'r':
            Fx += -sqrt(2)/2;
            Fy += sqrt(2)/2;
            logData("drone.txt","w+");
            break;
        case 'x':
            Fx += sqrt(2)/2;
            Fy += -sqrt(2)/2;
            logData("drone.txt","w+");
            break;
        case 'v':
            Fx += sqrt(2)/2;
            Fy += sqrt(2)/2;
            logData("drone.txt","w+");
            break;
        case 'w':
            Fx += -sqrt(2)/2;
            Fy += -sqrt(2)/2;
            logData("drone.txt","w+");
            break;
        case 'd':
            Fx=0;
            Fy=0;
            vx=0;
            vy=0;
            logData("drone.txt","w+");
            break;
    }

    // Solve the dynamic equation by the Taylor series expansion
    calculateDroneMotion();
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
