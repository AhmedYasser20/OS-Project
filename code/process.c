#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    int start =getClk();
    printf("Process here start %d\n",start);

    printf("run Time= %d\n",*(argv[1]) );

    remainingtime=getClk()+ *(argv[1]) ;
    printf("rem Time= %d\n",remainingtime );

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
