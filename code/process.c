#include "headers.h"




int I_getWait = 0;
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
    printf("New process with id  is now being interputed  \n" );
    I_getWait = 1; // if 0 your waiting
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
    printf("aloooooooooooo  \n" );

    I_getWait = 0;   // if 0 your running
    raise(SIGCONT);
    signal(SIGUSR1, SignalHandlerProcessesCont);
}




/* Modify this file as needed*/
int remQunatum; // Time that current process will finsh using cpu
int ALG = -1;
#define REDY 10
#define WIT  20
#define ENND 30
int status = 0;
int main(int agrc, char *argv[])
{
    //printf("hello world\n" );

    signal(SIGUSR1, SignalHandlerProcessesCont);
    signal(SIGUSR2, SignalHandlerProcessesStop);
    
    initClk();
    int start = getClk();
     tempstart=start;
     currentTiime=getClk();
    int Quantum;    // Time slot of RR 
     pid = getpid();
    if (agrc ==3)
    {
        // then you are round robin and you have recived Quatum and remaining time of the process
         remQunatum =  * (argv[1] );    // HPF  ,SJT   actuall rem time for the process to be terminated
         //RR  to current stop if rem 
         Quantum=* (argv[2] );     
    }
    else 
    {
         // then you are HPF or srtn and you have recived  remaining time of the process
      //   printf("yasser\n");
          remQunatum =  * (argv[1]);
          Quantum=1;

    }
   

    

   
    

     //MessageBetweenProcessAndScheduler temp;
    // int QueueKey = msgget(MSG_QUEUE_FROM_SCHEDULER_TO_PROCESS_KEY, 0666 | IPC_CREAT);  // sch -> P


    // msgrcv(QueueKey, &temp, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), pid, IPC_NOWAIT);

        // HPF (revived rem time) finsh when  rem =0
        //SRT  (revived rem time) finsh when  (rem =0 or (interputed and process sleep ) )
        // RR (Quantum ) finsh when  (Q =0 and sleep or (rem = 0 and end) )

      //  printf("New process with id %d is now being executed  \n",pid );
      //   printf(" remQunatum1 :  %d  \n",remQunatum );
        do
        {

            currentTiime=getClk();
           // printf("tempstart : %d     currentTiime: %d \n ",tempstart ,currentTiime );
            if (tempstart != currentTiime ) // check if there is clock happened 
            {
            //  printf("Sayed\n");
                    if ( currentTiime-tempstart==1  )
                    {
                        //raise(SIGCONT);
                        remQunatum--;
                      //  printf(" process with id %d has finhed 1 clock cycle  \n",pid );

//                          printf(" remQunatum :  %d  \n",remQunatum );

                        tempstart=getClk(); 
                    }
                   
                
            }
             currentTiime=getClk(); // the following if condition corrects if an iteruption occured at this line 
             if(currentTiime- tempstart >1)  
             {
                currentTiime=getClk();
                tempstart=currentTiime;
             }
        }while(   remQunatum !=0 && Quantum!=0 ); // if status != ENND

     printf(" process with id %d has finhed 1 running  \n",pid );
    //printf("MO yasser\n ");
    
    kill(getppid(),SIGUSR2);
    exit(0);

    destroyClk(false);

    return 0;
}

