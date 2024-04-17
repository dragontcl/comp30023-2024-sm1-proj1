#include "memScheduler.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXIMUM_MEMORY 2048
int findProcessAndInsert(processLL_t *process_list, processLL_t *running_list, const int clock) {
    int found = 0;
    processNode_t *currentNode = process_list->head;
    processNode_t *headRunning = running_list->head;
    processNode_t *prevNode = NULL;
    processNode_t *lastInserted = NULL;
    while (currentNode != NULL) {
        processNode_t *nextNode = currentNode->next;
        if (currentNode->process->arrivalTime <= clock) {
            found = 1;
            if (prevNode != NULL) {
                prevNode->next = currentNode->next;
            } else {
                process_list->head = currentNode->next;
            }
            currentNode->next = NULL;
            if (lastInserted == NULL) {
                if (running_list->head != NULL) {
                    currentNode->next = running_list->head;
                }
                running_list->head = currentNode;
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
        running_list->tail = lastInserted;
    }
    return found;
}
int findProcessAndInsertAtEnd(processLL_t *process_list, processLL_t *running_list, const int clock, const int quantum) {
    int found = 0;
    const processNode_t *currentNode = process_list->head;
    while(currentNode != NULL) {
        const processNode_t *nextNode = currentNode->next;
        if(currentNode->process->arrivalTime <= clock+quantum) {
            found = 1;
            addNodeToEnd(currentNode->process, running_list);
            removeNode(currentNode->process, process_list);
        }
        currentNode = nextNode;
    }
    return found;
}
int turnAroundTime(const process_t *finishedProcesses) {
    return finishedProcesses->completedTime - finishedProcesses->arrivalTime;
}
double avgOverheadTime(const processLL_t *finishedProcesses){
        double totalOverheadTime = 0;
        int totalProcesses = 0;
        const processNode_t *currentNode = finishedProcesses->head;
        while (currentNode != NULL) {
                totalOverheadTime += ((double)turnAroundTime(currentNode->process)/ currentNode->process->runTime);
                totalProcesses++;
                currentNode = currentNode->next;
        }
        return round(totalOverheadTime / totalProcesses * 100) / 100;
}
double maxOverheadTime(const processLL_t *finishedProcesses){
    double maxOverheadTime = 0;
    const processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        if(((double)turnAroundTime(currentNode->process)/ currentNode->process->runTime) > maxOverheadTime){
            maxOverheadTime = ((double)turnAroundTime(currentNode->process)/ currentNode->process->runTime);
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
        totalTurnAroundTime += turnAroundTime(currentNode->process);
        totalProcesses++;
        currentNode = currentNode->next;
    }
    return (int)ceil((double)totalTurnAroundTime / totalProcesses);
}
void rrInfiniteMem(processLL_t *process_list, const int quantum) {
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    const process_t *lastRunningProcess = NULL;
    int clock = 0;
    while (!isEmpty(process_list) || !isEmpty(runningProcesses)) {
        //if(findProcessAndInsert(process_list, runningProcesses, clock)) {
            //printf("NEW PROCESS GOT INSERTED\n");
        //}
        if(findProcessAndInsertAtEnd(process_list, runningProcesses, clock, quantum)) {
            //printf("NEW PROCESS GOT INSERTED\n");
        }
        const int timeSlice = quantum; // Always use full quantum time
        // there are processes that can be ran in the round robin scheduler
        if (!isEmpty(runningProcesses)) {
            // get the first process in the list
            const processNode_t *currentNode = runningProcesses->head;
            while (currentNode != NULL && currentNode->process->arrivalTime > clock) {
                currentNode = currentNode->next;  // Skip processes not yet arrived
            }
            if (currentNode == NULL) {
                clock += timeSlice;
                continue;
            }
            currentNode->process->status = RUNNING; // allow process to run
            // print on context switch
            if(currentNode->process->status == RUNNING) {
                //if (lastRunningProcess != currentNode->process) {
                    printf("%d,RUNNING,process-name=%s,remaining-time=%d\n",
                           clock, currentNode->process->name, currentNode->process->remainingTime);
                    //lastRunningProcess = currentNode->process;
                //}
                currentNode->process->remainingTime -= timeSlice;
            }
            // check if process is finished && is running
            if (currentNode->process->remainingTime <= 0 && currentNode->process->status == RUNNING) {
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
            clock+=timeSlice;;
        }
    }
    printf("Turnaround time %d\n", avgTurnAroundTime(finishedProcesses));
    printf("Time overhead %.2f %.2f\n", maxOverheadTime(finishedProcesses) ,avgOverheadTime(finishedProcesses));
    printf("Makespan %d\n", clock);
    destroyLL(runningProcesses);
    destroyLL(finishedProcesses);
}
int firstFitAllocate(process_t* process, int memory[]) {
    const int memoryRequired = process->memorySize;
    if(process->memory.status == ALLOCATED) { // already allocated memory no need to allocate again
        return 1;
    }
    for(int i = 0; i < MAXIMUM_MEMORY; i++ ) {
        if(memory[i] == 0) {
            int block = 0;
            for(int j = i; j < MAXIMUM_MEMORY; j++) {
                if(memory[j] == 0) {
                    block++;
                } else {
                    break;
                }
            }
            if(block >= memoryRequired) {
                for(int j = i; j < i + memoryRequired; j++) {
                    memory[j] = 1;
                }
                process->memory.start = i;
                process->memory.end = i + memoryRequired - 1;
                process->memory.status = ALLOCATED;
                return 1; //successfully allocated memory
            }
        }
    }
    return 0;//failed to allocate memory
}
int firstFitDeallocate(process_t* process, int memory[]) {
    if(process->memory.status == UNALLOCATED) { // already deallocated memory no need to deallocate again
        return 1;
    }
    for(int i = process->memory.start; i <= process->memory.end; i++) {
        memory[i] = 0;
    }
    process->memory.start = -1;
    process->memory.end = -1;
    process->memory.status = UNALLOCATED;
    return 1;
}
int calculateUsedMemory(int memory[]) {
    int usedMemory = 0;
    for (int i = 0; i < MAXIMUM_MEMORY; i++) {
        if (memory[i] == 1) {
            usedMemory++;
        }
    }
    double percentage = ((double) usedMemory / MAXIMUM_MEMORY) * 100.0;
    return (int)(percentage + 0.5); //do some casting magic to round
}
void rrFirstFitMem(processLL_t *process_list, const int quantum) {
    int memory[MAXIMUM_MEMORY];
    // initialize memory to 0
    for (int i = 0; i < MAXIMUM_MEMORY; i++) {
        memory[i] = 0;
    }
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    processLL_t *testProcess = createLL();
    const process_t *lastRunningProcess = NULL;
    int clock = 0;
    while (!isEmpty(process_list) || !isEmpty(runningProcesses)) {
        //if(findProcessAndInsert(process_list, runningProcesses, clock)) {
            //printf("NEW PROCESS GOT INSERTED\n");
        //}
        if(findProcessAndInsertAtEnd(process_list, runningProcesses, clock, quantum)) {
            //printf("NEW PROCESS GOT INSERTED\n");
        }
        const int timeSlice = quantum; // Always use full quantum time

        // there are processes that can be ran in the round robin scheduler
        if (!isEmpty(runningProcesses)) {
            // get the first process in the list

            const processNode_t *currentNode = runningProcesses->head;
            //const int timeSlice = quantum; // Always use full quantum time
            if(firstFitAllocate(currentNode->process, memory)) {
                currentNode->process->status = RUNNING; // allow process to run
            }
            else {
                moveNodeToEnd(currentNode->process, runningProcesses);
            }
            // print on context switch
            if(currentNode->process->status == RUNNING) {
                if (lastRunningProcess != currentNode->process) {
                    printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%,allocated-at=%d\n",
                           clock, currentNode->process->name, currentNode->process->remainingTime,
                           calculateUsedMemory(memory),
                           currentNode->process->memory.start);
                    lastRunningProcess = currentNode->process;
                }
                currentNode->process->remainingTime -= timeSlice;
            }
            // check if process is finished && is running
            if (currentNode->process->remainingTime <= 0 && currentNode->process->status == RUNNING) {
                currentNode->process->completedTime = clock + timeSlice;
                printf("%d,FINISHED,process-name=%s,proc-remaining=",
                       currentNode->process->completedTime, currentNode->process->name);
                firstFitDeallocate(currentNode->process, memory);
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
            clock+=timeSlice;;
        }
    }
    printf("Turnaround time %d\n", avgTurnAroundTime(finishedProcesses));
    printf("Time overhead %.2f %.2f\n", maxOverheadTime(finishedProcesses) ,avgOverheadTime(finishedProcesses));
    printf("Makespan %d\n", clock);
    destroyLL(runningProcesses);
    destroyLL(finishedProcesses);
}