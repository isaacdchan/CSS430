#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node* head = NULL;
Task*** taskArrays;
int* arrayIndices;

void createTaskArrays (int numTasks) {
    taskArrays = calloc(10, numTasks * sizeof(Task));
    arrayIndices = calloc(10, sizeof(int));

    for (int i = 0; i < 10; i++) {
        taskArrays[i] = calloc(numTasks, sizeof(Task));
        arrayIndices[i] = 0;
    }

    struct node* curr = head;
    // iterate over every node created by the cpu
    while (curr != NULL) {
        // check the nodes priority and find the array it belongs to
        // bias of -1
        int priority = curr->task->priority-1;
        Task** taskArray = taskArrays[priority];
        int arrayIndex = arrayIndices[priority];

        // place it in the next available array index
        taskArray[arrayIndex] = curr->task;
        // increment the next available index
        arrayIndices[priority]++;
        curr = curr->next;
    }
}

void freeTaskArrays() {
    for (int i = 0; i < 10; i++) {
        free(taskArrays[i]);
    }
    free(taskArrays);
}

void add (char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->name = strdup(name);
    insert(&head, newTask);
}

// finds the task whose name comes first in dictionary
void schedule() {
    //add is run here
    int current_time = 0;
    sortByName(head);

    int numTasks = getSize(head);
    createTaskArrays(numTasks);

    Task* task;
    Task** taskArray;
    // for each taskArray

    for (int i = 9; i >= 0; i--) {
        taskArray = taskArrays[i];
        int completedTasks = 0;
        int tasksInArray = arrayIndices[i];

        while (true) {
            for (int i = 0; i < tasksInArray; i++) {
                task = taskArray[i];
                if (task->burst <= 0) {
                    continue;
                }

                int burstRemaining = task->burst;
                // quantum of 10
                if (burstRemaining <= 10) {
                    task->burst = 0;
                    completedTasks++;
                    current_time += burstRemaining;
                    run(task, burstRemaining);
                } else {
                    task->burst -= 10;
                    current_time += 10;
                    run(task, 10);
                }
                printf("Time is now: %d\n", current_time);
            }
            if (completedTasks == tasksInArray) {
                break;
            }
        }
    }

    freeList(head);
    freeTaskArrays();
}
