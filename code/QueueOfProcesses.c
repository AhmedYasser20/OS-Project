#include "QueueOfProcesses.h"
#include <stdlib.h>
#include <stdio.h>

/*
    Function name: NewNode
    Description: Creating Queue Node with process and set the next pointer to NULL
    Input: struct Process
    Output: struct QueueProcessNode* (Pointer)

*/
int c=0;
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
     if (c==0)
         {
            q->head=q->tail=NULL;
         }
    if (q->tail == NULL)
    {
        q->head = q->tail = temp;
        //printf("LAST PRINTF\n");
        c++;
        return;
    }
   // printf("LAST PRINTF11\n");
   // if ("q->tail %d",q->tail)
    q->tail->next = temp;
    q->tail = temp; 
    c++;
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

void PopID(struct Queue *q, int IDTXT ) // this id is the same as comming from text file 
{
    struct QueueProcessNode* temp=q->head;
    
     if(temp==NULL)
     {
        return;
     }
    struct QueueProcessNode* tempDummy=q->head;
  
    if (q->head->key.id == IDTXT)
    {
         q->head=q->head->next;
         c--;
        
         free(temp);
         return ;
    }

    while(temp->next !=NULL )
    {   

            if(temp->next->key.id==IDTXT)
            {
                 tempDummy=temp->next;
                temp->next=tempDummy->next;
                c--;
                free(tempDummy);
                
                return;
            }
            temp=temp->next;
            
    }
 
    return;
}