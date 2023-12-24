

#include "DataStructures.h"

/*
    Structure of Processes LinkList
        =>Head, tail Pointer to Nodes
        =>Node {Process , pointer to the next processes}

*/

struct linkedListProcessNode
{
    struct Process key;
    struct linkedListProcessNode *next;
};

struct LinkList
{
    struct linkedListProcessNode *head,*tail;
};

/*
    Function name: NewNode
    Description: Creating LinkList Node with process and set the next pointer to NULL
    Input: struct Process 
    Output: struct linkedListProcessNode* (Pointer) 

*/


struct linkedListProcessNode* NewNodeForLinkedList(struct Process item);


/*
    Function name: CreatelinkedListProcessNode
    Description: allocate memory for Queue (head and tail pointers) then return Pointer
                 Ref. to it
    Input: void
    Output: struct Queue* 

*/

struct LinkList* CreateLinkedListOfProcess(); 

/*
    Function name: Push
    Description: Creating a Node using Process by call CreateQueueOfProcess() then add
                 it at the end of queue using struct Queue Pointer
    Input: struct Queue * , struct Process 
    Output: void 

*/

void PushLinkedList(struct LinkList *q,struct Process Item);

/*
    Function name: Pop
    Description: remove First Node in Queue (Head=Head->next)
    Input: struct Queue * 
    Output: void 

*/

void PopLinkedList(struct LinkList *q);


//void PopID(struct LinkedList *q,int IDTXT);

void pop_idLinkedList(struct LinkList* linked, int targetId);


void printLIn(struct LinkList* linked);

