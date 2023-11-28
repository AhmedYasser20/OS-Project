#include "headers.h"
#include "DataStructures.h"


int main(int argc, char * argv[])
{
    initClk();
    
    //TODO implement the scheduler :)
    
    int numberOfProcess,currentTime;
    struct array arrProcessesId,arrArrivalTime,arrRunTime;  //array carries process need to be executed if the id =-1 then the process was here terminated
    makeArray(numberOfProcess,arrProcessesId);
    makeArray(numberOfProcess,arrArrivalTime);
    makeArray(numberOfProcess,arrRunTime);



    currentTime=getClk();



    //upon termination release the clock resources.
    
    destroyClk(true);
}
