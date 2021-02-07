# Project Background
### Goal
1. Implement different process scheduling algorithms
2. Give a list of tasks with its priority and burst (duration)

### Algorithms
- FCFS: based on order
- SJF: arranges tasks based on burst
- Priority: based on assigned priority
- RR: assigned fixed time quantum
- PRR: RR within tasks of equal priorityImplement different process scheduling algorithms

### Example Task Input
|Task Name | Priority | Duration |
|----------|----------|----------|
|T1 | 4 | 2 |
```
// example input file format
T2, 2, 25
T3, 3, 25
T4, 3, 15
T5, 10, 10
```

# Implementation
### Storing tasks
Assume all tasks arrive at the same time  
Need a data structure to store tasks. What are your options
- Unordered list: linear search usually required to find relevant task
- Ordered list: place tasks in the list sorted by priority/duration
- **Multiple ordered lists (winner)**:
  - ex. one list for each of the 11 priorities. Would then sort within list by duration
  - Downside - additional space needs to be allocated
  
### C Implementation
#### driver.c
1. Reads in the list of tasks (*see above*)
2. Stores tasks in selected data structure
3. Invoke process scheduler by calling `schedule(tasks, schedulingAlgo)`
4. Each scheduling algo has their own implementation of `pickNextTask(tasks)`
5. Tasks are run using `run()`

#### cmake
1. Select the scheduling algorithm invoked by the driver
- ex. `make fcfs` would run the driver with "first come first serve"
2. Run the compiled executable with the task file
- ex. `./fcfs schedule.txt`

#### Desired Output
```
Running task = [T1] [4] [20] for 20 units.
Time is now: 20
Running task = [T2] [3] [25] for 25 units.
Time is now: 45
Running task = [T3] [3] [25] for 25 units.
Time is now: 70
Running task = [T4] [5] [15] for 15 units.
Time is now: 85
Running task = [T5] [5] [20] for 20 units.
Time is now: 105
Running task = [T6] [1] [10] for 10 units.
Time is now: 115
Running task = [T7] [3] [30] for 30 units.
Time is now: 145
Running task = [T8] [10] [25] for 25 units.
Time is now: 170
```

