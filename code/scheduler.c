#include "headers.h"

int QueueKey;                                   // Key of Queue between Scheduler and Processes Generator
struct PriorityQueueOfProcesses *HPFReadyQueue; // Inital Queue using to store Processes That read Form Generator
bool Generator = true;                          // Boolen used to Notify if Generator Finshed sending process or not
bool isRunning = false;                         // Boolen used to Notify Schedular if Process is finshed or not
ProcessPCB *PCB_Array;
int LastPlaceInArray = 0;
int Processid_run_now;
int processID_Min_RT;
int Processid_run_now = -1; // Processid_run_now
int Rem_CurrentP = 0;
int Start_processing_time;
int pidnow;
int Quantum;
int Algo;
struct Queue *RRreadyQ;
struct Queue *SRTNreadyQ;
bool ForceRR = false;
// 1- Fix Clock
// 2- Sleep Processes
// 3- End Processes =>
// 4- Abo ismail
// 5- Output

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

/*
    Function name: SignalHandlerProcessesEnd
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

    if (Algo == 2)
        PopID(SRTNreadyQ, PCB_Array[Processid_run_now].P.id);
    if (Algo == 3)
    {
        ForceRR = true;
        printf("poped id %d %d\n", RRreadyQ->head->key.id, PCB_Array[Processid_run_now].P.id);
        Pop(RRreadyQ);
    }
    printf("At time %d process %d finished arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime);
    isRunning = false;
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

    int processid = fork();

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
    PCB_Array[Processid_run_now].Pid = processid;
    printf("At time %d process %d starting arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime);
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
    temp.StopTime = 0;

    PCB_Array[LastPlaceInArray] = temp;
    LastPlaceInArray++;
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

void PrintPCBArray()
{
    for (int i = 0; i < LastPlaceInArray; i++)
    {
        printf("process id %d Arrive Time %d Runtime %d Priority %d   REMTIME %d \n", PCB_Array[i].P.id, PCB_Array[i].P.ArriveTime, PCB_Array[i].P.Runtime, PCB_Array[i].P.Priority, PCB_Array[i].RemainingTime);
        printf("start time =%d  Exec Time =%d  End time =%d  id=%d \n", PCB_Array[i].StartTime, PCB_Array[i].ExecTime, PCB_Array[i].EndTime, PCB_Array[i].P.id);
        printf("ITSLoc %d , PID %d , STATE %d\n", PCB_Array[i].itsLocationInArray, PCB_Array[i].Pid, PCB_Array[i].State);
    }
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

/* if i get P :
- nothing runing {first / if P end}
- something is running and the current running not the mini

- something is running and the current running is the mini

*/
//
void SRTN()
{
    processID_Min_RT = Search_Min_in_pcb();
    if (processID_Min_RT == -1)
    {
        return;
    }
    Processid_run_now = processID_Min_RT;

    if (isRunning)
    {
        PCB_Array[Processid_run_now].RemainingTime = Rem_CurrentP - (getClk() - Start_processing_time);
        Rem_CurrentP = PCB_Array[Processid_run_now].RemainingTime;
    }

    if ((PCB_Array[processID_Min_RT].RemainingTime < Rem_CurrentP && SRTNreadyQ->head != NULL && processID_Min_RT != Processid_run_now) || (!isRunning && SRTNreadyQ->head != NULL))
    {
        if (!isRunning) // nothing running
        {

            Rem_CurrentP = PCB_Array[processID_Min_RT].RemainingTime;
            Start_processing_time = getClk();

            if (PCB_Array[Processid_run_now].State == Ready)
            {
                Processid_run_now = processID_Min_RT;
                PCB_Array[Processid_run_now].StartTime = getClk();
                PCB_Array[Processid_run_now].State = Running;
                isRunning = true;
                Processid_run_now = processID_Min_RT;
                ForkProcess(PCB_Array[Processid_run_now].RemainingTime);
            }
            else if (PCB_Array[processID_Min_RT].State == Waitting) // case: a process RemT is the minimum
            {
                isRunning = true;
                kill(PCB_Array[processID_Min_RT].Pid, SIGCONT); // SIG CONT
                PCB_Array[Processid_run_now].State = Running;
                Processid_run_now = processID_Min_RT;
            }
        }
        else if (isRunning) // case: the running process is not the minimum
        {
            PCB_Array[Processid_run_now].State = Waitting;
            int x = kill(PCB_Array[Processid_run_now].Pid, SIGSTOP); // SIG stop
            isRunning = true;
            if (PCB_Array[Processid_run_now].State == Ready)
            {
                Processid_run_now = processID_Min_RT;
                Start_processing_time = getClk();
                Rem_CurrentP = PCB_Array[processID_Min_RT].RemainingTime;
                PCB_Array[Processid_run_now].StartTime = getClk();
                PCB_Array[Processid_run_now].State = Running;
                isRunning = true;
                ForkProcess(PCB_Array[Processid_run_now].RemainingTime);
            }
        }
    }
}

