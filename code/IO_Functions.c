#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "QueueOfProcesses.h"
#include "IO_Functions.h"

char ChooseAlgorithm(char *Quntam){
    char choice;
    do{
    printf("Please Choose a one of those Algorithms\n");
    printf("1-Non-Preemptive Highest Priority First (HPF)\n");
    printf("2-Shortest Remaining time Next (SRTN)\n");
    printf("3-Round Robin (RR)\n");
    printf("Enter Your Choice: ");
    scanf("%c",&choice);
    if(choice=='3'){
        printf("Please Quntam of Round Robin: ");
        scanf(" %c",Quntam);
    }
    } while(choice!='1' && choice!='2' && choice!='3');
    return choice;
}

void ReadInputFile(char *filename,struct Queue* q,int *NumberOfProcesses){
   FILE *ptr;
   ptr=fopen(filename,"r");

    if(ptr==NULL){
        puts("Error in opening file");
    }
    char *BufferChar=NULL;
    size_t SizeOFBuffer=0;
    while(getline(&BufferChar,&SizeOFBuffer,ptr)!=-1){
       for (int i = 0; i < strlen(BufferChar); i++)
                printf("%c",BufferChar[i]);
        printf("\n");        
        if(BufferChar[0]=='#'){
            continue;
        }
        else{
            struct Process temp;
            int j=0;
            for (int i = 0; i < strlen(BufferChar); i++)
            {
                
                if(BufferChar[i]>='0' && BufferChar[i]<='9'){
                    int num=BufferChar[i]-'0';
                    i++;
                    while(BufferChar[i]>='0' && BufferChar[i]<='9'){
                      
                        num*=10;
                        num+=BufferChar[i]-'0';
                        i++;
                    }
                    switch (j)
                    {
                    case 0:
                        temp.id=num;
                        break;
                    case 1:
                        temp.ArriveTime=num;
                        break;
                    case 2:
                        temp.Runtime=num;
                        break;
                    case 3:
                        temp.Priority=num;
                        break;
                    default:
                        break;
                    }
                    j++;
                }
             
            }
          printf( "id %d Arrtime %d runtime %d prio %d \n",temp.id,temp.ArriveTime,temp.Runtime,temp.Priority);
          struct QueueProcessNode* Node=NewNode(temp);
          Push(q,temp);
          *NumberOfProcesses=*NumberOfProcesses+1;   
        }
    }
}