#include "headers.h"

/* Modify this file as needed*/
int remainingtime;   // Time that current process will finsh using cpu

int main(int agrc, char * argv[])
{
    initClk();
    int start =getClk();
    printf("Process here start %d\n",start);

    printf("Quantum Time= %d\n",*(argv[1]) );

    

    remainingtime=getClk()+ *(argv[1]) ;
    printf("rem Time so cpu be free= %d\n",remainingtime );

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > start)
    {
       start=getClk();
    }
    printf("Process here end %d\n",start);
    
    kill(getppid(),SIGUSR2);
    destroyClk(false);
    
    return 0;
}
