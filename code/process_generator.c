#include "headers.h"

void clearResources(int);
void CreateCLK();
void CreateScheduler(int choice,int Quntam);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    struct Queue * ProcessesQueue=CreateQueueOfProcess();
    int NumberOfProcesses=0; 
    ReadInputFile("processes.txt",ProcessesQueue,&NumberOfProcesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int Quntam=0;
    int choice=ChooseAlgorithm(&Quntam);
    // 3. Initiate and create the scheduler and clock processes.
    CreateCLK();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
   destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
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

void CreateScheduler(int choice,int Quntam){
    int Schedulerid=fork();

    if(Schedulerid==0){
         execl("scheduler.out",choice,Quntam,NULL);
    }
    if(Schedulerid<0){
        printf("Error while creating Scheduler");
        exit(-1);
    }
}