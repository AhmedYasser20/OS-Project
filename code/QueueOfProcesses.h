#ifndef QUEUE_PROCESS_H
#define QUEUE_PROCESS_H

#include "DataStructures.h"

/*
    Structure of Processes Queue
        =>Head, tail Pointer to Nodes
        =>Node {Process , pointer to the next processes}

*/

struct QueueProcessNode
{
    struct Process key;
    struct QueueProcessNode *next;
};

struct Queue
{
    struct QueueProcessNode *head,*tail;
};

/*
    Function name: NewNode
    Description: Creating Queue Node with process and set the next pointer to NULL
    Input: struct Process 
    Output: struct QueueProcessNode* (Pointer) 

*/


struct QueueProcessNode* NewNode(struct Process item);


/*
    Function name: CreateQueueOfProcess
    Description: allocate memory for Queue (head and tail pointers) then return Pointer
                 Ref. to it
    Input: void
    Output: struct Queue* 

*/

struct Queue* CreateQueueOfProcess(); 

/*
    Function name: Push
    Description: Creating a Node using Process by call CreateQueueOfProcess() then add
                 it at the end of queue using struct Queue Pointer
    Input: struct Queue * , struct Process 
    Output: void 

*/

void Push(struct Queue *q,struct Process Item);

/*
    Function name: Pop
    Description: remove First Node in Queue (Head=Head->next)
    Input: struct Queue * 
    Output: void 

*/

void Pop(struct Queue *q);


//void PopID(struct Queue *q,int IDTXT);

void pop_id(struct Queue* queue, int targetId);


#endif