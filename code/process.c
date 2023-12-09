#include "headers.h"

/* Modify this file as needed*/
int remQunatum; // Time that current process will finsh using cpu
int I_getWait = 0;
int ALG = -1;
#define REDY 10
#define WIT  20
#define ENND 30
int status = 0;
int main(int agrc, char *argv[])
{
    initClk();
    int start = getClk();
    int tempstart=start;
    int Quantum;    // Time slot of RR 
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
          remQunatum =  * (argv[1]);

    }
   

    signal(SIGUSR2, SignalHandlerProcessesStop);
    signal(SIGUSR1, SignalHandlerProcessesCont);

   
    int pid = getpid();

     //MessageBetweenProcessAndScheduler temp;
    // int QueueKey = msgget(MSG_QUEUE_FROM_SCHEDULER_TO_PROCESS_KEY, 0666 | IPC_CREAT);  // sch -> P


    // msgrcv(QueueKey, &temp, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), pid, IPC_NOWAIT);

        // HPF (revived rem time) finsh when  rem =0
        //SRT  (revived rem time) finsh when  (rem =0 or (interputed and process sleep ) )
        // RR (Quantum ) finsh when  (Q =0 and sleep or (rem = 0 and end) )

       
        do
        {
            if (tempstart != getClk()) // check if there is clock happened 
            {
              
                    if (!I_getWait && (getClk()-tempstart==1 ) )
                    {
                        raise(SIGCONT);
                        remQunatum--;
                        tempstart=getClk();
                    }
                    else if (I_getWait)
                    {
                       raise(SIGSTOP);
                    }
                
            }
        }while(status !=30); // if status != ENND

    /*
    else if (ALG == 3)
    {

        int QueueKey2 = msgget(MSG_QUEUE_FROM_PROCESS_TO_SCHEDULER_KEY, 0666 | IPC_CREAT); // P -> sch

        MessageBetweenProcessAndScheduler temp;
        int rev = msgrcv(QueueKey, &temp, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), pid, !IPC_NOWAIT);

        do
        {

            if (I_getWait)
            {
                temp.Order = WAIT;
                raise(SIGSTOP);
            }
            else
            {
                raise(SIGCONT);
            }
            if (temp.Order == WAIT)
            {
                msgrcv(QueueKey, &temp, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), pid, !IPC_NOWAIT); // Sleep
                // المفروض نحط raise(SIGSTOP);
            }
            if (temp.Order == START)
            {
                int clktemp = getClk();
                temp.remainingtime--;
                temp.ExceTime++;
                temp.Qutam--;
                while (clktemp == getClk())
                    ;
            }
            if (temp.Order == END)
            {
                break;
            }
            if ((temp.remainingtime == 0 || temp.Qutam == 0) && temp.Order == START)
            {
                temp2.type = pid;
                temp2.ExceTime = temp.ExceTime;
                temp2.remainingtime = temp.remainingtime;
                temp2.Qutam = -1;
                msgsnd(QueueKey2, &temp2, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), !IPC_NOWAIT);
                // printf("What i sned As process type %ld,Remtime=%d,ExceTime=%d,Qutm=%d,ORDER=%d \n",temp2.type,temp2.remainingtime,temp2.ExceTime,temp2.Qutam,temp2.Order);
                kill(getppid(), SIGUSR2);
                // temp.Order=END;
                msgrcv(QueueKey, &temp, (sizeof(temp.ExceTime) + sizeof(temp.Order) + sizeof(temp.remainingtime) + sizeof(temp.Qutam)), pid, !IPC_NOWAIT);
                // printf("What i Rev As process type %ld,Remtime=%d,ExceTime=%d,Qutm=%d,ORDER=%d \n",temp.type,temp.remainingtime,temp.ExceTime,temp.Qutam,temp.Order);
            }
        } while (true);
    }*/

    destroyClk(false);

    return 0;
}

/*
    Function name: SignalHandlerProcessesStop
    Description:    Signal Handler for Signal User 2 -
    Input: int
    Output: void

*/

void SignalHandlerProcessesStop(int sig)
{
    I_getWait = 1; // if 0 your waiting
}

/*
    Function name: SignalHandlerProcessesCont
    Description:    Signal Handler for Signal User 2 -
    Input: int
    Output: void

*/

void SignalHandlerProcessesCont(int sig)
{
    I_getWait = 0;   // if 0 your running
}
