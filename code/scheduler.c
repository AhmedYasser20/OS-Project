#include "headers.h"


int QueueKey;               // Key of Queue between Scheduler and Processes Generator
struct Queue *ReadyQueue;   // Inital Queue using to store Processes That read Form Generator
bool Generator = true;      // Boolen used to Notify if Generator Finshed sending process or not
bool isRunning=false;       // Boolen used to Notify Schedular if Process is finshed or not  


/*
    Function name: SignalHandlerGentorEnd(
    Description:    Signal Handler for Signal User 1 - this Signal is one way form Generator to Scheduler 
                    Used to Set Generator Boolen
    Input: int
    Output: void 

*/

void SignalHandlerGentorEnd(int sig){
    Generator=false;
}

/*
    Function name: SignalHandlerProcessesEnd
    Description:    Signal Handler for Signal User 2 - this Signal is one way form  Forked Process to Scheduler 
                    Used to Set IsRunning Boolen
    Input: int
    Output: void 

*/

void SignalHandlerProcessesEnd(int sig){
    isRunning=false;
}

/*
    Function name: ForkProcess
    Description: This Function used to Fork the process and give it its Quantum
                 Quantum : Runtime of process after finshed it send Signal User 2 to Scheduler   
    Input: int
    Output: void 
*/


void ForkProcess(int Quantum){
    int processid=fork();
    if(processid==0){

         char para=Quantum;
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
    }while(Generator || ReadyQueue->head!=NULL);
    
    
    //destroyClk(true);
    return 0;
}