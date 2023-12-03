// C code to implement Priority Queue 
// using Linked List 
#pragma once
#include <stdio.h> 
#include <stdlib.h> 
#include "DataStructures.h"


// PriorityQueueProcessNode 
typedef struct PriorityQueueProcessNode { 
    struct Process p;
	//Lower values indicate higher priority 

	struct PriorityQueueProcessNode* next; 

} PriorityQueueProcessNode; 

typedef struct PriorityQueueOfProcesses{
	PriorityQueueProcessNode* head;
}PriorityQueueOfProcesses;


PriorityQueueOfProcesses* CreatePriorityQueueOfProcesses();
// Function to Create A New PriorityQueueProcessNode 
PriorityQueueProcessNode* newPriorityQueueProcessNode(struct Process p);

// Return the value at head 
struct Process peek(PriorityQueueOfProcesses *queue);


// Removes the element with the 
// highest priority from the list 
void pop(PriorityQueueOfProcesses*queue);

// Function to push according to priority 
void push(PriorityQueueOfProcesses *queue, struct Process pr);

// Function to check is list is empty 
int isEmpty(PriorityQueueOfProcesses *queue);
