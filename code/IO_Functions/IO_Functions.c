#include "IO_Functions.h"
#include <stdio.h>



void ReadInputFile(char *filename){
   FILE *_ptr;
   _ptr=fopen(filename,"r");

    if(_ptr==NULL){
        puts("Error in opening file");
    }
    char *_BufferChar=NULL;
    size_t  _SizeOFBuffer=0;
    while(getline(&_BufferChar,&_SizeOFBuffer,_ptr)!=-1){
        //printf("Size = %ld\n",_SizeOFBuffer);
        printf("%s \n",_BufferChar);
    }
}