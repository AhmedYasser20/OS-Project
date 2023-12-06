#include "headers.h"


int QueueKey;               // Key of Queue between Scheduler and Processes Generator
struct PriorityQueueOfProcesses *HPFReadyQueue;   // Inital Queue using to store Processes That read Form Generator
bool Generator = true;      // Boolen used to Notify if Generator Finshed sending process or not
bool isRunning=false;       // Boolen used to Notify Schedular if Process is finshed or not  
ProcessPCB * PCB_Array;
int LastPlaceInArray;
int processID_Now;
int QueueProcessesKey;
struct Queue *RRreadyQ;
int pidnow;
int TempCount=0;
int Quantum;
int Algo;

void RevAndSetMsgFormProcesses(){
    MessageBetweenProcessAndScheduler temp;
    msgrcv(QueueProcessesKey,&temp,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),0,IPC_NOWAIT);
    PCB_Array[processID_Now].ExecTime=temp.ExceTime;
    PCB_Array[processID_Now].RemainingTime=temp.remainingtime;
    PCB_Array[processID_Now].EndTime=getClk();
    if(temp.remainingtime<=0){
            if(Algo==3){Pop(RRreadyQ);}
           temp.Order = END;
        }
    else{
        temp.Order= WAITING;
    }    
    temp.type=pidnow;
    msgsnd(QueueProcessesKey,&temp,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),IPC_NOWAIT);
}

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
    RevAndSetMsgFormProcesses();
    isRunning=false;
}

/*
    Function name: ForkProcess
    Description: This Function used to Fork the process and give it its Quantum
                 Quantum : Runtime of process after finshed it send Signal User 2 to Scheduler   
    Input: int
    Output: void 
*/

void CreateMsgAndSend(int pid,int q){
            MessageBetweenProcessAndScheduler temp2;
            temp2.type=pid;
            temp2.ExceTime=PCB_Array[processID_Now].ExecTime;
            temp2.remainingtime=PCB_Array[processID_Now].RemainingTime;
            temp2.Qutam=q;
            temp2.Order=START;
            msgsnd(QueueProcessesKey,&temp2,(sizeof(temp2.ExceTime)+sizeof(temp2.Order)+sizeof(temp2.remainingtime)+sizeof(temp2.Qutam)),IPC_NOWAIT);
}



void ForkProcess(int Quantum){
    int processid=fork();
    CreateMsgAndSend(processid,Quantum);
    pidnow=processid;
    if(processid==0){
        char para=Quantum;
        execlp("./process.out", "process.out", &para, NULL);
    }
    if(processid<0){
        printf("Error while creating Scheduler");
        exit(-1);
    }
}

void SetPCB_Array(struct Process p){
    ProcessPCB temp;
    temp.P=p;
    temp.itsLocationInArray=LastPlaceInArray;
    temp.RemainingTime=p.Runtime;
    temp.State=Waiting;
    temp.ExecTime=0;
    temp.WaitingTime=0;
    PCB_Array[LastPlaceInArray]=temp;
    PCB_Array[LastPlaceInArray].itsLocationInArray=LastPlaceInArray;
    LastPlaceInArray++;
}

void PrintPCBArray()
{
    for (int i = 0; i < LastPlaceInArray; i++)
    {
        printf("start time =%d  Exec Time =%d  End time =%d  id=%d \n",PCB_Array[i].StartTime,PCB_Array[i].ExecTime,PCB_Array[i].EndTime,PCB_Array[i].P.id);
    }
    
}






void HPF(){

    /*
    
    
    
        CHANGE PROCESS END its Self
    
    
    
    */



    if(!isRunning && HPFReadyQueue->head!=NULL){
        SetPCB_Array(HPFReadyQueue->head->p); 
        PCB_Array[TempCount].StartTime=getClk();
        PCB_Array[TempCount].State=Running;
        PCB_Array[TempCount].itsLocationInArray=TempCount;
        processID_Now=PCB_Array[TempCount].itsLocationInArray;
        isRunning=true;
        ForkProcess(HPFReadyQueue->head->p.Runtime);
        pop(HPFReadyQueue);
        TempCount++;
    }
}

void RoundRobin(){
    if (!isRunning && RRreadyQ->head != NULL)
    {
        
        ForkProcess(Quantum);
        Push(RRreadyQ,RRreadyQ->head->key);
        Pop(RRreadyQ);
    }
}


int main(int argc , char*argv[]){
    initClk();
    Quantum = (*(argv[1]))-'0';
    Algo=(*(argv[0]))-'0';
       
    signal(SIGUSR1,SignalHandlerGentorEnd);
    signal(SIGUSR2,SignalHandlerProcessesEnd);
    PCB_Array=CreatePCB_Array();
   
    QueueProcessesKey=msgget(MSG_QUEUE_SCHEDULER_PROCESS_KEY,0666 | IPC_CREAT);
    HPFReadyQueue=CreatePriorityQueueOfProcesses();
    QueueKey=msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY,0666 | IPC_CREAT);

    do{
        struct MsgGeneratorScheduler temp;    
        int Rev=msgrcv(QueueKey,&temp,sizeof(temp.p),0,  IPC_NOWAIT);
        if (Rev != -1)
        {
            if (Algo == 1)
            {
                push(HPFReadyQueue, temp.p);
            }
            else if (Algo == 3)
            {  
               Push(RRreadyQ,temp.p);
               SetPCB_Array(RRreadyQ->head->key);
               PCB_Array[TempCount].itsLocationInArray=TempCount;
               TempCount++;
            }
        }
        if (Algo == 1)
        {
            HPF();
        }
        else if (Algo == 3)
        {
            RoundRobin();
        }  
    }while( isRunning || Generator || HPFReadyQueue->head!=NULL);
    PrintPCBArray();
    DestoryedPCB_Array(PCB_Array);
    msgctl(QueueProcessesKey,IPC_RMID,(struct msqid_ds *)0);
    destroyClk(true);
    return 0;
}