#include "PriorityQueue.h"


// Function to Create A New PriorityQueueProcessNode 
PriorityQueueProcessNode* newPriorityQueueProcessNode(struct Process pr) 
{ 
	PriorityQueueProcessNode* temp = (PriorityQueueProcessNode*)malloc(sizeof(PriorityQueueProcessNode)); 	
	temp->p = pr; 
	temp->next = NULL; 

	return temp; 
} 


PriorityQueueOfProcesses* CreatePriorityQueueOfProcesses(){
    PriorityQueueOfProcesses* q=(PriorityQueueOfProcesses*)malloc(sizeof(PriorityQueueOfProcesses));
	q->head=NULL;
    return q;
}

// Return the Process at head 
struct Process peek(PriorityQueueOfProcesses*queue) 
{ 
	return queue->head->p;
} 

// Removes the Process with the 
// highest priority from the list 
void pop(PriorityQueueOfProcesses*queue) 
{ 

	PriorityQueueProcessNode* temp = queue->head; 
	queue->head=queue->head->next;
	free(temp); 
} 

// Function to push according to priority 
void push(PriorityQueueOfProcesses *queue ,struct Process pr) 
{ 
	printf("pushing in PQ \n");
	PriorityQueueProcessNode* start = queue->head; 

	// Create new PriorityQueueProcessNode 
	PriorityQueueProcessNode* temp = newPriorityQueueProcessNode(pr); 

	// Special Case: The head of list has lesser 
	// priority than new node. So insert new 
	// node before head node and change head node. 
	if ( queue->head == NULL ) { 

		// Insert New Node before head 
		 
		queue->head= temp;
		printf("finishing pushing\n");
		return ;
	} 
	else if (queue->head->p.Priority > pr.Priority) { 

		// Insert New Node before head 
		temp->next = queue->head; 
		queue->head = temp; 
	} 
	else { 
		// Traverse the list and find a 
		// position to insert new node 
		while (start->next != NULL && start->next->p.Priority < pr.Priority) { 
			start = start->next; 
		} 

		// Either at the ends of the list 
		// or at required position 
		temp->next = start->next; 
		start->next = temp; 
	} 
	printf("finishing pushing\n");
} 


// Function to check is list is empty 
int isEmpty(PriorityQueueOfProcesses *queue) 
{ 
	return queue->head == NULL;
} 
