#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"

struct node* head = NULL;

void add (char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->name = strdup(name);
    insert(&head, newTask);
}

// based on traverse from list.c
// finds the task whose name comes first in dictionary

void schedule() {
    int current_time = 0;
    sortByName(head);
    struct node* temp = head;

    while (temp) {
        Task* task = temp->task;
        run(task, task->burst);
        current_time += task->burst;
        printf("Time is now: %d\n", current_time);
        temp = temp->next;
    }

    freeList(head);
}