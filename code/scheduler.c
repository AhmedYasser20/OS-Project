#include "headers.h"

int QueueKey;                                   // Key of Queue between Scheduler and Processes Generator
struct PriorityQueueOfProcesses *HPFReadyQueue; // Inital Queue using to store Processes That read Form Generator
bool Generator = true;                          // Boolen used to Notify if Generator Finshed sending process or not
bool isRunning = false;                         // Boolen used to Notify Schedular if Process is finshed or not
ProcessPCB *PCB_Array;
int LastPlaceInArray;
int processID_Now;  
int processID_Min_RT;
int Processid_run_now = -1;     // Processid_run_now 
int Rem_CurrentP =0;
int Start_processing_time;
int QueueProcessesKey;         // From Processes to Scheduler
int QueueProcessesKey2;       // From Scheduler to processes
int pidnow;
int TempCount = 0;
int Quantum;
int Algo;
struct Queue *RRreadyQ;
struct Queue *SRTNreadyQ;

// 1- Fix Clock
// 2- Sleep Processes
// 3- End Processes =>
// 4- Abo ismail
// 5- Output

void RevAndSetMsgFormProcesses()
{
    // puts("HERE");
    MessageBetweenProcessAndScheduler temp;

    msgrcv(QueueProcessesKey2, &temp, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), 0, IPC_NOWAIT);
    // printf("What i Rev As Sch type %ld,Remtime=%d,ExceTime=%d,Qutm=%d,ORDER=%d \n",temp.type,temp.remainingtime,temp.ExceTime,temp.Qutam,temp.Order);
    PCB_Array[processID_Now].ExecTime = temp.ExceTime;
    PCB_Array[processID_Now].RemainingTime = temp.remainingtime;
    PCB_Array[processID_Now].EndTime = getClk();
    if (temp.remainingtime <= 0)
    {
        temp.Order = END;
        PCB_Array[processID_Now].State = End;
    }
    else
    {
        temp.Order = WAIT;
        PCB_Array[processID_Now].State = Waitting;
    }
    temp.type = PCB_Array[processID_Now].Pid;
    msgsnd(QueueProcessesKey, &temp, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), IPC_NOWAIT);
    // printf("What i send As Sch type %ld,Remtime=%d,ExceTime=%d,Qutm=%d,ORDER=%d \n",temp.type,temp.remainingtime,temp.ExceTime,temp.Qutam,temp.Order);
}

/*
    Function name: SignalHandlerGentorEnd(
    Description:    Signal Handler for Signal User 1 - this Signal is one way form Generator to Scheduler
                    Used to Set Generator Boolen
    Input: int
    Output: void

*/

void SignalHandlerGentorEnd(int sig)
{
    Generator = false;
}

/*
    Function name: SignalHandlerProcessesEnd
    Description:    Signal Handler for Signal User 2 - this Signal is one way form  Forked Process to Scheduler
                    Used to Set IsRunning Boolen
    Input: int
    Output: void

*/

void SignalHandlerProcessesEnd(int sig)
{
    RevAndSetMsgFormProcesses();
    isRunning = false;
}

/*
    Function name: ForkProcess
    Description: This Function used to Fork the process and give it its Quantum
                 Quantum : Runtime of process after finshed it send Signal User 2 to Scheduler
    Input: int
    Output: void
*/

void CreateMsgAndSend(int pid, int q)
{
    MessageBetweenProcessAndScheduler temp2;

    temp2.type = pid;
    temp2.ExceTime = PCB_Array[processID_Now].ExecTime;
    temp2.remainingtime = PCB_Array[processID_Now].RemainingTime;
    temp2.Qutam = q;
    temp2.Order = START;

    msgsnd(QueueProcessesKey, &temp2, (sizeof(temp2.ExceTime) + sizeof(temp2.Order) + sizeof(temp2.remainingtime) + sizeof(temp2.Qutam)), IPC_NOWAIT);
}


