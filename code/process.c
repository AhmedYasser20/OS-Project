#include "headers.h"

/* Modify this file as needed*/
int remQunatum; // Time that current process will finsh using cpu
int currentTiime; // is used to check if a clock cycle passed
int prev;         // is used to check if a clock cycle passed

void SignalHandlerProcessesCONT(int sig)    
// this handler "CONT" is used to get the prev and current time when the schecduler send to continue 
{
    currentTiime = getClk();  
    prev = getClk();
}

int main(int agrc, char *argv[])
{
    remQunatum = *(argv[1]);

    initClk();
    signal(SIGUSR1, SignalHandlerProcessesCONT);

    currentTiime = getClk();
    prev = getClk();

    // HPF (revived rem time) finsh when  rem =0
    // SRT  (revived rem time) finsh when  (rem =0 or (interputed and process sleep ) )
    // RR (Quantum ) finsh when  (Q =0 and sleep or (rem = 0 and end) )
    do
    {

        if (currentTiime - prev >= 1)
        {
            prev = getClk();
            remQunatum--;
        }
        currentTiime = getClk();
        
    } while (remQunatum > 0); // if status != ENND

    kill(getppid(), SIGUSR2);  // send SIGUSR2 to notify the scheduler that i have finished my runtime
    destroyClk(false);
    exit(0);

    return 0;
}