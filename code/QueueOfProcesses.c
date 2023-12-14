#include "QueueOfProcesses.h"
#include <stdlib.h>
#include <stdio.h>

/*
    Function name: NewNode
    Description: Creating Queue Node with process and set the next pointer to NULL
    Input: struct Process
    Output: struct QueueProcessNode* (Pointer)

*/
struct QueueProcessNode *NewNode(struct Process item)
{
    struct QueueProcessNode *temp = (struct QueueProcessNode *)
        malloc(sizeof(struct QueueProcessNode));
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

struct Queue *CreateQueueOfProcess()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
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

void Push(struct Queue *q, struct Process item)
{
    struct QueueProcessNode *temp = NewNode(item);
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

void Pop(struct Queue *q)
{
    if (q->head == NULL)
        return;

    struct QueueProcessNode *temp = q->head;
    q->head = q->head->next;

    if (q->head == NULL)
        q->tail = NULL;

    free(temp);
}

// void PopID(struct Queue *q, int IDTXT ) // this id is the same as comming from text file 
// {
//     struct QueueProcessNode* temp=q->head;
    
//      if(temp==NULL)
//      {
//         return;
//      }
//     struct QueueProcessNode* tempDummy=q->head;
  
//     if (q->head->key.id == IDTXT)
//     {
//          q->head=q->head->next;
//          c--;
        
//          free(temp);
//          return ;
//     }

//     while(temp->next !=NULL )
//     {   

//             if(temp->next->key.id==IDTXT)
//             {
//                  tempDummy=temp->next;
//                 temp->next=tempDummy->next;
//                 c--;
//                 free(tempDummy);
                
//                 return;
//             }
//             temp=temp->next;
            
//     }
 
//     return;
// }




void pop_id(struct Queue* queue, int targetId)
 {
    struct QueueProcessNode* current = queue->head;
    struct QueueProcessNode* previous = NULL;

    while (current != NULL && current->key.id != targetId) 
    {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        // Node with the specified ID not found
        printf("Node with ID %d not found in the queue\n", targetId);
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