void ForkProcess(int Quantum)
{
    int processid = fork();
    CreateMsgAndSend(processid, Quantum);
    PCB_Array[processID_Now].Pid = processid;
    if (processid == 0)
    {
        char para = Quantum;
        execlp("./process.out", "process.out", &para, NULL);
    }
    if (processid < 0)
    {
        printf("Error while creating Scheduler");
        exit(-1);
    }
}

void SetPCB_Array(struct Process p)
{
    ProcessPCB temp;

    temp.P = p;
    temp.itsLocationInArray = LastPlaceInArray;
    temp.StartTime = 0;
    temp.EndTime = 0;
    temp.RemainingTime = p.Runtime;
    temp.State = Ready;
    temp.ExecTime = 0;
    temp.WaitingTime = 0;
    temp.Pid = -1;

    PCB_Array[LastPlaceInArray] = temp;
    LastPlaceInArray++;
}

void PrintPCBArray()
{
    for (int i = 0; i < LastPlaceInArray; i++)
    {
        printf("process id %d Arrive Time %d Runtime %d Priority %d \n", PCB_Array[i].P.id, PCB_Array[i].P.ArriveTime, PCB_Array[i].P.Runtime, PCB_Array[i].P.Priority);
        printf("start time =%d  Exec Time =%d  End time =%d  id=%d \n", PCB_Array[i].StartTime, PCB_Array[i].ExecTime, PCB_Array[i].EndTime, PCB_Array[i].P.id);
        printf("ITSLoc %d , PID %d , STATE %d\n", PCB_Array[i].itsLocationInArray, PCB_Array[i].Pid, PCB_Array[i].State);
    }
}

void HPF()
{
    if (!isRunning && HPFReadyQueue->head != NULL)
    {
        //SetPCB_Array(HPFReadyQueue->head->p);
        PCB_Array[TempCount].StartTime = getClk();
        PCB_Array[TempCount].State = Running;
        PCB_Array[TempCount].itsLocationInArray = TempCount;
        processID_Now = PCB_Array[TempCount].itsLocationInArray;
        isRunning = true;
        ForkProcess(HPFReadyQueue->head->p.Runtime);
        pop(HPFReadyQueue);
        TempCount++;
    }
}

int SearchInPCBArray(int id)
{
    for (int i = 0; i < LastPlaceInArray; i++)
    {
        if (PCB_Array[i].P.id == id)
        {
            return PCB_Array[i].itsLocationInArray;
        }
    }
    return -1;
}

int Search_Min_in_pcb()
{
    int min = PCB_Array[0].RemainingTime;
    int MIN_index = -1;
    for(int i=1; i< LastPlaceInArray ; i++)
    {
        if(PCB_Array[i].RemainingTime < min )
        {
            MIN_index = i;
        }
    }
    return MIN_index;
}

/* if i get P :
- nothing runing {first / if P end}
- something is running and the current running not the mini

- something is running and the current running is the mini

*/// 
void SRTN()
{
    
    // processID_Now = SearchInPCBArray(SRTNreadyQ->head->key.id);
    processID_Min_RT = Search_Min_in_pcb();
    processID_Now = processID_Min_RT;

    
    
    if(isRunning )
    {
        PCB_Array[Processid_run_now].RemainingTime =  Rem_CurrentP - (getClk() - Start_processing_time);
        Rem_CurrentP = PCB_Array[Processid_run_now].RemainingTime ;
    }
    if ( (PCB_Array[processID_Min_RT].RemainingTime < Rem_CurrentP && SRTNreadyQ->head != NULL) || ( !isRunning && SRTNreadyQ->head != NULL) )
    {
        if(!isRunning) // nothing running
        {
            
            Rem_CurrentP = PCB_Array[processID_Min_RT].RemainingTime;
            Start_processing_time = getClk();
           
            if (PCB_Array[processID_Now].State == Ready)
            {
                processID_Now = processID_Min_RT;
                PCB_Array[processID_Now].StartTime = getClk();
                PCB_Array[processID_Now].State = Running;
                isRunning = true;
                Processid_run_now = processID_Min_RT;
                ForkProcess(PCB_Array[processID_Now].RemainingTime);
            }
            else if(PCB_Array[processID_Min_RT].State == Waitting ) // case: a process RemT is the minimum
            {
                isRunning = true;
                kill(PCB_Array[processID_Min_RT].Pid , SIGUSR1);// SIG CONT
                PCB_Array[processID_Now].State = Running;
                Processid_run_now = processID_Min_RT;
            }
        }
        else if(isRunning) // case: the running process is not the minimum 
        {
                kill(PCB_Array[Processid_run_now].Pid , SIGUSR2); //SIG stop
                isRunning = false;
                // Processid_run_now = processID_Min_RT;
                if (PCB_Array[processID_Now].State == Ready)
                {
                    Processid_run_now = processID_Min_RT;
                    Start_processing_time = getClk();
                    Rem_CurrentP = PCB_Array[processID_Min_RT].RemainingTime ;
                    PCB_Array[processID_Now].StartTime = getClk();
                    PCB_Array[processID_Now].State = Running;
                    isRunning = true;
                    ForkProcess(PCB_Array[processID_Now].RemainingTime);
                }
        }     

    }
}

