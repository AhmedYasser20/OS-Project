#include "linkedList.h"
#include <stdlib.h>
#include <stdio.h>

/*
    Function name: NewNode
    Description: Creating Queue Node with process and set the next pointer to NULL
    Input: struct Process
    Output: struct QueueProcessNode* (Pointer)

*/
struct linkedListProcessNode *NewNodeForLinkedList(struct Process item)
{
    struct linkedListProcessNode *temp = (struct linkedListProcessNode *)
        malloc(sizeof(struct linkedListProcessNode));
    temp->key = item;
    temp->next = NULL;
    return temp;
}

/*
    Function name: CreateQueueOfProcess
    Description: allocate memory for Queue (head and tail pointers) then return Pointer
                 Ref. to it
    Input: void
    Output: struct Queue*

*/







struct LinkList *CreateLinkedListOfProcess()
{
    struct LinkList *q = (struct LinkList *)malloc(sizeof(struct LinkList));
    q->head = q->tail = NULL;
    return q;
}

/*
    Function name: Push
    Description: Creating a Node using Process by call CreateQueueOfProcess() then add
                 it at the end of queue using struct Queue Pointer
    Input: struct Queue * , struct Process
    Output: void

*/

void PushLinkedList(struct LinkList *q,struct Process Item)
{
    struct linkedListProcessNode *temp = NewNodeForLinkedList(Item);
    if (q->tail == NULL)
    {
        q->head = q->tail = temp;
        return;
    }

    q->tail->next = temp;
    q->tail = temp; 
}

/*
    Function name: Pop
    Description: remove First Node in Queue (Head=Head->next)
    Input: struct Queue *
    Output: void

*/

void PopLinkedList(struct LinkList *q)
{
    if (q->head == NULL)
        return;

    struct linkedListProcessNode *temp = q->head;
    q->head = q->head->next;

    if (q->head == NULL)
        q->tail = NULL;

    free(temp);
}





void pop_idLinkedList(struct LinkList* queue, int targetId)
 {
    struct linkedListProcessNode* current = queue->head;
    struct linkedListProcessNode* previous = NULL;

    while (current != NULL && current->key.id != targetId) 
    {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        // Node with the specified ID not found
        //printf("Node with ID %d not found in the queue\n", targetId);
        return;
    }

    if (previous == NULL) 
    {
        // Node to be removed is the first node
        queue->head = current->next;
    } 
    else 
    {
        // Node to be removed is not the first node
        previous->next = current->next;
    }

    // If the removed node was the last node, update the trailer
    if (current == queue->tail)
     {
        queue->tail = previous;
    }

    // Free the memory of the removed node
    free(current);
}


void printLIn(struct LinkList* linked)
{
    struct linkedListProcessNode* current = linked->head;

    while(current!=NULL)
    {
        printf("node id : %d",current->key.id);
        current=current->next;
    }
}