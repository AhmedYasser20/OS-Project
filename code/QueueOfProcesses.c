#include "QueueOfProcesses.h"
#include <stdlib.h>


struct QueueProcessNode* NewNode(struct Process item){
        struct QueueProcessNode * temp=(struct QueueProcessNode *)
         malloc(sizeof(struct QueueProcessNode));
         temp->key=item;
         temp->next=NULL;
         return temp;
}

struct Queue* CreateQueueOfProcess(){
    struct Queue* q=(struct Queue*)malloc(sizeof(struct Queue));
    q->head=q->tail=NULL;
    return q;
}

void Push(struct Queue *q,struct Process item){
    struct QueueProcessNode * temp=NewNode(item);
    if(q->tail==NULL){
        q->head=q->tail=temp;
        return;
    }
    q->tail->next=temp;
    q->tail=temp;
}

void Pop(struct Queue *q){
    if(q->head==NULL)
        return;
    
    struct QueueProcessNode* temp=q->head;
    q->head=q->head->next;

    if(q->head==NULL)
        q->tail=NULL;

    free(temp);    
}
