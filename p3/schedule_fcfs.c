#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;

bool comesBefore(char *a, char *b) {
    return strcmp(a, b) < 0;
}

void add (char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->name = strdup(name);
    insert(&head, newTask);
}

// based on traverse from list.c
// finds the task whose name comes first in dictionary
Task *pickNextTask() {
    // if list is empty, nothing to do
    if (!head)
        return NULL;

    struct node *temp;
    temp = head;
    Task *best_sofar = temp->task;

    while (temp != NULL) {
        if (comesBefore(temp->task->name, best_sofar->name))
            best_sofar = temp->task;
        temp = temp->next;
    }
    // delete the node from list, Task will get deleted later
    delete (&head, best_sofar);
    return best_sofar;
}

void schedule() {
    int current_time = 0;
    Task* task = pickNextTask();
    while (task) {
        run(task, task->burst);
        current_time += task->burst;
        free(task->name);
        free(task);
        task = pickNextTask();
    }
}