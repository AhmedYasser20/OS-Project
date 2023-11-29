#include "headers.h"

void clearResources(int);
void CreateCLK();
void CreateScheduler(char choice,char Quntam);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    struct Queue * ProcessesQueue=CreateQueueOfProcess();
    int NumberOfProcesses=0; 
    ReadInputFile("processes.txt",ProcessesQueue,&NumberOfProcesses);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    char Quntam=0;
    char choice=ChooseAlgorithm(&Quntam);
    // 3. Initiate and create the scheduler and clock processes.
    CreateCLK();
    CreateScheduler(choice,Quntam);
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

void CreateScheduler(char choice,char Quntam){
    int Schedulerid=fork();

    if(Schedulerid==0){
         char  paras[]={choice,Quntam};
         execl("scheduler.out",paras,NULL);
    }
    if(Schedulerid<0){
        printf("Error while creating Scheduler");
        exit(-1);
    }
}