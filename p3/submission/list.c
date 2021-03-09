#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"
#include "task.h"


// add a new task to the list of tasks
void insert(struct node **head, Task *newTask) {
    // add the new task to the list 
    struct node *newNode = malloc(sizeof(struct node));

    newNode->task = newTask;
    newNode->next = *head;
    *head = newNode;
}

int getSize(struct node* head) {
    int size = 0;
    struct node* temp;
    temp = head;
    while (temp != NULL) {
        size++;
        temp = temp->next;
    }

    return size;
}

/* function to swap data of two nodes a and b*/
void swap(struct node* a, struct node* b) {
    Task* temp = a->task;
    a->task = b->task;
    b->task = temp;
}

bool compareTaskName(char *a, char *b) { return strcmp(a, b) < 0; }

void sortByName(struct node* head) {
    int swapped;
    struct node *curr;
    struct node *prev = NULL;

    /* Checking for empty list */
    if (head == NULL)
        return;

    do {
        swapped = 0;
        curr = head;

        while (curr->next != prev) {
            if (!compareTaskName(curr->task->name, curr->next->task->name)) {
                swap(curr, curr->next);
                swapped = 1;
            }
            curr = curr->next;
        }
        prev = curr;
    }
    while (swapped);
}

void sortByPriority(struct node* head) {
    int swapped;
    struct node *curr;
    struct node *prev = NULL;

    /* Checking for empty list */
    if (head == NULL)
        return;

    do {
        swapped = 0;
        curr = head;

        while (curr->next != prev) {
            if (curr->task->priority < curr->next->task->priority) {
                swap(curr, curr->next);
                swapped = 1;
            }
            curr = curr->next;
        }
        prev = curr;
    }
    while (swapped);
}

void sortByBurst(struct node* head) {
    int swapped;
    struct node *curr;
    struct node *prev = NULL;

    /* Checking for empty list */
    if (head == NULL)
        return;

    do {
        swapped = 0;
        curr = head;

        while (curr->next != prev) {
            if (curr->task->burst > curr->next->task->burst) {
                swap(curr, curr->next);
                swapped = 1;
            }
            curr = curr->next;
        }
        prev = curr;
    }
    while (swapped);
}

void freeList(struct node* head) {
    struct node* next;

    while (head) {
        next = head->next;
        free(head->task->name);
        free(head->task);
        free(head);
        head = next;
    }
}
