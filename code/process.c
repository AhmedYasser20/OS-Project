#include "headers.h"

/* Modify this file as needed*/
int remainingtime;   // Time that current process will finsh using cpu

int main(int agrc, char * argv[])
{
    initClk();
    int start =getClk();
    remainingtime=getClk()+ *(argv[1]) ;

    int QueueKey=msgget(MSG_QUEUE_FROM_SCHEDULER_TO_PROCESS_KEY,0666 | IPC_CREAT);
    int QueueKey2=msgget(MSG_QUEUE_FROM_PROCESS_TO_SCHEDULER_KEY,0666 | IPC_CREAT);
   
    int pid=getpid();
    MessageBetweenProcessAndScheduler temp;
    int rev=msgrcv(QueueKey,&temp,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),pid,!IPC_NOWAIT);  
    
    do{
        if(temp.Order==WAIT){
              msgrcv(QueueKey,&temp,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),pid,!IPC_NOWAIT);
        }
        if(temp.Order==START){
           int clktemp=getClk();
           temp.remainingtime--;
           temp.ExceTime++;
           temp.Qutam--;
           while(clktemp==getClk());   
        }
        if(temp.Order==END){
            puts("END");
            break;
        }
        if(temp.Qutam==0 && temp.Order==START){
            MessageBetweenProcessAndScheduler temp2;
            temp2.type=pid;
            temp2.ExceTime=temp.ExceTime;
            temp2.remainingtime=temp.remainingtime;
            temp2.Qutam=-1;
            msgsnd(QueueKey2,&temp2,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),!IPC_NOWAIT);
            printf("What i sned As process type %ld,Remtime=%d,ExceTime=%d,Qutm=%d,ORDER=%d \n",temp2.type,temp2.remainingtime,temp2.ExceTime,temp2.Qutam,temp2.Order);
            kill(getppid(),SIGUSR2);
            //temp.Order=END;
            msgrcv(QueueKey,&temp,(sizeof(temp.ExceTime)+sizeof(temp.Order)+sizeof(temp.remainingtime)+sizeof(temp.Qutam)),pid,!IPC_NOWAIT);
            printf("What i Rev As process type %ld,Remtime=%d,ExceTime=%d,Qutm=%d,ORDER=%d \n",temp.type,temp.remainingtime,temp.ExceTime,temp.Qutam,temp.Order);
        }

    }while(true);

   


    destroyClk(false);
    
    return 0;
}
