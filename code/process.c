#include "headers.h"

#define REDY 10
#define WIT 20
#define ENND 30

/* Modify this file as needed*/
int remQunatum; // Time that current process will finsh using cpu
int ALG = -1;
int status = 0;
int pid;
int currentTiime;
int tempstart;

/*
    Function name: SignalHandlerProcessesStop
    Description:    Signal Handler for Signal User 2 -
    Input: int
    Output: void

*/

void SignalHandlerProcessesStop(int sig)
{
    raise(SIGSTOP);
}

/*
    Function name: SignalHandlerProcessesCont
    Description:    Signal Handler for Signal User 2 -
    Input: int
    Output: void

*/

void SignalHandlerProcessesCont(int sig)
{
    raise(SIGCONT);
    signal(SIGUSR1, SignalHandlerProcessesCont);
}


int main(int agrc, char *argv[])
{
    signal(SIGUSR1, SignalHandlerProcessesCont);
    signal(SIGUSR2, SignalHandlerProcessesStop);

    initClk();
    int start = getClk();
    tempstart = start;
    currentTiime = getClk();
    pid = getpid();

    remQunatum = *(argv[1]);
   

    // HPF (revived rem time) finsh when  rem =0
    // SRT  (revived rem time) finsh when  (rem =0 or (interputed and process sleep ) )
    // RR (Quantum ) finsh when  (Q =0 and sleep or (rem = 0 and end) )
    
    do
    {
        currentTiime = getClk();
        if (tempstart != currentTiime) // check if there is clock happened
        {
            if (currentTiime - tempstart == 1)
            {
                remQunatum--;
                tempstart = getClk();
            }
        }
        currentTiime = getClk(); // the following if condition corrects if an iteruption occured at this line
        if (currentTiime - tempstart > 1)
        {
            currentTiime = getClk();
            tempstart = currentTiime;
        }

    } while (remQunatum != 0); // if status != ENND

    kill(getppid(), SIGUSR2);
    exit(0);

    
    destroyClk(false);

    return 0;
}
