#include "memScheduler.h"
#include <stdio.h>
#include <math.h>


void findProcessAndInsert(processLL_t *processQueue, processLL_t *runningProcesses, int clock) {
    processNode_t *currentNode = processQueue->head;
    processNode_t *nextNode = NULL;
    while (currentNode != NULL) {
        nextNode = currentNode->next;
        if (currentNode->process->arrivalTime == clock) {
            //TEST TO SEE IF PROCESS GETS FOUND
            //printf("Process Name: %s | Arrival Time: %d | Run Time: %d | Completed Time: %d | Memory Size: %d KB\n",
            //      currentNode->process->name, currentNode->process->arrivalTime, currentNode->process->runTime,
            //      currentNode->process->completedTime, currentNode->process->memorySize);
            addNodeToStart(currentNode->process, runningProcesses);
            removeNode(currentNode->process, processQueue);
        }
        currentNode = nextNode;
    }
}
int avgTurnAroundTime(processLL_t *finishedProcesses) {
    int totalTurnAroundTime = 0;
    int totalProcesses = 0;
    processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        totalTurnAroundTime += currentNode->process->completedTime - currentNode->process->arrivalTime;
        totalProcesses++;
        currentNode = currentNode->next;
    }
    return (int)ceil((double)totalTurnAroundTime / totalProcesses);
}

void rrInfiniteMem(processLL_t *processQueue, int quantum) {
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    processNode_t *lastRunningProcess = NULL;
    int clock = 0, timeSlice = 0;
    while (!isEmpty(processQueue) || !isEmpty(runningProcesses)) {
        findProcessAndInsert(processQueue, runningProcesses, clock);
        if (!isEmpty(runningProcesses)) {
            processNode_t *currentNode = runningProcesses->head;
            timeSlice = (currentNode->process->runTime < quantum) ? currentNode->process->runTime : quantum;
            if (lastRunningProcess != currentNode->process) {
                printf("%d,RUNNING,process-name=%s,remaining-time=%d\n",
                       clock,currentNode->process->name, currentNode->process->runTime);
                lastRunningProcess = currentNode->process;
            }
            currentNode->process->runTime -= timeSlice;
            if (currentNode->process->runTime <= 0) {
                currentNode->process->completedTime = clock + timeSlice;
                printf("%d,FINISHED,process-name=%s",
                      currentNode->process->completedTime,  currentNode->process->name);
                int remainingProcesses = 0;
                processNode_t *node = runningProcesses->head;
                while (node) {
                    if (node->process->runTime > 0) {
                        remainingProcesses++;
                    }
                    node = node->next;
                }
                printf(",proc-remaining=%d\n", remainingProcesses);
                addNodeToEnd(currentNode->process, finishedProcesses);
                removeNode(currentNode->process, runningProcesses);
                lastRunningProcess = NULL;
            } else {
                moveNodeToEnd(currentNode->process, runningProcesses);
            }
            clock += timeSlice;
        } else {
            clock++;
        }
    }
    printf("Turnaround time: %d\n", avgTurnAroundTime(finishedProcesses));

    printf("Makespan: %d\n", clock);
}
