#include"memory.h"
#include <stdlib.h>
#include <stdio.h>


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

Node * createNode(int startAddress,int size,int child){
    Node * temp=(Node *)malloc(sizeof(Node));
    temp->startAddress=startAddress;
    temp->Size=size;
    temp->isFree=1;
    temp->child=child;
    temp->Next =temp->Prev=(void*)0;
    return temp;
}


void freeNode(Node * ptr){
    free(ptr);
}


buddyList * initializeBuddySystem(){
    buddyList * temp=(buddyList*)malloc(sizeof(buddyList));
    temp->Head=createNode(0,1024,-1);
    return temp;
}

int isPowerOfTwo(int number) {
    // A number is a power of 2 if and only if it has exactly one '1' bit set in its binary representation.
    return (number > 0) && ((number & (number - 1)) == 0);
}

int nextPowerOfTwo(int number){
    if(isPowerOfTwo(number)){
        return number;
    }
    int ans=1;
    while(ans<number){
        ans<<=1;
    }
    return ans;
}

Node * Split(Node *ptr){
    Node * temp1=createNode(ptr->startAddress,ptr->Size/2,1);
    Node * temp2=createNode(ptr->startAddress+(ptr->Size/2),(ptr->Size/2),2);

    temp1->Prev=ptr->Prev;
    if(temp1->Prev){
        temp1->Prev->Next=temp1;
    }
    temp1->Next=temp2;
    temp2->Prev=temp1;
    temp2->Next=ptr->Next;
    if(temp2->Next){
        temp2->Next->Prev=temp2;
    }
    freeNode(ptr);
    return temp1;
}


Node * allocateMemory(buddyList * ptr,int sizeOfMemory){
    int fixedSize=nextPowerOfTwo(sizeOfMemory);
    Node * temp=ptr->Head;
    int min=1024;
    while(temp){
        if(temp->isFree && fixedSize<=temp->Size)
        min=MIN(temp->Size,min);
        if(temp->Size==fixedSize && temp->isFree){
            temp->isFree=0;
            return temp;
        }
        temp=temp->Next;
    }
    temp=ptr->Head;
    while(temp){
        if(temp->Size==min && temp->isFree){

            while(temp->Size>fixedSize &&temp->Size>1){
                temp=Split(temp);
                if(!temp->Prev){
                    ptr->Head=temp;
                }
                // printMemory(ptr);
                // puts("*********");
            }
            if(temp->Size==fixedSize){
                
                temp->isFree=0;
                
                return temp;
            }
            else{
                break;
            }
        }
        temp=temp->Next;
    }
    return (void *)0;
}

Node * Merge(buddyList * buddy,Node * ptr){
    int newSize=ptr->Size*2;
    Node * temp2=ptr->Next;
    Node * temp=createNode(ptr->startAddress,newSize,-1);
    temp->Prev=ptr->Prev;
    if(temp->Prev){
        temp->Prev->Next=temp;
    }
    temp->Next=temp2->Next;
    if(temp->Next){
       temp->Next->Prev=temp;
    }
    if(ptr->Prev){ 
        if(ptr->Prev->child==1){
            temp->child=2;
        }
        else{
            temp->child=1;
        }
        
    }
    else{
            temp->child=1;
            buddy->Head=temp;
    }
    freeNode(ptr);
    freeNode(temp2);
    return temp;
}

void RearrangeMemory(buddyList * ptr){
    Node * temp=ptr->Head;
    while(temp && temp->Next){
        if(temp->isFree && temp->Next->isFree && temp->child==1 && temp->Next->child==2 && temp->Size==temp->Next->Size){
          temp=Merge(ptr,temp);
          temp=ptr->Head;
        }
        else 
        temp=temp->Next;
    }
}

int dellocateMemory(buddyList * ptr,Node * ptr2){
    Node * temp=ptr->Head;
    while(temp){
        if(temp==ptr2){
            temp->isFree=1;
            RearrangeMemory(ptr);       
            return 1;
        }
        temp=temp->Next;
    }   
    return 0;    
}

void printMemory(buddyList * ptr){
    Node * temp=ptr->Head;
    while(temp){
        printf("startaddress %d size %d child %d isFree %d \n",temp->startAddress,temp->Size,temp->child,temp->isFree);
        temp=temp->Next;
    }
}