void StopProcess(int idInPCB_Array)
{

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

    printf("At time %d process %d Stoped arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime);
}

void ContiueProcess(int idInPCB_Array)
{
    if (PCB_Array[idInPCB_Array].Pid == -1)
    {
        printf("Errors in contine id %d\n", idInPCB_Array);

        return;
    }

    int x = kill(PCB_Array[idInPCB_Array].Pid, SIGCONT);
    if (x < 0)
    {
        printf("Error in Resuming ");
    }

    PCB_Array[idInPCB_Array].WaitingTime += getClk() - PCB_Array[idInPCB_Array].StopTime;
    PCB_Array[idInPCB_Array].State = Running;

    printf("At time %d process %d Resumed arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime);
}

int testt = 1;
int startRoundRobin = 0;
int tempForRR;
void RoundRobin()
{
    //
    // printf("at clk %d  starttime %d  isrunning %d \n",getClk(),startRoundRobin+Quantum,isRunning);
    if (((startRoundRobin + Quantum == getClk() && isRunning) || ForceRR) && RRreadyQ->head != NULL)
    {
        startRoundRobin = getClk();
        printf("Clk = %d\n", getClk());
        int temp2 = SearchInPCBArray(RRreadyQ->head->key.id);
        printf("id %d State %d\n", temp2, PCB_Array[temp2].State);
        if (PCB_Array[temp2].State == Waitting)
        {
            ContiueProcess(temp2);
        }
        else if (PCB_Array[temp2].State == Running)
        {
            StopProcess(temp2);
            printf("pushed %d\n", RRreadyQ->head->key.id);
            Push(RRreadyQ, RRreadyQ->head->key);
            Pop(RRreadyQ);
            int temp = SearchInPCBArray(RRreadyQ->head->key.id);
            
            
            int temp22 = getClk();
            while (temp22 == getClk());    
            startRoundRobin = getClk();
            
            
            if (PCB_Array[temp].State == Ready)
            {
                printf("Forking %d \n", RRreadyQ->head->key.id);
                isRunning = true;
                PCB_Array[temp].State = Running;
                Processid_run_now = temp;
                ForkProcess(PCB_Array[temp].P.Runtime);
            }
            else
            {

                ContiueProcess(temp);
                Processid_run_now = temp;
            }
        }
        else if (PCB_Array[temp2].State == Ready)
        {
            printf("Forking %d \n", RRreadyQ->head->key.id);
            PCB_Array[temp2].State = Running;
            Processid_run_now = temp2;
            ForkProcess(PCB_Array[temp2].P.Runtime);
        }
        isRunning = true;
        ForceRR = false;
    }
    else if (!isRunning)
    {

        if (RRreadyQ->head != NULL)
        {
            startRoundRobin = getClk();
            printf("Clk = %d\n", getClk());
            tempForRR = SearchInPCBArray(RRreadyQ->head->key.id);
            if (PCB_Array[tempForRR].State == Ready)
            {
                isRunning = true;
                PCB_Array[tempForRR].State = Running;
                Processid_run_now = tempForRR;
                ForkProcess(PCB_Array[tempForRR].P.Runtime);
            }
            else if (PCB_Array[tempForRR].State == Waitting)
            {
                ContiueProcess(tempForRR);
                Processid_run_now = tempForRR;
            }
        }
    }
}
int main(int argc, char *argv[])
{
    Quantum = (*(argv[1])) - '0';
    Algo = (*(argv[0])) - '0';
    signal(SIGUSR1, SignalHandlerGentorEnd);
    signal(SIGUSR2, SignalHandlerProcessesEnd);
    PCB_Array = CreatePCB_Array();

    HPFReadyQueue = CreatePriorityQueueOfProcesses();
    RRreadyQ = CreateQueueOfProcess();
    SRTNreadyQ = CreateQueueOfProcess();

    QueueKey = msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY, 0666 | IPC_CREAT);
    printf("#At time x process y state arr w total z remain y wait k\n");
    initClk();

    do
    {
        struct MsgGeneratorScheduler temp;
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

        int Rev = msgrcv(QueueKey, &temp, sizeof(temp.p), 0, IPC_NOWAIT);
        if (Rev != -1)
        {
            if (Algo == 1) // HPF
            {
                push(HPFReadyQueue, temp.p);
                SetPCB_Array(HPFReadyQueue->head->p);
            } // SRTN
            else if (Algo == 2)
            {
                Push(SRTNreadyQ, temp.p);
                SetPCB_Array(temp.p);
            }
            else
            {
                printf("Pushed in RRQeue %d\n", temp.p.id);
                Push(RRreadyQ, temp.p);
                SetPCB_Array(temp.p);
            }
        }

    } while (isRunning || Generator || HPFReadyQueue->head != NULL || RRreadyQ->head != NULL || SRTNreadyQ->head != NULL);
    DestoryedPCB_Array(PCB_Array);
    destroyClk(true);
    return 0;
}