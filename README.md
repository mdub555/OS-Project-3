# Project 3 - CPU Scheduling Simulator

## Name
Mitchell Waibel

## Files

* `README.md`
  This file.
* `example_simulation`
  The example input file.
* `makefile`
  Makefile to build the program
* `simulator`
* `src/`
  * `main.cpp`
    The file that starts off the program and calls the simulation.
  * `simulation.cpp`
    The file that runs the simulation and handles all the events in the event queue.
  * `simulation.h`
    The header file which contains all the function definitions for simulation.cpp.
  * `algorithms/`
    * `fcfs_scheduler.*`
      Implementation for the first-come first-serve algorithm.
    * `multilevel_feedback_scheduler.*`
      Implementation for the multi-level feedback queue algorithm.
    * `priority_scheduler.*`
      Implementation for the priority algorithm.
    * `round_robin_scheduler.*`
      Implementation for the round robin algorithm.
    * `scheduler.h`
      Parent class for all the simulation algorithms.
  * `types/`
    * `burst.h`
      Holds information for a CPU or IO burst.
    * `event.h`
      Holds information needed for a scheduler event.
    * `process.h`
      Holds information relating to a process.
    * `scheduling_decision.h`
      Holds information needed for a scheduling decision.
    * `system_stats.h`
      Holds all the statistics for the scheduler and how well it ran.
    * `thread.*`
      Holds information and functions for a thread.
  * `util/`
    * `flags.*`
      Class to parse the command line flags.
    * `logger.*`
      Class to format simulator output.

## Features
This program does not include any additional features.

## Time Spent
| Deliverable      | Time     |
| ---------------- | --------:|
| FCFS Scheduler   |  2 hours |
| Other Schedulers | 12 hours |
| _Total_          | 14 hours |

## Multi-Level Feedback Queue
For my multi-level feedback algorithm, I implemented 8 levels of round robin queues. The round robin
queue implements preemption by removing the currently running thread after a set amount of time.
This made preemption easy to implement since all the code already existed in the round robin
algorithm class. As each process is preempted, they are moved to a lower level queue when that are
enqueued. The MLFQ algorithm prioritizes higher priority processes similar to a priority
queue, in that lower priority processes start out in a lower queue, allowing high priority processes
to run for a longer time initially. Since this algorithm can get a constant stream of high priority
processes, thus never running any of the currently ready low priority processes, the lower priority
processes can be starved. This makes the algorithm unfair, since fairness intails that a priority
that is enqueued will eventually be ran, which might not be the case in any system that allows
starvation.
