#ifndef IO_FUNCTIONS_HEADER
#define IO_FUNCTIONS_HEADER

#include "DataStructures.h"

#define MAX_LINE_LENGTH 256

void ReadInputFile(char *filename,struct Queue* q,int *NumberOfProcesses);

int ChooseAlgorithm(int *Quntam);

#endif