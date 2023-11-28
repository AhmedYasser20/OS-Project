#ifndef DATA_STRUCTURES_HEADER
#define DATA_STRUCTURES_HEADER

struct Process
{
    int id;         //ID for Each Process Unique
    int ArriveTime; //Arrive time of process (when i arrive to cpu or OS see it) 
    int Runtime;    //run time  (the time that OS give the process)
    int Priority;   //Priority of Processes
};



#endif