void RoundRobin()
{
    if (!isRunning && RRreadyQ->head != NULL)
    {

        isRunning = true;
        processID_Now = SearchInPCBArray(RRreadyQ->head->key.id);
        if (PCB_Array[processID_Now].State == Ready)
        {
            PCB_Array[processID_Now].StartTime = getClk();
            PCB_Array[processID_Now].State = Running;
            ForkProcess(Quantum);
        }
        else if (PCB_Array[processID_Now].State == Waitting)
        {
            PCB_Array[processID_Now].State = Running;
            CreateMsgAndSend(PCB_Array[processID_Now].Pid, Quantum);
        }
        else if (PCB_Array[processID_Now].State == End)
        {
            // POP
            Pop(RRreadyQ);
            isRunning = false;
            return;
        }
        Push(RRreadyQ, RRreadyQ->head->key);
        Pop(RRreadyQ);
    }
}

int main(int argc, char *argv[])
{
    initClk();
    Quantum = (*(argv[1])) - '0';
    Algo = (*(argv[0])) - '0';
    printf("Algo =%d Q =%d \n", Algo, Quantum);
    signal(SIGUSR1, SignalHandlerGentorEnd);
    signal(SIGUSR2, SignalHandlerProcessesEnd);
    PCB_Array = CreatePCB_Array();

    QueueProcessesKey = msgget(MSG_QUEUE_FROM_SCHEDULER_TO_PROCESS_KEY, 0666 | IPC_CREAT);
    QueueProcessesKey2 = msgget(MSG_QUEUE_FROM_PROCESS_TO_SCHEDULER_KEY, 0666 | IPC_CREAT);

    HPFReadyQueue = CreatePriorityQueueOfProcesses();
    RRreadyQ = CreateQueueOfProcess();
    SRTNreadyQ = CreateQueueOfProcess();

    QueueKey = msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY, 0666 | IPC_CREAT);

    do
    {
        struct MsgGeneratorScheduler temp;
        int Rev = msgrcv(QueueKey, &temp, sizeof(temp.p), 0, IPC_NOWAIT);
        if (Rev != -1)
        {
            if (Algo == 1)//HPF
            {
                push(HPFReadyQueue, temp.p);
                SetPCB_Array(HPFReadyQueue->head->p);
            }//SRTN
            else if(Algo == 2)
            {
                push(SRTNreadyQ,temp.p);
                SetPCB_Array(temp.p);
            }
            else
            {
                Push(RRreadyQ, temp.p);
                SetPCB_Array(temp.p);
            }
        }
        if (Algo == 1)
        {
            HPF();
        }
        else if(Algo == 2)
        {
                SRTN();
        }
        else if (Algo == 3)
        {
            RoundRobin();
        }

    } while (isRunning || Generator || HPFReadyQueue->head != NULL || RRreadyQ->head != NULL);
    PrintPCBArray();
    DestoryedPCB_Array(PCB_Array);
    msgctl(QueueProcessesKey, IPC_RMID, (struct msqid_ds *)0);
    msgctl(QueueProcessesKey2, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    return 0;
}