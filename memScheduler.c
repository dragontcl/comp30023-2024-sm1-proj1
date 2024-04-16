#include "memScheduler.h"
#include <stdio.h>
int isEmpty(processLL_t *list);

// used to find next ready process in queue
void findProcessAndInsert(processLL_t *processQueue, processLL_t *runningProcesses, int clock) {
    processNode_t *currentNode = processQueue->head;
    processNode_t *nextNode = NULL;
    while (currentNode != NULL) {
        nextNode = currentNode->next;
        if (currentNode->process->arrivalTime == clock) {
            printf("Process Name: %s | ", currentNode->process->name);
            printf("Arrival Time: %d | ", currentNode->process->arrivalTime);
            printf("Run Time: %d | ", currentNode->process->runTime);
            printf("Completed Time: %d | ", currentNode->process->completedTime);
            printf("Memory Size: %d KB\n", currentNode->process->memorySize);
            addNodeToStart(currentNode->process, runningProcesses);
            removeNode(currentNode->process, processQueue);
        }
        currentNode = nextNode;
    }
}

void rrInfiniteMem(processLL_t *processQueue, int quantum) {
    processLL_t* runningProcesses = createLL();
    processLL_t* finishedProcesses = createLL();
    int clock = 0;

    while (clock < 200) {
        findProcessAndInsert(processQueue, runningProcesses, clock);

        clock++;
    }
    printf("is empty pq %d\n", isEmpty(processQueue));
    printf("is empty rp %d\n", isEmpty(runningProcesses));
}


int isEmpty(processLL_t *list) {
    return list->head == NULL;
}
