#ifndef _MemoryManagement_H_
#define _MemoryManagement_H_

#include "src/bigint/src/support.h"


void reserveMemoryinHeap(int n);
void * reserveMemoryForStack(int n);
void * reserveMemoryForAnObject(size_t size);
void * reserveMemoryForSDA(int n);
void * reserveMemoryForPM(int n);

#endif