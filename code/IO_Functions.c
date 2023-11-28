#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "QueueOfProcesses.h"
#include "IO_Functions.h"

void ReadInputFile(char *filename,struct Queue* q,int *NumberOfProcesses){
   FILE *_ptr;
   _ptr=fopen(filename,"r");

    if(_ptr==NULL){
        puts("Error in opening file");
    }
    char *_BufferChar=NULL;
    size_t  _SizeOFBuffer=0;
    while(getline(&_BufferChar,&_SizeOFBuffer,_ptr)!=-1){
        if(_BufferChar[0]=='#'){
            continue;
        }
        else{
            struct Process temp;
            int j=0;
            for (int i = 0; i < strlen(_BufferChar); i++)
            {
                if(_BufferChar[i]!=' '){
                    int num=atoi(&_BufferChar[i++]);

                    while(_BufferChar[i]!=' ')
                    {
                        num*=10;
                        num+=atoi(&_BufferChar[i++]);
                    } 
                    printf("%d\n",num);  
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
          struct QueueProcessNode* Node=NewNode(temp);
          Push(q,temp);
          *NumberOfProcesses++;   
        }
    }
}