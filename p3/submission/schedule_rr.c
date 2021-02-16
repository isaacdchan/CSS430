#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node* head = NULL;
struct task** tasks;

void add (char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->name = strdup(name);
    insert(&head, newTask);
}

void createTaskArray(int numTasks) {
    tasks = calloc(numTasks, sizeof(Task));
    struct node* temp = head;

    int i = 0;
    while (temp != NULL) {
        tasks[i] = temp->task;
        temp = temp->next;
        i++;
    }
}

void schedule() {
   int current_time = 0;
    sortByName(head);

    int numTasks = getSize(head);
    createTaskArray(numTasks);

    Task* task;
    int burstRemaining;
    int completedTasks = 0;
    while (true) {
        for (int i = 0; i < numTasks; i++) {
            task = tasks[i];
            if (task->burst <= 0) {
                continue;
            }

            burstRemaining = task->burst;
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
        if (completedTasks == numTasks) {
            break;
        }
    }

    freeList(head);
    free(tasks);
}