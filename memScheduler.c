#include "memScheduler.h"
#include <stdio.h>
#include <math.h>


void findProcessAndInsert(processLL_t *processQueue, processLL_t *runningProcesses, int clock) {
    processLL_t *tempList = createLL();
    processNode_t *currentNode = processQueue->head;
    processNode_t *nextNode = NULL;
    while (currentNode != NULL) {
        nextNode = currentNode->next;
        if (currentNode->process->arrivalTime == clock) {
            addNodeToEnd(currentNode->process, runningProcesses);
            removeNode(currentNode->process, processQueue);
        }
        currentNode = nextNode;
    }
    // Add tempList to the start of runningProcesses
    if (!isEmpty(tempList)) {
        processNode_t *tempNode = tempList->head;
        while (tempNode != NULL) {
            addNodeToStart(tempNode->process, runningProcesses);
            tempNode = tempNode->next;
        }
    }
    destroyLL(tempList);
}
int TurnAroundTime(const process_t *finishedProcesses) {
    return finishedProcesses->completedTime - finishedProcesses->arrivalTime;
}
double avgOverheadTime(const processLL_t *finishedProcesses){
        double totalOverheadTime = 0;
        int totalProcesses = 0;
        processNode_t *currentNode = finishedProcesses->head;
        while (currentNode != NULL) {
                totalOverheadTime += ((double)TurnAroundTime(currentNode->process)/ currentNode->process->runTime);
                totalProcesses++;
                currentNode = currentNode->next;
        }
        return ((double)totalOverheadTime / totalProcesses);
}
double MaxOverheadTime(const processLL_t *finishedProcesses){
    double maxOverheadTime = 0;
    processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        if(((double)TurnAroundTime(currentNode->process)/ currentNode->process->runTime) > maxOverheadTime){
            maxOverheadTime = ((double)TurnAroundTime(currentNode->process)/ currentNode->process->runTime);
        }
        currentNode = currentNode->next;
    }
    return maxOverheadTime;
}
int avgTurnAroundTime(const processLL_t *finishedProcesses) {
    int totalTurnAroundTime = 0;
    int totalProcesses = 0;
    processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        totalTurnAroundTime += TurnAroundTime(currentNode->process);
        totalProcesses++;
        currentNode = currentNode->next;
    }
    return (int)ceil((double)totalTurnAroundTime / totalProcesses);
}


void rrInfiniteMem(processLL_t *processQueue, int quantum) {
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    process_t *lastRunningProcess = NULL;
    int clock = 0;
    while (!isEmpty(processQueue) || !isEmpty(runningProcesses)) {
        findProcessAndInsert(processQueue, runningProcesses, clock);
        if (!isEmpty(runningProcesses)) {
            processNode_t *currentNode = runningProcesses->head;
            int timeSlice = (currentNode->process->remainingTime < quantum)
            ? currentNode->process->remainingTime
            : quantum;
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
                processNode_t *temp = runningProcesses->head;
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

    printf("Turnaround time: %d\n", avgTurnAroundTime(finishedProcesses));
    printf("Time overhead %.2f %.2f\n", MaxOverheadTime(finishedProcesses) ,avgOverheadTime(finishedProcesses));
    printf("Makespan: %d\n", clock);
}

