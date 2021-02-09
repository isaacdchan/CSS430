/**
 * list data structure containing the tasks in the system
 */

#include "task.h"

struct node {
    Task *task;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, Task *task);
void delete(struct node **head, Task *task);
int getSize(struct node* head);
void sortByName(struct node* head);
void sortByPriority(struct node* head);
void sortByBurst(struct node* head);
void freeList(struct node* head);
