#include "memScheduler.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


int findProcessAndInsert(processLL_t *processQueue, processLL_t *runningProcesses, int clock) {
    int found = 0;
    processNode_t *currentNode = processQueue->head;
    processNode_t *prevNode = NULL;
    processNode_t *lastInserted = NULL;
    while (currentNode != NULL) {
        processNode_t *nextNode = currentNode->next;
        if (currentNode->process->arrivalTime == clock) {
            found = 1;
            if (prevNode != NULL) {
                prevNode->next = currentNode->next;
            } else {
                processQueue->head = currentNode->next;
            }
            currentNode->next = NULL;
            if (lastInserted == NULL) {
                if (runningProcesses->head != NULL) {
                    currentNode->next = runningProcesses->head;
                }
                runningProcesses->head = currentNode;
                lastInserted = currentNode;
            } else {
                lastInserted->next = currentNode;
                lastInserted = currentNode;
            }
        } else {
            prevNode = currentNode;
        }
        currentNode = nextNode;
    }
    if (lastInserted != NULL && lastInserted->next == NULL) {
        runningProcesses->tail = lastInserted;
    }
    return found;
}

int TurnAroundTime(const process_t *finishedProcesses) {
    return finishedProcesses->completedTime - finishedProcesses->arrivalTime;
}
double avgOverheadTime(const processLL_t *finishedProcesses){
        double totalOverheadTime = 0;
        int totalProcesses = 0;
        const processNode_t *currentNode = finishedProcesses->head;
        while (currentNode != NULL) {
                totalOverheadTime += ((double)TurnAroundTime(currentNode->process)/ currentNode->process->runTime);
                totalProcesses++;
                currentNode = currentNode->next;
        }
        return round(totalOverheadTime / totalProcesses * 100) / 100;
}
double MaxOverheadTime(const processLL_t *finishedProcesses){
    double maxOverheadTime = 0;
    const processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        if(((double)TurnAroundTime(currentNode->process)/ currentNode->process->runTime) > maxOverheadTime){
            maxOverheadTime = ((double)TurnAroundTime(currentNode->process)/ currentNode->process->runTime);
        }
        currentNode = currentNode->next;
    }

    return round(maxOverheadTime * 100) / 100;
}
int avgTurnAroundTime(const processLL_t *finishedProcesses) {
    int totalTurnAroundTime = 0;
    int totalProcesses = 0;
    const processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        totalTurnAroundTime += TurnAroundTime(currentNode->process);
        totalProcesses++;
        currentNode = currentNode->next;
    }
    return (int)ceil((double)totalTurnAroundTime / totalProcesses);
}

void rrInfiniteMem(processLL_t *processQueue, const int quantum) {
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    const process_t *lastRunningProcess = NULL;
    int clock = 0;
    while (!isEmpty(processQueue) || !isEmpty(runningProcesses)) {
        if(findProcessAndInsert(processQueue, runningProcesses, clock)) {
            //printf("NEW PROCESS GOT INSERTED\n");
        }
        if (!isEmpty(runningProcesses)) {
            const processNode_t *currentNode = runningProcesses->head;
            const int timeSlice = quantum; // Always use full quantum time
            if (lastRunningProcess != currentNode->process) {
                printf("%d,RUNNING,process-name=%s,remaining-time=%d\n",
                       clock, currentNode->process->name, currentNode->process->remainingTime);
                lastRunningProcess = currentNode->process;
            }
            currentNode->process->remainingTime -= timeSlice;
            if (currentNode->process->remainingTime <= 0) {
                currentNode->process->completedTime = clock + timeSlice;
                printf("%d,FINISHED,process-name=%s,proc-remaining=",
                       currentNode->process->completedTime, currentNode->process->name);
                addNodeToEnd(currentNode->process, finishedProcesses);
                removeNode(currentNode->process, runningProcesses);
                lastRunningProcess = NULL;
                int i = 0;
                const processNode_t *temp = runningProcesses->head;
                while (temp != NULL) {
                    i++;
                    temp = temp->next;
                }
                printf("%d\n", i);
            } else {
                moveNodeToEnd(currentNode->process, runningProcesses);
            }
            clock += timeSlice;
        } else {
            clock++;
        }
    }
    printf("Turnaround time %d\n", avgTurnAroundTime(finishedProcesses));
    printf("Time overhead %.2f %.2f\n", MaxOverheadTime(finishedProcesses) ,avgOverheadTime(finishedProcesses));
    printf("Makespan %d\n", clock);
    destroyLL(runningProcesses);
    destroyLL(finishedProcesses);
}


