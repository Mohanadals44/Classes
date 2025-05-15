# README
Worked alone one this project
## Overview
This project implements solutions for the Sleeping Stylist problem using Monitor-based (`sleepingStylistMon`). The solutions demonstrate the signal-and-continue discipline in synchronization.

## Instructions to Run
1. Compile the programs using the provided `Makefile`:
    ```bash
    make
    ```
2. Run the Monitor-based solution:
    ```bash
    ./sleepingStylistMon
    ```

## Observations
To ensure the solutions follow the signal-and-continue discipline:
- Observe that when a stylist signals a customer, the stylist continues their work without waiting for the customer to leave.
- Similarly, when a customer signals the stylist, the customer continues without waiting for the stylist to start.
- the code uses signal and wait within an if statement which shows that it uses the signal-and-continue discipline.
- also it shows the same exact prints debugs as the Professor made it 


## Time Spent and Difficulty


| Semaphore-based solution | time spent: 6 hours rating: 5

## Cleanup
To remove compiled binaries, run:
```bash
make clean
```
