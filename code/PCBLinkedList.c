#include "PCBLinkedList.h"

#include<malloc.h>




// Function to create a new node
struct NodePCB *createNodePCB(struct ProcessPCB process) 
{
    struct NodePCB *newNode = (struct NodePCB *)malloc(sizeof(struct NodePCB));
    newNode->Row = process;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a process at the end of the linked list
void insertEnd(struct NodePCB **head, struct ProcessPCB process) 
{
    struct NodePCB *newNode = createNodePCB(process);

    if (*head == NULL) {
        *head = newNode;
        return;
    }

    struct NodePCB *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = newNode;
}

// Function to print the elements in the linked list
void printLinkedList(struct NodePCB *head) 
{
    struct NodePCB *current = head;
    while (current != NULL) {
        printf("PID: %d, Priority: %d\n", current->Row.PID, current->Row.priority);
        current = current->next;
    }
}


void setStatus(struct NodePCB *head,int pid ,char * status,int remTime,int waitTime,int executionTIme ) 
{
    struct NodePCB *current = head;
    while (current->Row.PID !=pid)
     {
        current = current->next;
    }
    current->Row.status=status;
    current->Row.status=remTime;
    current->Row.waitingTime=waitTime;
    current->Row.execTime=executionTIme;
}

struct ProcessPCB  getStatus(struct NodePCB *head,int pid ) 
{
    struct NodePCB *current = head;
    while (current->Row.PID !=pid)
     {
        current = current->next;
    }
    return current->Row;
   
}

// Function to free the memory allocated for the linked list
void freeLinkedList(struct NodePCB *head) {
    while (head != NULL) 
    {
        struct NodePCB *temp = head;
        head = head->next;
        free(temp->Row.status);
        free(temp);
    }
}


