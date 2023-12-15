#include "headers.h"

FILE *schedulerLog;
FILE *schedulerPerf;
double diff_finish_start=0;
double totalwaittingtime=0;
double avg_WeightedTurnAroundTime;
double avg_waittingtime;


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
struct Queue *FCFSreadyQ;
bool ForceRR = false;
int dummyQuantum = 3;
bool lastone = false;
struct QueueProcessNode *current;
int startRoundRobin = 0;

// 1- Fix Clock
// 2- Sleep Processes
// 3- End Processes =>
// 4- Abo ismail
// 5- Output

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

void RevFrmGenetor()
{
    struct MsgGeneratorScheduler temp;
    int Rev = msgrcv(QueueKey, &temp, sizeof(temp.p), 0, IPC_NOWAIT);

    if (Rev != -1)
    {
        if (Algo == 1) // HPF
        {
           // printf("clk %d Pushed in HPFQueue %d\n", getClk(), temp.p.id);
            push(HPFReadyQueue, temp.p);
            SetPCB_Array(temp.p);
        } // SRTN
        else if (Algo == 2)
        {
           // printf("clk %d Pushed in STRNQeue %d\n", getClk(), temp.p.id);
            Push(SRTNreadyQ, temp.p);
            SetPCB_Array(temp.p);
        }
        else if (Algo == 3)

        {
           // printf(" Pushed in RRQeue %d\n", temp.p.id);
            if (RRreadyQ->head == NULL)
            {
                Push(RRreadyQ, temp.p);
                current = RRreadyQ->head;
            }
            else
            {
                Push(RRreadyQ, temp.p);
            }
            SetPCB_Array(temp.p);
        }
    }
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

void PrintPCBArrayWithID(int id)
{
    int i = id;

    printf("process id %d Arrive Time %d Runtime %d Priority %d   REMTIME %d \n", PCB_Array[i].P.id, PCB_Array[i].P.ArriveTime, PCB_Array[i].P.Runtime, PCB_Array[i].P.Priority, PCB_Array[i].RemainingTime);
    printf("start time =%d  Exec Time =%d  End time =%d  id=%d \n", PCB_Array[i].StartTime, PCB_Array[i].ExecTime, PCB_Array[i].EndTime, PCB_Array[i].P.id);
    printf("ITSLoc %d , PID %d , STATE %d\n", PCB_Array[i].itsLocationInArray, PCB_Array[i].Pid, PCB_Array[i].State);
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
        pop_id(SRTNreadyQ, PCB_Array[Processid_run_now].P.id);
    if (Algo == 3)
    {
        ForceRR = true;
        dummyQuantum=0;
        while(startRoundRobin==getClk());
      //  printf("poped id %d %d\n", current->key.id, PCB_Array[Processid_run_now].P.id);
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
    //printf("At time %d process %d finished arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime);
     fprintf(schedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d\n",getClk(), PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime );
    isRunning = false;
    diff_finish_start = getClk();
    printf("DIFFFFF FINSH  %0.2f \n",diff_finish_start);
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
     
    totalwaittingtime += getClk() - diff_finish_start ;
    printf("TOTALL WAITINGG TIME %0.2f \n",totalwaittingtime);

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

    PCB_Array[Processid_run_now].RemainingTime = PCB_Array[Processid_run_now].RemainingTime - Quantum;
    if (PCB_Array[Processid_run_now].RemainingTime <= 0)
    {
        printf("LASSSSST %d \n", getClk());
        lastone = true;
    }
  
   // printf("At time %d process %d starting arr %d total %d remain %d wait %d\n", getClk(),
          // PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime);
     fprintf(schedulerLog, "#At time %d process %d starting arr %d total %d remain %d wait %d\n",  getClk(), PCB_Array[Processid_run_now].P.id, PCB_Array[Processid_run_now].P.ArriveTime, PCB_Array[Processid_run_now].P.Runtime, PCB_Array[Processid_run_now].RemainingTime, PCB_Array[Processid_run_now].WaitingTime); 
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

      //  printf("PCB_Array[%d].WaitingTime before = %d\n",PCB_Array[Processid_run_now].P.id ,PCB_Array[Processid_run_now].WaitingTime);
        PCB_Array[Processid_run_now].WaitingTime += ( getClk() - PCB_Array[Processid_run_now].P.ArriveTime );
       // printf("PCB_Array[%d].WaitingTime after = %d\n",PCB_Array[Processid_run_now].P.id ,PCB_Array[Processid_run_now].WaitingTime);

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
int processID_Now;
void SRTN()
{
    processID_Min_RT = Search_Min_in_pcb();
    if (processID_Min_RT == -1)
    {

        return;
    }
    processID_Now = processID_Min_RT;
    if (isRunning)
    {
        PCB_Array[Processid_run_now].RemainingTime = Rem_CurrentP - (getClk() - Start_processing_time);
        Start_processing_time = getClk();
        Rem_CurrentP = PCB_Array[Processid_run_now].RemainingTime;
    }

    if ((PCB_Array[processID_Min_RT].RemainingTime < Rem_CurrentP && SRTNreadyQ->head != NULL && processID_Min_RT != Processid_run_now) || (!isRunning && SRTNreadyQ->head != NULL))
    {

        if (!isRunning) // nothing running
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

              //  printf("PCB_Array[%d].WaitingTime before = %d\n",PCB_Array[processID_Now].P.id ,PCB_Array[processID_Now].WaitingTime);
                PCB_Array[processID_Now].WaitingTime += ( getClk() - PCB_Array[processID_Now].P.ArriveTime );
               // printf("PCB_Array[%d].WaitingTime after = %d\n",PCB_Array[processID_Now].P.id ,PCB_Array[processID_Now].WaitingTime);

                ForkProcess(PCB_Array[processID_Now].RemainingTime);
            }
            else if (PCB_Array[processID_Min_RT].State == Waitting) // case: a process RemT is the minimum
            {
               // printf("ContextSwitch2  \n");
                isRunning = true;

              //  printf("PCB_Array[%d].WaitingTime before = %d\n",PCB_Array[processID_Min_RT].P.id ,PCB_Array[processID_Min_RT].WaitingTime);
                PCB_Array[processID_Min_RT].WaitingTime +=( getClk() - PCB_Array[processID_Min_RT].StopTime );
              //  printf("PCB_Array[%d].WaitingTime after = %d\n",PCB_Array[processID_Min_RT].P.id ,PCB_Array[processID_Min_RT].WaitingTime);

                kill(PCB_Array[processID_Min_RT].Pid, SIGCONT); // SIG CONT
                PCB_Array[processID_Now].State = Running;
                Processid_run_now = processID_Min_RT;
            }
        }
        else if (isRunning) // case: the running process is not the minimum
        {

           // printf("ContextSwitch1 \n");
            PCB_Array[Processid_run_now].State = Waitting;
            
            PCB_Array[Processid_run_now].StopTime = getClk();
           // printf("PCB_Array[%d].stoptime = %d\n",PCB_Array[Processid_run_now].P.id ,PCB_Array[Processid_run_now].StopTime);

            kill(PCB_Array[Processid_run_now].Pid, SIGSTOP); // SIG stop
            isRunning = true;
            if (PCB_Array[processID_Now].State == Ready)
            {
                Processid_run_now = processID_Min_RT;
                Start_processing_time = getClk();
                Rem_CurrentP = PCB_Array[processID_Min_RT].RemainingTime;
                PCB_Array[processID_Now].StartTime = getClk();
                PCB_Array[processID_Now].State = Running;
                isRunning = true;

                
              //  printf("PCB_Array[%d].WaitingTime before = %d\n",PCB_Array[processID_Now].P.id ,PCB_Array[processID_Now].WaitingTime);
                PCB_Array[processID_Now].WaitingTime += ( getClk() - PCB_Array[processID_Now].P.ArriveTime );
             //   printf("PCB_Array[%d].WaitingTime after = %d\n",PCB_Array[processID_Now].P.id ,PCB_Array[processID_Now].WaitingTime);
                // PCB_Array[processID_Now].WaitingTime=PCB_Array[processID_Now].StartTime-PCB_Array[processID_Now].P.ArriveTime;
                ForkProcess(PCB_Array[processID_Now].RemainingTime);
            }
        }
    }
}

void StopProcess(int idInPCB_Array)
{
    diff_finish_start = getClk();
    printf("DIFFFFF FINSH  %0.2f \n",diff_finish_start);

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

   // printf("At time %d process %d Stoped arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime);
    fprintf(schedulerLog, "At time %d process %d Stoped arr %d total %d remain %d wait %d\n" ,getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime );
}

void ContiueProcess(int idInPCB_Array)
{
    totalwaittingtime += getClk() - diff_finish_start ;
    printf("TOTALL WAITINGG TIME %0.2f \n",totalwaittingtime);

    int x = kill(PCB_Array[idInPCB_Array].Pid, SIGCONT);
    printf("x: %d idInPCB_Array %d\n",x,idInPCB_Array);

    PCB_Array[idInPCB_Array].WaitingTime += getClk() - PCB_Array[idInPCB_Array].StopTime;
    PCB_Array[idInPCB_Array].State = Running;
    PCB_Array[idInPCB_Array].RemainingTime -= Quantum;
    if (PCB_Array[idInPCB_Array].RemainingTime <= 0)
    {
        lastone = true;
    }
    Processid_run_now = idInPCB_Array;
    printf("At time %d process %d Resumed arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime);
    fprintf(schedulerLog, "At time %d process %d Resumed arr %d total %d remain %d wait %d\n", getClk(), PCB_Array[idInPCB_Array].P.id, PCB_Array[idInPCB_Array].P.ArriveTime, PCB_Array[idInPCB_Array].P.Runtime, PCB_Array[idInPCB_Array].RemainingTime, PCB_Array[idInPCB_Array].WaitingTime);
}

int testt = 1;
int tempForRR;
int temp22;
bool contextSwitch = false;
int ContextSwitchToo;

void RoundRobin1()
{

    if (lastone)
    {

        return;
    }
    if (((startRoundRobin + Quantum == getClk() && isRunning) || ForceRR) && current != NULL)
    {
        startRoundRobin = getClk();
        int temp2 = SearchInPCBArray(current->key.id);
        if (PCB_Array[temp2].State == Waitting)
        {
            isRunning = true;
            ContiueProcess(temp2);
        }
        else if (PCB_Array[temp2].State == Running)
        {
            RevFrmGenetor();
            current = current->next;
            if (current == NULL)
            {
                current = RRreadyQ->head;
            }
            StopProcess(temp2);

            ContextSwitchToo = SearchInPCBArray(current->key.id);
            if (PCB_Array[ContextSwitchToo].State == Ready)
            {
                printf("Forking %d \n", current->key.id);
                isRunning = true;
                PCB_Array[ContextSwitchToo].State = Running;
                Processid_run_now = ContextSwitchToo;
                ForkProcess(PCB_Array[ContextSwitchToo].P.Runtime);
            }
            else
            {
                isRunning = true;
                Processid_run_now = ContextSwitchToo;
                ContiueProcess(ContextSwitchToo);
            }
        }
        else if (PCB_Array[temp2].State == Ready)
        {
            printf("Forking %d \n", current->key.id);
            PCB_Array[temp2].State = Running;
            Processid_run_now = temp2;
            ForkProcess(PCB_Array[temp2].P.Runtime);
        }
        isRunning = true;
        ForceRR = false;
    }
    else if (!isRunning)
    {

        if (current != NULL)
        {
            startRoundRobin = getClk();
            printf("Clk = %d\n", getClk());
            tempForRR = SearchInPCBArray(current->key.id);
            if (PCB_Array[tempForRR].State == Ready)
            {
                isRunning = true;
                PCB_Array[tempForRR].State = Running;
                Processid_run_now = tempForRR;
                ForkProcess(PCB_Array[tempForRR].P.Runtime);
            }
            else if (PCB_Array[tempForRR].State == Waitting)
            {
                Processid_run_now = tempForRR;
                ContiueProcess(tempForRR);
            }
        }
    }
}

// 1-Check if there is a process in RRQueue
// 2-check on current if its null or not
// 3-if found check state

void RoundRobin()
{
    if (current != NULL)
    {
        int inexTorun = SearchInPCBArray(current->key.id);
        
        if(lastone)
        {
            return;
        }
        if (dummyQuantum == 0 || !isRunning)
        {
            dummyQuantum=Quantum;
            isRunning=true;

            if (PCB_Array[inexTorun].State == Ready)
            {
                //we need to fork
                startRoundRobin=getClk();
                PCB_Array[inexTorun].State = Running;
                Processid_run_now = inexTorun;
                PCB_Array[inexTorun].StartTime=getClk();
                ForkProcess(PCB_Array[inexTorun].P.Runtime);

            }
            else if (PCB_Array[inexTorun].State == Waitting)
            {
                Processid_run_now = inexTorun;
                startRoundRobin=getClk();
                ContiueProcess(inexTorun);
            }
        }
        else if (dummyQuantum > 0 &&isRunning)
        {
            if(getClk()-startRoundRobin ==1 )
            {
            dummyQuantum--;
            startRoundRobin=getClk();
            }
        }
        if(dummyQuantum==0  && isRunning )
        {
            printf("ContextSwitch\n");
            RevFrmGenetor();
            current=current->next;
            if(current==NULL)
            {
                current=RRreadyQ->head;
            }
            isRunning=false;
            if(PCB_Array[inexTorun].State ==Ready)
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

void writePref(){
    schedulerPerf = fopen("scheduler.perf", "w");
    for(int i=0 ; i<LastPlaceInArray  ; i++) // calculating  [avg_WeightedTurnAroundTime]  AND  [avg_waittingtime]
    {
        avg_WeightedTurnAroundTime += PCB_Array[i].WeightedTurnAroundTime;
        avg_waittingtime           += PCB_Array[i].WaitingTime;
    }
    avg_WeightedTurnAroundTime = avg_WeightedTurnAroundTime / (LastPlaceInArray);
    avg_waittingtime           = avg_waittingtime  / (LastPlaceInArray);

    //(-XWTA1)^2
    int X=0;
    for(int i=0 ; i<LastPlaceInArray  ; i++){
        X += (PCB_Array[i].WeightedTurnAroundTime - avg_WeightedTurnAroundTime)* (PCB_Array[i].WeightedTurnAroundTime - avg_WeightedTurnAroundTime);
    }
    float VAR=0;
    VAR = X / (LastPlaceInArray-1);

    /*Explanation: Suppose you have 4 processes (A, B, C and D).
    1. You calculate the WTA of A, B, C, and D (say WTA1, WTA2, WTA3, and WTA4)
    2. You then calculate the average of these four values (say X)
    3. You then subtract each value from this average, square it, and add them together. (WTA1-X)^2 + (WTA2-X)^2 + (WTA3-X)^2 + (WTA4-X)^2
    4. Divide what you computed in (3) by the number of processes which is 4. What you computed so far is the variance. 
    5. Standard deviation is the square root of the variance.
*/

    double cpu_finish_time = getClk();
    double useful_Time = cpu_finish_time - totalwaittingtime;
    float CPU_utilization = (float) (useful_Time / cpu_finish_time) *100;
    fprintf(schedulerPerf, "totalwaittingtime = %.2f\n",totalwaittingtime);
    fprintf(schedulerPerf, "cpu_finish_time = %.2f\n",cpu_finish_time);
    fprintf(schedulerPerf, "useful_Time = %.2f\n",useful_Time);
    fprintf(schedulerPerf, "CPU utilization = %.2f%%\n",CPU_utilization);
    
    
    fprintf(schedulerPerf, "avg_WeightedTurnAroundTime = %.2f\n",avg_WeightedTurnAroundTime);
    fprintf(schedulerPerf, "avg_waittingtime = %.2f\n",avg_waittingtime);
    //fprintf(schedulerPerf, "sqrt(VAR) = %.2f%%\n",sqrt(VAR) );

    fclose(schedulerPerf);
}


int main(int argc, char *argv[])
{
    schedulerLog  = fopen("scheduler.log" , "w");
   

    Quantum = (*(argv[1])) - '0';
    Algo = (*(argv[0])) - '0';
    signal(SIGUSR1, SignalHandlerGentorEnd);
    signal(SIGUSR2, SignalHandlerProcessesEnd);
    PCB_Array = CreatePCB_Array();

    HPFReadyQueue = CreatePriorityQueueOfProcesses();
    RRreadyQ = CreateQueueOfProcess();
    SRTNreadyQ = CreateQueueOfProcess();
    FCFSreadyQ = CreateQueueOfProcess();
    QueueKey = msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY, 0666 | IPC_CREAT);
     int x=fprintf(schedulerLog, "#At time x process y state arr w total z remain y wait k\n");
     printf("XXXXXXXXXXXXXXXXXXX %d \n",x);
    initClk();

    do
    {

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
    } while (isRunning || Generator || HPFReadyQueue->head != NULL || RRreadyQ->head != NULL || SRTNreadyQ->head != NULL);
    // commment
    puts("Normal");
    fclose(schedulerLog);
    writePref();
    DestoryedPCB_Array(PCB_Array);
    destroyClk(true);
    return 0;
}