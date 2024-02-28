# ARP2_pipe

# Drone Simulation Program

## Overview

This C program simulates a drone environment using the NCurses library. It provides a real-time graphical representation of drone targets and obstacles within a bordered window. The simulation is dynamic, and the positions of targets and obstacles are updated based on input received through a pipe.

## Features

- **Real-time Simulation:** Utilizes the NCurses library for a dynamic and interactive drone simulation.
- **Dynamic Position Update:** Targets and obstacles are updated in real-time based on input from the main program through a pipe.
- **Signal Handling:** Gracefully terminates the simulation on receiving the SIGINT signal.

## Prerequisites

Ensure the following dependencies are installed:

- **NCurses Library:** Required for graphical interface. Install using your package manager.

    ```bash
    sudo apt-get install libncurses5-dev
    ```

- **C Compiler (e.g., GCC):**

    ```bash
    sudo apt-get install build-essential
    ```

## Project Structure

```
drone_simulation/
│
├── drone_simulation.c   # Main C program for drone simulation
├── blackboard.h         # Header file with data structures used in the simulation
├── window.txt           # Log file recording process information
```

## Compilation

Compile the program using GCC:

```bash
gcc -o drone_simulation drone_simulation.c -lncurses
```

## Execution

1. **Run the main program:**
   - Ensure the main program is running and providing input through the pipe.

2. **Execute the drone simulation:**

    ```bash
    ./drone_simulation
    ```

## Termination

To terminate the simulation, press `Ctrl + C`.

## Logging

The program logs essential process information, including PID and activity timestamp, in `window.txt`.

## File Descriptions

- **drone_simulation.c:** Main C program for drone simulation.
- **blackboard.h:** Header file with data structures used in the simulation.
- **window.txt:** Log file recording process information.


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
