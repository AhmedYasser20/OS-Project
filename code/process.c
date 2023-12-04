#include "headers.h"

/* Modify this file as needed*/
int remainingtime;   // Time that current process will finsh using cpu

int main(int agrc, char * argv[])
{
    initClk();
    int start =getClk();
    remainingtime=getClk()+ *(argv[1]) ;
 

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > start)
    {
       start=getClk();
    }
    
    kill(getppid(),SIGUSR2);
    destroyClk(false);
    
    return 0;
}
