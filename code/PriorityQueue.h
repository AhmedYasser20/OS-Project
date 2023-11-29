// C code to implement Priority Queue 
// using Linked List 
#include <stdio.h> 
#include <stdlib.h> 
#include "DataStructures.h"


// Node 
typedef struct node { 
	int data; 
    struct Process p;

	// Lower values indicate higher priority 
	int priority; 

	struct node* next; 

} Node; 

// Function to Create A New Node 
Node* newNode(int d, int p);

// Return the value at head 
int peek(Node** head);


// Removes the element with the 
// highest priority from the list 
void pop(Node** head);

// Function to push according to priority 
void push(Node** head, int d, int p);

// Function to check is list is empty 
int isEmpty(Node** head);
