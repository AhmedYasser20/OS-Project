#ifndef QUEUE_PROCESS_H
#define QUEUE_PROCESS_H

#include "DataStructures.h"

struct QueueProcessNode
{
    struct Process key;
    struct QueueProcessNode *next;
};

struct Queue
{
    struct QueueProcessNode *head,*tail;
};

struct QueueProcessNode* NewNode(struct Process item);

struct Queue* CreateQueueOfProcess(); 

void Push(struct Queue *q,struct Process Item);

void Pop(struct Queue *q);

#endif