#ifndef DATA_STRUCTURES_HEADER
#define DATA_STRUCTURES_HEADER


struct Process
{
    int id;         //ID for Each Process Unique
    int ArriveTime; //Arrive time of process (when i arrive to cpu or OS see it) 
    int Runtime;    //run time  (the time that OS give the process)
    int Priority;   //Priority of Processes
};

typedef enum {
    RUNNING,WAITING,ENDED,READY
}StateOfProcess;

// Define your process stored in the pcb struct
typedef struct ProcessPCB 
{
    struct Process P;
    int itsLocationInArray;
    int WaitingTime;
    int ExecTime;
    int StartTime;
    int RemainingTime;
    int EndTime;
    int pid;
    StateOfProcess State;
}ProcessPCB ;

typedef enum {
    START,WAIT,END
}Action;

typedef struct MessageBetweenProcessAndScheduler{
    long type;
    int remainingtime;
    int ExceTime;
    int Qutam;
    Action Order;
}MessageBetweenProcessAndScheduler;


struct array
{
    int size;
    int *ptr;
    
};

struct Log
{
    int time;
    int Pid;
    char* state;
    int ArriveT;
    int TotalT;
    int RemainingT;
    int ResponseT;
    int TA;
    int WAT;
    
    int RunningT;
    int waitingT;
    
};



void makeArray(int N,struct array arr);
#endif