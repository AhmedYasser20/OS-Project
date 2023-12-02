#include "headers.h"


int QueueKey;

bool Genetor=true;

void SignalHandlerProcessesEnd(int sig){
    Genetor=false;
}

int main(int argc , char*argv[]){
    
    signal(SIGUSR1,SignalHandlerProcessesEnd);
    initClk();

    struct Queue *WaitingQueue=CreateQueueOfProcess();

    QueueKey=msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY,0666 | IPC_CREAT);

    do{
        struct MsgGeneratorScheduler temp;    
        msgrcv(QueueKey,&temp,sizeof(temp.p),0,!IPC_NOWAIT);
        
        Push(WaitingQueue,temp.p);
    }while(Genetor);
    
    //destroyClk(true);
    return 0;
}