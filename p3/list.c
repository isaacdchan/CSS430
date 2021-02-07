#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"


void swap(struct node *pNode, struct node *pNode1);

// add a new task to the list of tasks
void insert(struct node **head, Task *newTask) {
    // add the new task to the list 
    struct node *newnode = malloc(sizeof(struct node));

    newnode->task = newTask;
    newnode->next = *head;
    *head = newnode;
}

// delete the selected task from the list
void delete(struct node **head, Task *task) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    // special case - beginning of list
    if (strcmp(task->name,temp->task->name) == 0) {
        *head = (*head)->next;
    }
    else {
        // interior or last element in the list
        prev = *head;
        temp = temp->next;
        while (strcmp(task->name,temp->task->name) != 0) {
            prev = temp;
            temp = temp->next;
        }

        prev->next = temp->next;
    }
}

void bubbleSort(struct node *head) {
    int swapped, i;
    struct node *curr;
    struct node *prev = NULL;

    /* Checking for empty list */
    if (head == NULL)
        return;

    do {
        swapped = 0;
        curr = head;

        while (curr->next != prev) {
            if (curr->data > curr->next->data) {
                swap(curr, curr->next);
                swapped = 1;
            }
            curr = curr->next;
        }
        lptr = curr;
    }
    while (swapped);
}

/* function to swap data of two nodes a and b*/
void swap(struct node *pNode, struct node *pNode1) {
    Task temp = a->task;
    a->task = b->task;
    b->task = temp;
}
