#include "headers.h"

/* Modify this file as needed*/
int remainingtime;   // Time that current process will finsh using cpu

int main(int agrc, char * argv[])
{
    initClk();
    int start =getClk();
    remainingtime=getClk()+ *(argv[1]) ;

    int QueueKey=msgget(MSG_QUEUE_SCHEDULER_PROCESS_KEY,0666 | IPC_CREAT);
    int pid=getpid();
    MessageBetweenProcessAndScheduler temp;
    int rev=msgrcv(QueueKey,&temp,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),pid,!IPC_NOWAIT);
    printf("PID %d \n",pid);
    
    do{
        if(temp.Order==START){
           puts("122\n"); 
           int clktemp=getClk();
           temp.remainingtime--;
           temp.ExceTime++;
           temp.Qutam--;
           while(clktemp==getClk());   
        }
        rev=msgrcv(QueueKey,&temp,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)),pid,IPC_NOWAIT);
        if(rev!=-1){
           
            if(temp.Order==END){
                break;
            }
        }
        if(temp.Qutam==0){
            MessageBetweenProcessAndScheduler temp2;
            temp2.type=pid;
            temp2.ExceTime=temp.ExceTime;
            temp2.remainingtime=temp.remainingtime;
            temp2.Qutam=-1;
            msgsnd(QueueKey,&temp2,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),!IPC_NOWAIT);
            kill(getppid(),SIGUSR2);
        }

    }while(true);

   


    destroyClk(false);
    
    return 0;
}
