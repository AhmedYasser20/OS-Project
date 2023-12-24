#pragma once

#include "DataStructures.h"





Node * createNode(int startAddress,int size,int child);

void freeNode(Node * ptr);

int isPowerOfTwo(int number);

int nextPowerOfTwo(int number);

buddyList * initializeBuddySystem();

//Allocate = dellocate = merge = Slipt

Node * Split(Node *ptr);

Node * Merge(buddyList * buddy,Node * ptr);

Node * allocateMemory(buddyList * ptr,int sizeOfMemory);

void RearrangeMemory(buddyList * ptr);

int dellocateMemory(buddyList * ptr,Node * ptr2);

void printMemory(buddyList * ptr);