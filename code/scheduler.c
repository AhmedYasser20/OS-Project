#include "headers.h"


int QueueKey;               // Key of Queue between Scheduler and Processes Generator
struct PriorityQueueOfProcesses *HPFReadyQueue;   // Inital Queue using to store Processes That read Form Generator
bool Generator = true;      // Boolen used to Notify if Generator Finshed sending process or not
bool isRunning=false;       // Boolen used to Notify Schedular if Process is finshed or not  
struct Queue * RRreadyQ;

struct NodePCB* nodepcb;




/*
Function name: settingPCBRow (
    Description:    Setting PCB paraters of a certain 
                    
    Input: int
    Output: void 

*/
void settingPCBRow (int pid,struct Process procs,int TypeofRunningAlgo, int PPID,int Quantum )
{
    
    struct ProcessPCB PCB;
    PCB.execTime=0;
    PCB.PID=pid;
    PCB.TypeofRunningAlgo=TypeofRunningAlgo;
    PCB.remainingTime=procs.Runtime;
    PCB.PPID=PPID;
    PCB.Quantum=Quantum;
    PCB.status="Ready";
    PCB.priority=procs.Priority;
    PCB.waitingTime=0;
    insertEnd(& nodepcb,PCB );
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
    isRunning=false;
}

/*
    Function name: ForkProcess
    Description: This Function used to Fork the process and give it its Quantum
                 Quantum : Runtime of process after finshed it send Signal User 2 to Scheduler   
    Input: int
    Output: void 
*/


void ForkProcess(int Quantum)
{
    int processid=fork();
    if(processid==0)
    {

        

        char para=Quantum;
        //printf("para = %d \n",Quantum);
        execlp("./process.out", "process.out", &para, NULL);
        
    }
    if(processid<0){
        printf("Error while creating Scheduler");
        exit(-1);
    }
}



void HPF(){
    if(!isRunning && HPFReadyQueue->head!=NULL){
        isRunning=true;
        //puts("HERE in HPF\n");
        //printf("id = %d\n" , HPFReadyQueue->head->p.id );
        ForkProcess(HPFReadyQueue->head->p.Runtime);
        pop(HPFReadyQueue);
    }
}
void RR(int Quantum)
{
    if(!isRunning && RRreadyQ->head !=NULL)
    {
        isRunning=true;
        //int pid,int TypeofRunningAlgo,int remainingTime, int PPID,int Quantum,char *status,int priority,int waitingTime,int execTime
        settingPCBRow(RRreadyQ->head->key);
        ForkProcess(Quantum);
        Push(RRreadyQ,RRreadyQ->head->key);
        Pop(RRreadyQ);
    }
}

int main(int argc , char*argv[]){
    
    signal(SIGUSR1,SignalHandlerGentorEnd);
    signal(SIGUSR2,SignalHandlerProcessesEnd);
    initClk();
    if((int) *(argv[1]) ==1)
    {
    HPFReadyQueue=CreatePriorityQueueOfProcesses(); 
    }
    else  if( (int) *(argv[1]) ==3)
    {
    RRreadyQ=CreateQueueOfProcess(); 
    }
    int Quantum=(int) (*(argv[2] ) );
    

    QueueKey=msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY,0666 | IPC_CREAT);

    do{
        struct MsgGeneratorScheduler temp;    
        int Rev=msgrcv(QueueKey,&temp,sizeof(temp.p),0,  IPC_NOWAIT);
        //printf("id = %d   rev = %d\n", temp.p.id , Rev);
        if(Rev!=-1){
            //printf("i'm here \n");
            if( (int) *(argv[1]) ==3)
            {
                Push(RRreadyQ,temp.p); 
            }
            else if( (int) *(argv[1]) ==1)
            {
           push(HPFReadyQueue,temp.p); 
            }

           //printf("id = %d\n",HPFReadyQueue->head->p.id);
        }

        if( (int) *(argv[1]) ==1)
            {
                HPF();  
            }
            else if((int) *(argv[1]) ==3)
            {
                RR(Quantum) ;
            }
      
     
    }while( isRunning || Generator || HPFReadyQueue->head!=NULL);
    
    
    //destroyClk(true);
    return 0;
}