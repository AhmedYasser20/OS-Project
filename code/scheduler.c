#include "headers.h"
#include <math.h>
FILE *schedulerLog;
FILE *MemoryLog;
FILE *schedulerPerf;
double diff_finish_start = 0;
double totalwaittingtime = 0;
double avg_WeightedTurnAroundTime;
double avg_waittingtime;

int QueueKey;                                   // Key of Queue between Scheduler and Processes Generator
struct PriorityQueueOfProcesses *HPFReadyQueue; // Inital Queue using to store Processes That read Form Generator
bool Generator = true;                          // Boolen used to Notify if Generator Finshed sending process or not
bool isRunning = false;                         // Boolen used to Notify Schedular if Process is finshed or not
ProcessPCB *PCB_Array;
int LastPlaceInArray = 0;
int processID_Min_RT;
int Processid_run_now = -1; // Processid_run_now
int Rem_CurrentP = 0;
int Start_processing_time;
int pidnow;
int Quantum;
int Algo;
buddyList *buddySystem;
struct LinkList *WaitingQueue;
struct Queue *RRreadyQ;
struct Queue *SRTNreadyQ;
struct Queue *FCFSreadyQ;
int dummyQuantum = 3;
bool lastone = false;
struct QueueProcessNode *current;
int startRoundRobin = 0;

void SetPCB_Array(struct Process p, Node *tempMemoryNode)
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
    temp.StopTime = 0;
    temp.memoryBlock=tempMemoryNode;
    PCB_Array[LastPlaceInArray] = temp;
    LastPlaceInArray++;
}


Node *AlloacteMemoryForProcess(struct Process p)
{
    Node *tempNode = allocateMemory(buddySystem, p.memsize);
    if (tempNode)
    {
        fprintf(MemoryLog,"At time %d allocated %d bytes for process %d from %d to %d\n",getClk(), p.memsize,p.id,tempNode->startAddress,(tempNode->Size+tempNode->startAddress));
        printf("At time %d allocated %d bytes for process %d from %d to %d\n",getClk(), p.memsize,p.id,tempNode->startAddress,(tempNode->Size+tempNode->startAddress));
        return tempNode;
    }
    puts("AYYYYYYY");
    PushLinkedList(WaitingQueue, p);
    return tempNode;
}

int AsignProcess(struct Process temp){
     Node *tempMemoryNode = AlloacteMemoryForProcess(temp);
        if (tempMemoryNode)
        {
            if (Algo == 1) // HPF
            {
                push(HPFReadyQueue, temp);
            } // SRTN
            else if (Algo == 2)
            {
                Push(SRTNreadyQ, temp);
            }
            else if (Algo == 3)
            {
                if (RRreadyQ->head == NULL)
                {
                    Push(RRreadyQ, temp);
                    current = RRreadyQ->head;
                }
                else
                {
                    Push(RRreadyQ, temp);
                }
            }
            SetPCB_Array(temp,tempMemoryNode);
            return 1;
        }
        return 0;
}

void RevFrmGenetor()
{
    struct MsgGeneratorScheduler temp;
    int Rev = msgrcv(QueueKey, &temp, sizeof(temp.p), 0, IPC_NOWAIT);

    if (Rev != -1)
    {
        puts("ABOSNOW ASSigned");
        AsignProcess(temp.p);
    }
    
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
    Generator = 0;
}

void dellocateMemoryForProcess(){
    printf("At time %d freed %d bytes from process %d from %d to %d\n",getClk(),PCB_Array[Processid_run_now].P.memsize,PCB_Array[Processid_run_now].P.id,PCB_Array[Processid_run_now].memoryBlock->startAddress,PCB_Array[Processid_run_now].memoryBlock->startAddress+PCB_Array[Processid_run_now].memoryBlock->Size);
    fprintf(MemoryLog,"At time %d freed %d bytes from process %d from %d to %d\n",getClk(),PCB_Array[Processid_run_now].P.memsize,PCB_Array[Processid_run_now].P.id,PCB_Array[Processid_run_now].memoryBlock->startAddress,PCB_Array[Processid_run_now].memoryBlock->startAddress+PCB_Array[Processid_run_now].memoryBlock->Size);
    dellocateMemory( buddySystem,PCB_Array[Processid_run_now].memoryBlock);
    printMemory(buddySystem);
}

void CheckWaitingQueue(){
    struct linkedListProcessNode* current= WaitingQueue->head;
    while(current){
      
        if(AsignProcess(current->key)){
            pop_idLinkedList(WaitingQueue,current->key.id);
        }   
        current=current->next;
    }
    printLIn(WaitingQueue);
}


