#include "IO_Functions.h"
#include <stdio.h>



void ReadInputFile(char *filename){
    FILE *_ptr;
    _ptr=fopen(filename,'r');

    if(_ptr==NULL){
        puts("Error in opening file");
    }

    
}