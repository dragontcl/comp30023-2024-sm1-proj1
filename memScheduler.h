//
// Created by dragontang on 17/04/24.
//

#ifndef MEMSCHEDULER_H
#define MEMSCHEDULER_H
#include "process.h"
int findProcessAndInsert(processLL_t *processQueue, processLL_t *runningProcesses, const int clock);
int turnAroundTime(const process_t *finishedProcesses);
double avgOverheadTime(const processLL_t *finishedProcesses);
double maxOverheadTime(const processLL_t *finishedProcesses);
int avgTurnAroundTime(const processLL_t *finishedProcesses);
int firstFitAllocate(process_t* process, int memory[]);
void rrInfiniteMem(processLL_t *process_list, int quantum);
void rrFirstFitMem(processLL_t *process_list, int quantum);
void rrPagedMem(processLL_t *process_list, int quantum);
void rrVirtualMem(processLL_t *process_list, int quantum);
#endif //MEMSCHEDULER_H