/*
    Function name:  SignalHandlerProcessesEnd
    Description:    Signal Handler for Signal User 2 - this Signal is one way form  Forked Process to Scheduler
                    Used to Set IsRunning Boolen
    Input: int
    Output: void
*/

void SignalHandlerProcessesEnd(int sig)
{
    int status;
    int sid = wait(&status);

    PCB_Array[Processid_run_now].EndTime = getClk();
    PCB_Array[Processid_run_now].State = End;
    PCB_Array[Processid_run_now].RemainingTime = 0;
    dellocateMemoryForProcess();
    CheckWaitingQueue();
      
    if (Algo == 2)
        pop_id(SRTNreadyQ, PCB_Array[Processid_run_now].P.id);
    if (Algo == 3)
    {
        dummyQuantum = 0;
        while (startRoundRobin == getClk())
            ;
        int x = current->key.id;
      
        RevFrmGenetor();
        current = current->next;
        pop_id(RRreadyQ, x);
        if (current == NULL)
        {
            current = RRreadyQ->head;
        }

        lastone = false;
    }
    PCB_Array[Processid_run_now].TurnAroundTime = PCB_Array[Processid_run_now].EndTime - PCB_Array[Processid_run_now].P.ArriveTime;
    PCB_Array[Processid_run_now].WeightedTurnAroundTime = PCB_Array[Processid_run_now].TurnAroundTime / PCB_Array[Processid_run_now].P.Runtime;
    fprintf(schedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d  TA %.0f  WTA %.2f\n", getClk(), PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime, PCB_Array[Processid_run_now].TurnAroundTime, PCB_Array[Processid_run_now].WeightedTurnAroundTime);
    isRunning = false;
    diff_finish_start = getClk();
}

/*
    Function name: ForkProcess
    Description: This Function used to Fork the process and give it its Quantum
                 Quantum : Runtime of process after finshed it send Signal User 2 to Scheduler
    Input: int
    Output: void
*/

void ForkProcess(int Quantumtemp)
{

    isRunning = true;

    totalwaittingtime += getClk() - diff_finish_start;

    int processid = fork();

    if (processid == 0)
    {
        char para = Quantumtemp;
        execlp("./process.out", "process.out", &para, NULL);
    }
    if (processid < 0)
    {
        printf("Error while creating Scheduler");
        exit(-1);
    }
    PCB_Array[Processid_run_now].Pid = processid;
    PCB_Array[Processid_run_now].WaitingTime += (getClk() - PCB_Array[Processid_run_now].P.ArriveTime);

    fprintf(schedulerLog, "At time %d process %d starting arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime);
    PCB_Array[Processid_run_now].RemainingTime = PCB_Array[Processid_run_now].RemainingTime - Quantum;
    if (PCB_Array[Processid_run_now].RemainingTime <= 0)
    {
        lastone = true;
    }
}

void RemoveFromPCB_Array(int indx)
{
    PCB_Array[indx].Pid = -5000;
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

void HPF()
{
    if (!isRunning && HPFReadyQueue->head != NULL)
    {

        Processid_run_now = SearchInPCBArray(HPFReadyQueue->head->p.id);
        PCB_Array[Processid_run_now].StartTime = getClk();
        PCB_Array[Processid_run_now].State = Running;
        PCB_Array[Processid_run_now].itsLocationInArray = Processid_run_now;
        isRunning = true;
        ForkProcess(HPFReadyQueue->head->p.Runtime);
        pop(HPFReadyQueue);
    }
}

int Search_Min_in_pcb()
{

    int min = 20000;
    int MIN_index = -1;

    for (int i = 0; i < LastPlaceInArray; i++)
    {
        {
            if (PCB_Array[i].RemainingTime != 0 && PCB_Array[i].RemainingTime < min)
            {
                MIN_index = i;
                min = PCB_Array[i].RemainingTime;
            }
        }
    }
    return MIN_index;
}

void StopProcess(int idInPCB_Array)
{
    diff_finish_start = getClk();
    isRunning = false;

    if (PCB_Array[idInPCB_Array].Pid == -1)
    {
        printf("Errors in Stop id %d\n", idInPCB_Array);

        return;
    }
    int x = kill(PCB_Array[idInPCB_Array].Pid, SIGSTOP);
    if (x < 0)
    {
        printf("Error in Stoping ");
    }

    PCB_Array[idInPCB_Array].StopTime = getClk();
    PCB_Array[idInPCB_Array].State = Waitting;

    fprintf(schedulerLog, "At time %d process %d Stoped arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime);
}

void ContiueProcess(int idInPCB_Array)
{
    isRunning = true;

    totalwaittingtime += getClk() - diff_finish_start;

    kill(PCB_Array[idInPCB_Array].Pid, SIGCONT);
    kill(PCB_Array[idInPCB_Array].Pid, SIGUSR1);

    PCB_Array[idInPCB_Array].WaitingTime += getClk() - PCB_Array[idInPCB_Array].StopTime;
    PCB_Array[idInPCB_Array].State = Running;
    fprintf(schedulerLog, "At time %d process %d Resumed arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime);

    PCB_Array[idInPCB_Array].RemainingTime -= Quantum;
    if (PCB_Array[idInPCB_Array].RemainingTime <= 0)
    {
        lastone = true;
    }
    Processid_run_now = idInPCB_Array;
}

/* if i get Process :
- nothing runing {first / if Process end}
- something is running and the current running not the mini
- something is running and the current running is the mini
*/

void SRTN()
{
    processID_Min_RT = Search_Min_in_pcb();
    if (processID_Min_RT == -1)
    {

        return;
    }

    if (isRunning) // updating remaining time for the running Process
    {
        PCB_Array[Processid_run_now].RemainingTime = Rem_CurrentP - (getClk() - Start_processing_time);
        Start_processing_time = getClk();
        Rem_CurrentP = PCB_Array[Processid_run_now].RemainingTime;
    }
    /*
    - nothing is running
    - something is running and another Process arrived
    -
    */
    if ((PCB_Array[processID_Min_RT].RemainingTime < Rem_CurrentP && SRTNreadyQ->head != NULL && processID_Min_RT != Processid_run_now) || (!isRunning && SRTNreadyQ->head != NULL))
    {

        if (!isRunning) // nothing running
        {
            Rem_CurrentP = PCB_Array[processID_Min_RT].RemainingTime;
            Start_processing_time = getClk();

            if (PCB_Array[processID_Min_RT].State == Ready) // case: nothing is running and a process arrived
            {
                PCB_Array[processID_Min_RT].StartTime = getClk();
                PCB_Array[processID_Min_RT].State = Running;

                Processid_run_now = processID_Min_RT;

                ForkProcess(PCB_Array[processID_Min_RT].RemainingTime);
            }
            else if (PCB_Array[processID_Min_RT].State == Waitting) // case: nothing is running and a waiting Process become the shortest one
            {

                ContiueProcess(processID_Min_RT);
                PCB_Array[processID_Min_RT].State = Running;
                Processid_run_now = processID_Min_RT;
            }
        }
        else if (isRunning) // case: - something is running and another Process arrived with smaller Remaining time
        {

            PCB_Array[Processid_run_now].State = Waitting;

            PCB_Array[Processid_run_now].StopTime = getClk();

            StopProcess(Processid_run_now);

            if (PCB_Array[processID_Min_RT].State == Ready)
            {
                Processid_run_now = processID_Min_RT;
                Start_processing_time = getClk();
                Rem_CurrentP = PCB_Array[processID_Min_RT].RemainingTime;
                PCB_Array[processID_Min_RT].StartTime = getClk();
                PCB_Array[processID_Min_RT].State = Running;

                ForkProcess(PCB_Array[processID_Min_RT].RemainingTime);
            }
        }
    }
}

bool contextSwitch = false;

// 1-Check if there is a process in RRQueue
// 2-check on current if its null or not
// 3-if found check state

void RoundRobin()
{
    if (current != NULL)
    {
        int inexTorun = SearchInPCBArray(current->key.id);

        if (lastone)
        {
            return;
        }
        if (dummyQuantum == 0 || !isRunning)
        {
            dummyQuantum = Quantum;
            isRunning = true;

            if (PCB_Array[inexTorun].State == Ready)
            {
                // we need to fork
                startRoundRobin = getClk();
                PCB_Array[inexTorun].State = Running;
                Processid_run_now = inexTorun;
                PCB_Array[inexTorun].StartTime = getClk();
                ForkProcess(PCB_Array[inexTorun].P.Runtime);
            }
            else if (PCB_Array[inexTorun].State == Waitting)
            {
                Processid_run_now = inexTorun;
                startRoundRobin = getClk();
                ContiueProcess(inexTorun);
            }
        }
        else if (dummyQuantum > 0 && isRunning)
        {
            if (getClk() - startRoundRobin == 1)
            {
                dummyQuantum--;
                startRoundRobin = getClk();
            }
        }
        if (dummyQuantum == 0 && isRunning)
        {
            // printf("ContextSwitch\n");

            RevFrmGenetor();
            current = current->next;
            if (current == NULL)
            {
                current = RRreadyQ->head;
            }
            isRunning = false;
            if (PCB_Array[inexTorun].State == Ready)
            {
                return;
            }
            else
            {
                StopProcess(inexTorun);
            }
        }
    }
}

void writePref()
{
    schedulerPerf = fopen("scheduler.perf", "w");
    for (int i = 0; i < LastPlaceInArray; i++) // calculating  [avg_WeightedTurnAroundTime]  AND  [avg_waittingtime]
    {
        avg_WeightedTurnAroundTime += PCB_Array[i].WeightedTurnAroundTime;
        avg_waittingtime += PCB_Array[i].WaitingTime;
    }
    avg_WeightedTurnAroundTime = avg_WeightedTurnAroundTime / (LastPlaceInArray);
    avg_waittingtime = avg_waittingtime / (LastPlaceInArray);

    double X = 0;
    for (int i = 0; i < LastPlaceInArray; i++)
    {
        X += (PCB_Array[i].WeightedTurnAroundTime - avg_WeightedTurnAroundTime) * (PCB_Array[i].WeightedTurnAroundTime - avg_WeightedTurnAroundTime);
    }
    double VAR = 0;
    VAR = X / (LastPlaceInArray);

    /*Explanation: Suppose you have 4 processes (A, B, C and D).
    1. You calculate the WTA of A, B, C, and D (say WTA1, WTA2, WTA3, and WTA4)
    2. You then calculate the average of these four values (say X)
    3. You then subtract each value from this average, square it, and add them together. (WTA1-X)^2 + (WTA2-X)^2 + (WTA3-X)^2 + (WTA4-X)^2
    4. Divide what you computed in (3) by the number of processes which is 4. What you computed so far is the variance.
    5. Standard deviation is the square root of the variance.
    */

    double cpu_finish_time = getClk();
    double useful_Time = cpu_finish_time - totalwaittingtime;
    float CPU_utilization = (float)(useful_Time / cpu_finish_time) * 100;
    fprintf(schedulerPerf, "totalwaittingtime = %.2f\n", totalwaittingtime);
    fprintf(schedulerPerf, "cpu_finish_time = %.2f\n", cpu_finish_time);
    fprintf(schedulerPerf, "useful_Time = %.2f\n", useful_Time);
    fprintf(schedulerPerf, "CPU utilization = %.2f%%\n", CPU_utilization);

    fprintf(schedulerPerf, "avg_WeightedTurnAroundTime = %.2f\n", avg_WeightedTurnAroundTime);
    fprintf(schedulerPerf, "avg_waittingtime = %.2f\n", avg_waittingtime);
    double tempvar = sqrt(VAR);
    fprintf(schedulerPerf, "sqrt(VAR) = %.2lf%%\n", tempvar);

    fclose(schedulerPerf);
}

int main(int argc, char *argv[])
{
    schedulerLog = fopen("scheduler.log", "w");
    MemoryLog =fopen("memory.log", "w");
    Quantum = atoi((argv[1]));
    Algo = (*(argv[0])) - '0';
    signal(SIGUSR1, SignalHandlerGentorEnd);
    signal(SIGUSR2, SignalHandlerProcessesEnd);
    PCB_Array = CreatePCB_Array();
    buddySystem = initializeBuddySystem();
    HPFReadyQueue = CreatePriorityQueueOfProcesses();
    WaitingQueue = CreateLinkedListOfProcess();
    RRreadyQ = CreateQueueOfProcess();
    SRTNreadyQ = CreateQueueOfProcess();
    FCFSreadyQ = CreateQueueOfProcess();
    QueueKey = msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY, 0666 | IPC_CREAT);
    fprintf(schedulerLog, "#At time x process y state arr w total z remain y wait k\n");
    initClk();

    do
    {
        CheckWaitingQueue();
        RevFrmGenetor();
        if (Algo == 1)
        {
            HPF();
        }
        else if (Algo == 2)
        {
            SRTN();
        }
        else if (Algo == 3)
        {
            RoundRobin();
        }
    } while (isRunning || Generator || HPFReadyQueue->head != NULL || RRreadyQ->head != NULL || SRTNreadyQ->head != NULL || WaitingQueue->head != NULL);

    fclose(schedulerLog);
    fclose(MemoryLog);
    writePref();
    DestoryedPCB_Array(PCB_Array);
    destroyClk(true);
    return 0;
}