#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MemoryManagement.h"


void startGC();
void copyRootObjects();
ObjRef copyObjectToFreeMem(ObjRef org);
size_t sizeOfObject(ObjRef object);
ObjRef relocate(ObjRef orig); 
void scan(void);

void * passiveDestinationHalfMemory;
void * activeDestinationHalfMemory;
void * freePointer;
void * DestinationMemoryPointer;
void * endOfHalfMemory;
int sizeOfHeap;

void reserveMemoryinHeap(int n){
    sizeOfHeap = n;
    void * memory = malloc(n * 1024);
    activeDestinationHalfMemory = freePointer = DestinationMemoryPointer = memory;
    passiveDestinationHalfMemory = endOfHalfMemory = memory + ((n * 1024) / 2);
}

void * reserveMemoryForStack(int n){
    return malloc(n * 1024);
}

void * reserveMemoryForSDA(int n){
    return malloc(n * 1024);
}

void * reserveMemoryForPM(int n){
    return malloc(n * sizeof(unsigned int));
}

void * reserveMemoryForAnObject(size_t Objsize){
    if ((freePointer + Objsize) > endOfHalfMemory)
    {
        startGC();
        if ((freePointer + Objsize) > endOfHalfMemory)
        {
            fatalError("heap overflow");
        }
    }
    
    void * object = freePointer;
    freePointer += Objsize;
    
    return object;
}

void startGC(){
    void * tmpptr = activeDestinationHalfMemory;
    activeDestinationHalfMemory = passiveDestinationHalfMemory;
    passiveDestinationHalfMemory = tmpptr;

    DestinationMemoryPointer = freePointer = activeDestinationHalfMemory; 
    endOfHalfMemory = DestinationMemoryPointer + ((sizeOfHeap * 1024) / 2);

    copyRootObjects();
    scan();
}

ObjRef copyObjectToFreeMem(ObjRef org)
{
    size_t size = sizeOfObject(org);
    if (freePointer + size > endOfHalfMemory)
        fatalError("memory not enough for copping the object - should never happen.\n");
    ObjRef copy;
    
    if (size > 0)
        memcpy((ObjRef)freePointer, org, size);
    copy = freePointer;
    freePointer += size;
    return copy;
}

void scan(void){
    void * scan = DestinationMemoryPointer;  // der vor dem Wechsel aktiveHalbspeicher
    
    while (scan < freePointer)
    {
        ObjRef object = (ObjRef) scan;
        if (IS_PRIMITIVE(object) == FALSE)
        {
            for (int i = 0; i < GET_ELEMENT_COUNT(object); i++){
                GET_REFS_PTR(object)[i] = relocate(GET_REFS_PTR(object)[i]);
            }
        }
        scan += sizeOfObject(object);
    }
}

ObjRef relocate(ObjRef orig){
    ObjRef copy;
    if (orig == NULL) {
        /* relocate(nil) = nil */
        copy = NULL;
        } else {
        if(HEART_BROKEN(orig) == 1){
            /* Objekt ist bereits kopiert */
            copy=orig->forward_Pointer;
        } else {
            /* Objekt muss noch kopiert werden*/
            copy = copyObjectToFreeMem(orig);
            orig->size = 0;
            orig->size |= MSBH;
            orig->forward_Pointer = copy;
        }
    }
    return copy;
}

size_t sizeOfObject(ObjRef object){
    if (IS_PRIMITIVE(object) == TRUE){
        return sizeof(*object)+(object->size * sizeof(unsigned char)); 
    }
    else if(IS_PRIMITIVE(object) == FALSE){
        return sizeof(*object)+(GET_ELEMENT_COUNT(object) * 8);
    } 
    else{
        fatalError("can't know which type the object has - should never happen");
    }
}

void copyRootObjects()
{
    for (int i=sp-1; i>=0; i--){
        if (stack[i].isDefined == false || stack[i].isObjRef == false)
            continue;
        stack[i].u.objRef = relocate(stack[i].u.objRef);
    }

    for (int i = 0; i < info[3]; i++){
        SDA[i] = relocate(SDA[i]);
    }

    if (RVR != NULL){
        RVR = relocate(RVR);
    }
    if (bip.op1 != NULL){
        bip.op1 = relocate(bip.op1);
    }
    if (bip.op2 != NULL){
        bip.op2 = relocate(bip.op2);
    }
    if (bip.rem != NULL){
        bip.rem = relocate(bip.rem);
    }
    if (bip.res != NULL){
        bip.res = relocate(bip.res);
    }
}