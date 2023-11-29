#include "DataStructures.h"
#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();

    // TODO implement the scheduler :)

    // RR
    int SchedullingAlgo = 1;
    if (SchedullingAlgo == 0)
    {

        int numberOfProcess, currentTime, Quantum = 1;

        struct array arrProcessesIds, arrArrivalTimeOfProcesses, arrRunTimeOfProcesses, arrRemaingTimeOfProcesses; // array carries process need to be executed if the id =-1 then the process was here terminated
        // makeArray(numberOfProcess, arrProcessesIds);
        // makeArray(numberOfProcess, arrArrivalTimeOfProcesses);
        // makeArray(numberOfProcess, arrRunTimeOfProcesses);
        int nextArrivalTimeindex = 0;                                              // this variable indicates index of  the next arrival process in arrival array
        int nextArrivalTime = arrArrivalTimeOfProcesses.ptr[nextArrivalTimeindex]; // this variable indicates when the time of  arrival of the next process
        int numberOfArrivedProcess = 0;
        for (int i = 0; i < numberOfProcess; i++)
        {
            arrProcessesIds.ptr[i] = -1;
        }

        while (numberOfProcess)
        {

             //here we check if process arrived
            currentTime = getClk();
            if (nextArrivalTime > currentTime)
            {
                sleep(nextArrivalTime - currentTime);
            }
            if (currentTime >= nextArrivalTime)
            {
                numberOfArrivedProcess++;
                nextArrivalTimeindex++;
                nextArrivalTime = arrArrivalTimeOfProcesses.ptr[nextArrivalTimeindex];
            }
            if (numberOfArrivedProcess)
            {

                for (int i = 0; i < numberOfProcess; i++)
                {
                    if (arrProcessesIds.ptr[i] != -1)
                    {
                        if (Quantum >= arrRemaingTimeOfProcesses.ptr[i])
                        {
                            sleep(Quantum);
                            arrRemaingTimeOfProcesses.ptr[i] = arrRemaingTimeOfProcesses.ptr[i] - Quantum;
                            if (arrRemaingTimeOfProcesses.ptr[i] == 0)
                            {
                                arrProcessesIds.ptr[i] = -1;
                            }
                        }
                        else
                        {
                            sleep(arrRemaingTimeOfProcesses.ptr[i]);
                            arrRemaingTimeOfProcesses.ptr[i] = 0;
                            arrProcessesIds.ptr[i] = -1;
                        }
                    }
                }
            }

            //here we check if process arrived
            currentTime = getClk();
            if (nextArrivalTime > currentTime)
            {
                sleep(nextArrivalTime - currentTime);
            }
            if (currentTime >= nextArrivalTime)
            {
                numberOfArrivedProcess++;
                nextArrivalTimeindex++;
                nextArrivalTime = arrArrivalTimeOfProcesses.ptr[nextArrivalTimeindex];
            }


        }
    }
    //HPF
    else if(SchedullingAlgo == 1)// ana sayed
    {  //Input process ids ,Arrival time ,Priority , running time
        int time;
        int Pid;
        char* state;
        int ArriveT;
        int TotalT;
        int RemainingT;
        int ResponseT;
        int TA;
        int WAT;
        
        int RunningT;
        int waitingT;

        int  currentTime = getClk();
        int rec_val;
        int QueueKey =msgget(MSG_QUEUE_GENERATOR_SCHEDULER_KEY,0666 | IPC_CREAT);
        
        struct MsgGeneratorScheduler temp;
        int NumberOfProcesses =5;

        while(NumberOfProcesses)
        {   
            rec_val = msgrcv(QueueKey, &temp, sizeof(temp.p), 1 , !IPC_NOWAIT);
            while(rec_val != -1 )// try to make the queue free of processes
            {
                rec_val = msgrcv(QueueKey, &temp, sizeof(temp.p), 1 , IPC_NOWAIT);
                printf("id=%d\n",temp.p.id);
            }

            rec_val = msgrcv(QueueKey, &temp, sizeof(temp.p), 1 , !IPC_NOWAIT);
            printf("id=%d\n",temp.p.id);


            NumberOfProcesses--;
        }
       
       
       //output total time ,seq of the runned processes
    }
    // upon termination release the clock resources.

    destroyClk(1);
}
