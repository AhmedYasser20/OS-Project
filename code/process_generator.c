#include "headers.h"

void clearResources();
void CreateCLK();
void CreateScheduler();
int Schedulerid;
int QueueKey;
char Quntam[3];
char choice;
int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    QueueKey =msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY,0666 | IPC_CREAT);
    // TODO Initialization
    // 1. Read the input files.
    struct Queue * ProcessesQueue=CreateQueueOfProcess();
    int NumberOfProcesses=0; 
    ReadInputFile("processes.txt",ProcessesQueue,&NumberOfProcesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    choice=ChooseAlgorithm(Quntam);
    // 3. Initiate and create the scheduler and clock processes.
    CreateCLK();
    CreateScheduler();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    int time;
    printf("NumberOfProcesses ==== %d \n",NumberOfProcesses);
    while(NumberOfProcesses){
        time=getClk();
        if(ProcessesQueue->head->key.ArriveTime<=time){
            struct MsgGeneratorScheduler temp;
            temp.type=1;
            temp.p=ProcessesQueue->head->key;
            int rev =msgsnd(QueueKey,&temp,sizeof(temp.p),!IPC_NOWAIT);
            if(rev<0){
                puts("Pushed \n");
            }
            else{
                printf("clk %d sent id %d\n",getClk(),temp.p.id);
            }
            NumberOfProcesses--;
            Pop(ProcessesQueue);
        }
    }
    kill(Schedulerid,SIGUSR1);
    int status;
    waitpid(Schedulerid,&status,0);
    // 6. Send the information to the scheduler at the appropriate time.
    clearResources();
    // 7. Clear clock resources
   destroyClk(true);
}

void clearResources()
{
    //TODO Clears all resources in case of interruption
    msgctl(QueueKey,IPC_RMID,(struct msqid_ds *)0);
}

void CreateCLK(){
    int clk_pid=fork();

    if(clk_pid==0){
        execl("clk.out"," ",NULL);
    }

    if(clk_pid<0){
        printf("Error while creating clock");
        exit(-1);
    }
}

void CreateScheduler(){
    Schedulerid=fork();

    if(Schedulerid==0){
         
         execl("scheduler.out",&choice,&Quntam,NULL);
    }
    if(Schedulerid<0){
        printf("Error while creating Scheduler");
        exit(-1);
    }
}