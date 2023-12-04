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
    Running,Waiting,Blocked
}StateOfProcess;

// Define your process stored in the pcb struct
typedef struct ProcessPCB 
{
    struct Process P;
    int itsLocationInArray;
    float WaitingTime;
    float ExecTime;
    float StartTime;
    float RemainingTime;
    float EndTime;
    StateOfProcess State;
}ProcessPCB ;


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