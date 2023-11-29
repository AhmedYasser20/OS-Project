#include "DataStructures.h"
#include <stdlib.h>

void makeArray(int N,struct array arr) // this fution takes size of the array and struct of of type array and reserve places for the valeus
{
    arr.size=N;
    arr.ptr=(int*)malloc(N * sizeof(int));
}

