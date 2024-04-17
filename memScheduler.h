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
  VIRTUAL
} memoryType_t;
typedef enum pageFreeType{
  FULL,
  PARTIAL
} pageFreeType_t;

int findProcessAndInsertAtEnd(processLL_t *process_list, processLL_t *running_list, const int clock, const int quantum);
int turnAroundTime(const process_t *finishedProcesses);
double avgOverheadTime(const processLL_t *finishedProcesses);
double maxOverheadTime(const processLL_t *finishedProcesses);
int avgTurnAroundTime(const processLL_t *finishedProcesses);
int firstFitAllocate(process_t* process, int memory[]);
void rrInfiniteMem(processLL_t *process_list, int quantum);
void rrFirstFitMem(processLL_t *process_list, int quantum);
void rrPagedMem(processLL_t *process_list, int quantum);
void rrVirtualMem(processLL_t *process_list, int quantum);
int calculateUsedMemory(int memory[], const memoryType_t type);
void firstFitDeallocate(process_t *process, int memory[]);
void printMemPage(const process_t * process);
void pagedMemDeallocate(process_t *process, int memory[], const int clock);
int pagedMemAllocate(process_t *process, int memory[]);
int virtualMemAllocate(processNode_t *process_node, int memory[], int clock);
void virtualMemDeallocate(process_t *process, int deallocateAmt, int memory[],
                          const int clock, pageFreeType_t type);
#endif //MEMSCHEDULER_H
