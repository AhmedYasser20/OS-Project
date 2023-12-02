#include "headers.h"


int QueueKey;
struct Queue *ReadyQueue;
bool Genetor=true;

void SignalHandlerProcessesEnd(int sig){
    printf("Flag");
    Genetor=false;
}


void HPF(){
    for (int i = 0; i < 5; i++)
    {
        printf("HPF \n");
        sleep(5);
    }
    
}

int main(int argc , char*argv[]){
    printf("ID %d\n",getpid());
    signal(SIGUSR1,SignalHandlerProcessesEnd);
    initClk();

    ReadyQueue=CreateQueueOfProcess();

    QueueKey=msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY,0666 | IPC_CREAT);

    do{
        struct MsgGeneratorScheduler temp;    
        int Rev=msgrcv(QueueKey,&temp,sizeof(temp.p),0,IPC_NOWAIT);
        if(Rev!=-1){
           printf("ID %d\n",temp.p.id);
        }
    }while(Genetor);
    HPF();
    
    //destroyClk(true);
    return 0;
}