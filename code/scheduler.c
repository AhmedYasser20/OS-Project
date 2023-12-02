#include "headers.h"


int QueueKey;
struct Queue *ReadyQueue;
bool Genetor=true;

bool isRunning=false;

void SignalHandlerGentorEnd(int sig){
    printf("Flag");
    Genetor=false;
}

void SignalHandlerProcessesEnd(int sig){
    isRunning=false;
}

void ForkProcess(int x){
    int processid=fork();
        printf("Forked %d\n",processid);
    if(processid==0){

         char para=x;
         execl("process.out",&para,NULL);
    }
    if(processid<0){
        printf("Error while creating Scheduler");
        exit(-1);
    }
}

void HPF(){
    if(!isRunning && ReadyQueue->head!=NULL){
        isRunning=true;
        puts("HERE in HPF\n");
        ForkProcess(ReadyQueue->head->key.Runtime);
        Pop(ReadyQueue);
    }
}

int main(int argc , char*argv[]){
    printf("ID %d\n",getpid());
    signal(SIGUSR1,SignalHandlerGentorEnd);
    signal(SIGUSR2,SignalHandlerProcessesEnd);
    initClk();

    ReadyQueue=CreateQueueOfProcess();

    QueueKey=msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY,0666 | IPC_CREAT);

    do{
        struct MsgGeneratorScheduler temp;    
        int Rev=msgrcv(QueueKey,&temp,sizeof(temp.p),0,IPC_NOWAIT);
        if(Rev!=-1){
           Push(ReadyQueue,temp.p); 
        }
        HPF();    
    }while(Genetor || ReadyQueue->head!=NULL);
    
    
    //destroyClk(true);
    return 0;
}