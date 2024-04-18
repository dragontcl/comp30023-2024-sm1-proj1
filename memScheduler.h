//
// Created by dragontang on 17/04/24.
//

#ifndef MEMSCHEDULER_H
#define MEMSCHEDULER_H
#include "process.h"

typedef enum memoryType{
  INFINITE,
  FIRST_FIT,
  PAGED,
  VIRTUAL,
  INVALID
} memoryType_t;
typedef enum pageFreeType{
  FULL,
  PARTIAL
} pageFreeType_t;

int findProcessAndInsertAtEnd(processLL_t *processList, processLL_t *runningList, const int clock, const int quantum);
int turnAroundTime(const process_t *finishedProcesses);
double avgOverheadTime(const processLL_t *finishedProcesses);
double maxOverheadTime(const processLL_t *finishedProcesses);
int avgTurnAroundTime(const processLL_t *finishedProcesses);
int firstFitAllocate(process_t* process, int memory[]);
int calculateUsedMemory(int memory[], const memoryType_t type);
void blockMemDeallocate(process_t *process, int memory[]);
void printMemPage(const process_t * process);
void pagedMemDeallocate(process_t *process, int memory[], const int clock);
int pagedMemAllocate(process_t *process, int memory[]);
int virtualMemAllocate(const processNode_t *processNode, int memory[], int clock);
void virtualMemDeallocate(process_t *process, int deallocateAmt, int memory[],const int clock, pageFreeType_t type);
void rrMemoryScheduler(processLL_t *processList, const int quantum, const memoryType_t memoryType);
#endif //MEMSCHEDULER_H
