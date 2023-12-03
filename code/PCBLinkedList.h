#include "DataStructures.h"



// Define the node structure for the linked list
struct NodePCB
 {
    struct ProcessPCB Row;
    struct NodePCB *next;
};

// Function to create a new node
struct NodePCB *createNodePCB(struct ProcessPCB process) ;

// Function to insert a process at the end of the linked list
void insertEnd(struct NodePCB **head, struct ProcessPCB process) ;

// Function to print the elements in the linked list
void printLinkedList(struct NodePCB *head) ;


void setStatus(struct NodePCB *head,int pid ,char * status,int remTime,int waitTime,int executionTIme ) ;

struct ProcessPCB * getStatus(struct NodePCB *head,int pid) ;

// Function to free the memory allocated for the linked list
void freeLinkedList(struct NodePCB *head);