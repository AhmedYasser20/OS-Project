#include "ArrayOfPCB.h"
#include <malloc.h>

ProcessPCB * CreatePCB_Array(){
    ProcessPCB * array=(ProcessPCB*) malloc(MAX_NUM_OF_PROCESSES*(sizeof(ProcessPCB)));
    return array;
}

void DestoryedPCB_Array(ProcessPCB * p){
    free(p);
}