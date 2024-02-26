#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int pipefd[2]; // File descriptors for the pipe
int sockfd, newsockfd, clilen;
struct sockaddr_in serv_addr, cli_addr;

void sig_killhandler(int signo);
void logData(char *fileName, char *mode);
void createLogFile(char *fileName, char *mode);

int main(int argc, char *argv[]) {
    // Creating a 'Log File' to record the process information like 'PID' and 'Last Activity Time'
    createLogFile("./server.txt", "w+");
    // Logging the data
    logData("server.txt", "w+");

    // Creating the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Creates a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    // Configuring the server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(3500);

    // Binding the socket to the IP address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(EXIT_FAILURE);
    }
        
    // Server is ready! Waits for new client to request
    printf("Server is ready! Waits for new client to request...\n");
    listen(sockfd, 5);

    // Accepts the request
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    printf("Accepts the request\n");

    // Error Handling
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(EXIT_FAILURE);
    }
}

/*
Defining the signal handlers function
*/
void sig_killhandler(int signo) {
    if (signo == SIGINT) {
        printf("I Received SIGINT Signal!\n");

        // Close the pipe
        close(pipefd[0]);
        close(pipefd[1]);

        // Close the socket
        if (close(sockfd) == -1) {
            perror("Error closing socket");
            exit(EXIT_FAILURE);
        }

        // Kill yourself!
        kill(getpid(), SIGKILL);
    }
}

/* a function to create a log file for the target process */
void createLogFile(char *fileName, char *mode) {
    // In a new run of the program, if the log file exists, remove the old log file and create a new one
    FILE *fp;
    if (!remove(fileName)) {
        printf("The old log file is deleted successfully!\n");
    }
    if ((fp = fopen(fileName, mode)) != NULL) {
        printf("The new log file is created successfully!\n");
        logData(fileName, mode);
    } else {
        printf("Could not create a log file!\n");
    }
}

/* a function to record the essential information of a process into a logfile. */
void logData(char *fileName, char *mode) {
    FILE *fp;
    int pid = getpid();

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
   
    // Opening the logfile and record the current activity time of the process
    fp = fopen(fileName, mode);
    // Error Checking
    if (fp == NULL) {
        printf("Could not open the %s file; errno=%d\n", fileName, errno);
        exit(EXIT_FAILURE); 
    }
    // Writing into the file
    fprintf(fp, "%d,%d,%d,%d\n", pid, tm.tm_hour, tm.tm_min, tm.tm_sec);
    // Closing the file
    fclose(fp);
}
