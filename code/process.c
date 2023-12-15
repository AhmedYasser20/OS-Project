#include "headers.h"



/* Modify this file as needed*/
int remQunatum; // Time that current process will finsh using cpu
int ALG = -1;
int status = 0;
int pid;
int currentTiime;
int tempstart;







int main(int agrc, char *argv[])
{
    remQunatum = *(argv[1]);
   
    initClk();
    int start = getClk();
    tempstart = start;
    currentTiime = getClk();
    pid = getpid();

    
   

    // HPF (revived rem time) finsh when  rem =0
    // SRT  (revived rem time) finsh when  (rem =0 or (interputed and process sleep ) )
    // RR (Quantum ) finsh when  (Q =0 and sleep or (rem = 0 and end) )
    int prev=getClk();
    do
    {

        while(prev==getClk());
        if(getClk() - prev >1)
        {
            prev=getClk();
        }
        else
        {
        prev=getClk();
        remQunatum--;
        }
               printf(" clk %d pid %d RemQ %d\n",getClk(),getpid(),remQunatum);
    } while (remQunatum >0); // if status != ENND

    kill(getppid(), SIGUSR2);
    destroyClk(false);
    exit(0);

    
   

    return 0;
}
