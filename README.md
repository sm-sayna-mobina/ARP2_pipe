# ARP2_pipe

# Drone Simulation Main Program

## Overview

This C program simulates a drone environment using the NCurses library. It provides a real-time graphical representation of drone targets and obstacles within a bordered window. The simulation is dynamic, and the positions of targets and obstacles are updated based on input received through a pipe.

## Features

- **Real-time Simulation:** Utilizes the NCurses library for a dynamic and interactive drone simulation.
- **Dynamic Position Update:** Targets and obstacles are updated in real-time based on input from the main program through a pipe.
- **Signal Handling:** Gracefully terminates the simulation on receiving the SIGINT signal.

## Compilation and Execution

1. **Prerequisites:**
    - Ensure NCurses library and a C compiler (e.g., GCC) are installed.

2. **Compilation:**
    ```bash
    gcc -o drone_simulation drone_simulation.c -lncurses
    ```

3. **Execution:**
    ```bash
    ./drone_simulation
    ```

4. **Termination:**
    - Press `Ctrl + C` to terminate the simulation.

## Logging

The program logs essential process information, including PID and activity timestamp, in `window.txt`.

# Watchdog Process

## Overview

This C program serves as a watchdog process for monitoring the activity of other processes in a drone simulation. It checks the activity status of specified target processes and takes appropriate actions if any process has been inactive for a defined time.

## Features

- **Activity Monitoring:** Monitors the last activity time of specified target processes.
- **Automatic Termination:** Kills all specified target processes if they have been inactive for a defined time.

## Prerequisites

- C compiler (e.g., GCC)

## Compilation and Execution

1. **Compilation:**
    ```bash
    gcc -o watchdog_process watchdog_process.c
    ```

2. **Execution:**
    ```bash
    ./watchdog_process
    ```

  
# Log File Creation and Logging

## Overview

This C program demonstrates log file creation and logging for a target process in a drone simulation. It records the PID and activity timestamp in a log file.

## Features

- **Log File Creation:** Creates a new log file or replaces an existing one.
- **Logging:** Records essential process information, including PID and activity timestamp.

## Compilation and Execution

1. **Compilation:**
    ```bash
    gcc -o log_file_creator log_file_creator.c
    ```

2. **Execution:**
    ```bash
    ./log_file_creator
    ```

## Logging

The program records PID and activity timestamp in the specified log file.


### How to install and run ###
- Install the ncurses library:
```console
sudo apt-get install libncurses-dev
```
- Open the terminal
- clone this repository to your desired location:
<pre><code>git clone https://github.com/sm-sayna-mobina/ARP2_pipe.git</code></pre>
- Go to the folder:
<pre><code>cd ARP2_pipe/ARP</code></pre>
- Run the project:
 <pre><code> ./run.sh </code></pre>
 If the code does not run, check the file's permissions in the 'Properties' section.
