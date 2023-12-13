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
        printf(" clk %d pid %d RemQ %d\n",getClk(),getpid(),remQunatum);
       // prev=getClk();
        // currentTiime = getClk();
        // if (tempstart != currentTiime) // check if there is clock happened
        // {
        //     if (currentTiime - tempstart == 1)
        //     {
        //         remQunatum--;
        //         printf(" clk %d pid %d RemQ %d\n",getClk(),getpid(),remQunatum);
        //         tempstart = getClk();
        //     }
        // }
        // currentTiime = getClk(); // the following if condition corrects if an iteruption occured at this line
        // if (currentTiime - tempstart > 1)
        // {
        //     currentTiime = getClk();
        //     tempstart = currentTiime;
        // }
        while(prev==getClk());
        prev=getClk();
        remQunatum--;
       
    } while (remQunatum >0); // if status != ENND

    kill(getppid(), SIGUSR2);
    destroyClk(false);
    exit(0);

    
   

    return 0;
}
